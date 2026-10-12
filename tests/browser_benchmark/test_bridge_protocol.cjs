const http = require('http');
const net = require('net');
const crypto = require('crypto');
const { spawn } = require('child_process');

console.log('[*] Spawning bridge_daemon.cjs...');
const daemon = spawn('node', ['bridge_daemon.cjs'], { stdio: 'inherit' });

setTimeout(() => {
  console.log('[*] Testing HTTP GET /telemetry...');
  http.get('http://localhost:9099/telemetry', (res) => {
    let data = '';
    res.on('data', chunk => data += chunk);
    res.on('end', () => {
      const parsed = JSON.parse(data);
      console.log(`[+] HTTP Response OK: Frame #${parsed.frameIndex}, Kernel: ${parsed.metrics.activeKernel}, GFLOPS: ${parsed.metrics.currentGflops}`);

      console.log('[*] Testing WebSocket Handshake (RFC 6455)...');
      const client = net.createConnection({ port: 9099, host: 'localhost' }, () => {
        const secKey = crypto.randomBytes(16).toString('base64');
        const req = [
          'GET /stream HTTP/1.1',
          'Host: localhost:9099',
          'Upgrade: websocket',
          'Connection: Upgrade',
          `Sec-WebSocket-Key: ${secKey}`,
          'Sec-WebSocket-Version: 13',
          '\r\n'
        ].join('\r\n');
        client.write(req);
      });

      let receivedHandshake = false;
      client.on('data', (chunk) => {
        const str = chunk.toString();
        if (!receivedHandshake && str.includes('101 Switching Protocols')) {
          receivedHandshake = true;
          console.log('[+] WebSocket Handshake OK: 101 Switching Protocols verified!');
        } else if (receivedHandshake) {
          console.log(`[+] WebSocket Telemetry Frame Received! Length: ${chunk.length} bytes`);
          client.end();
          daemon.kill();
          console.log('[+] Bridge protocol verification passed 100% cleanly!');
          process.exit(0);
        }
      });
    });
  });
}, 800);
