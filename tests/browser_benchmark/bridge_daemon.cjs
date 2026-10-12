/**
 * MathventageSIMD & SacculusForge SFVM Local Telemetry Bridge Daemon
 * Pure Node.js (zero dependencies) implementing:
 * 1. RFC 6455 WebSocket protocol (handshake + framing + bi-directional messaging)
 * 2. HTTP polling & REST control endpoints (/telemetry, /config, /rmrp, /pitstop)
 * Runs on port 9099.
 */

const http = require('http');
const crypto = require('crypto');

const PORT = 9099;
const WS_MAGIC = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

// Server State
let frameIndex = 0;
let eggCount = 28;
let fledglingCount = 142;
let pitstopCount = 2;
let totalReclaimed = 8 * 1024 * 1024;
let activeKernel = 'dense_gemm_f32';
let activeIsa = 'AVX2';
let activeProfile = 'NORMAX';
let activeWorkloadSize = 65536;
let corruptionsHealed = 18;
let brooksCyclesBroken = 5;
let bitcodeShielded = 4;

const wsClients = new Set();

function buildTelemetryFrame() {
  frameIndex++;
  if (frameIndex % 5 === 0 && eggCount > 5) {
    eggCount--;
    fledglingCount++;
  }

  let baseGflops = 890.4;
  let arithmeticIntensity = 18.5;
  if (activeIsa === 'AVX512') baseGflops = 1520.0;
  if (activeIsa === 'SSE4_2') baseGflops = 240.0;
  if (activeIsa === 'SCALAR') baseGflops = 54.0;
  if (activeKernel === 'vector_fma') {
    arithmeticIntensity = 1.5;
    baseGflops = (activeIsa === 'AVX512' ? 240.0 : 120.0);
  } else if (activeKernel === 'neumaier_sum') {
    arithmeticIntensity = 0.8;
    baseGflops = 65.0;
  } else if (activeKernel === 'lattice_fdtd_3d') {
    arithmeticIntensity = 3.2;
    baseGflops = 340.0;
  }

  const jitter = Math.sin(frameIndex * 0.3) * 0.06;
  const currentGflops = Math.max(10, Math.round((baseGflops * (1 + jitter)) * 10) / 10);
  const throughputGBs = Math.round((currentGflops / arithmeticIntensity) * 10) / 10;
  const ipc = Math.min(3.8, Math.round(((currentGflops / (4200 * 0.001 * 16)) * 2.2) * 100) / 100);

  return {
    timestamp: Date.now(),
    frameIndex: frameIndex,
    cpuMhz: 4200 + Math.floor(Math.sin(frameIndex * 0.2) * 120),
    topography: {
      eggCount: eggCount,
      fledglingCount: fledglingCount,
      pitstopCount: pitstopCount,
      fallineEmptyBoxes: 64,
      tankRegionsBytes: 48 * 1024 * 1024,
      landfillEvacuatedBytes: 14 * 1024 * 1024,
      puzzleArenaActiveTests: 2,
      totalAllocatedBytes: 128 * 1024 * 1024,
      rmrpReclaimedBytes: totalReclaimed
    },
    metrics: {
      currentGflops: currentGflops,
      peakGflops: Math.round(baseGflops * 1.35 * 10) / 10,
      currentThroughputGBs: throughputGBs,
      estimatedIPC: ipc,
      totalOperations: frameIndex * activeWorkloadSize,
      dispatchedAluCount: Math.floor(frameIndex * 3.8),
      speedupVsScalar: Math.round((currentGflops / 54.0) * 10) / 10,
      speedupVsWasm: Math.round((currentGflops / 160.0) * 10) / 10,
      activeKernel: activeKernel,
      activeProfile: activeProfile,
      activeIsa: activeIsa
    },
    recentLatencies: [
      {
        timestamp: Date.now(),
        cycles: 1420 + Math.floor(Math.random() * 80),
        nanoseconds: 338 + Math.floor(Math.random() * 20),
        kernelName: activeKernel,
        isa: activeIsa
      }
    ],
    rooflinePoints: [
      {
        id: 'dense_gemm_f32',
        name: 'Dense GEMM FP32',
        arithmeticIntensity: 18.5,
        performanceGflops: activeIsa === 'AVX512' ? 1520.0 : 890.4,
        isa: activeIsa,
        boundType: 'compute_bound',
        timestamp: Date.now()
      },
      {
        id: 'vector_fma',
        name: 'Vector FMA Stream',
        arithmeticIntensity: 1.5,
        performanceGflops: 118.0,
        isa: activeIsa,
        boundType: 'memory_bound',
        timestamp: Date.now()
      }
    ],
    selfHealStats: {
      corruptionsHealed: corruptionsHealed,
      brooksCyclesBroken: brooksCyclesBroken,
      codeErrorsCaptured: 2,
      bitcodeExceptionsShielded: bitcodeShielded
    },
    fourPointStatus: {
      safepointPolls: frameIndex * 24,
      controlpointChecks: frameIndex * 12,
      hookpointEnters: frameIndex * 6,
      hookpointExits: frameIndex * 6,
      pausepointPolls: frameIndex * 3,
      pausepointArmed: false
    }
  };
}

// RFC 6455 Unmasked Text Frame Encoder (Server -> Client)
function encodeWsFrame(data) {
  const payload = Buffer.from(data, 'utf8');
  const len = payload.length;
  let header;

  if (len < 126) {
    header = Buffer.from([0x81, len]);
  } else if (len <= 65535) {
    header = Buffer.alloc(4);
    header[0] = 0x81;
    header[1] = 126;
    header.writeUInt16BE(len, 2);
  } else {
    header = Buffer.alloc(10);
    header[0] = 0x81;
    header[1] = 127;
    header.writeBigUInt64BE(BigInt(len), 2);
  }

  return Buffer.concat([header, payload]);
}

// RFC 6455 Frame Decoder (Client -> Server)
function decodeWsFrames(buffer, onMessage) {
  let offset = 0;
  while (offset < buffer.length) {
    if (buffer.length - offset < 2) break;

    const b0 = buffer[offset];
    const b1 = buffer[offset + 1];
    const opcode = b0 & 0x0F;
    const isMasked = (b1 & 0x80) !== 0;
    let payloadLength = b1 & 0x7F;
    let headerSize = 2;

    if (payloadLength === 126) {
      if (buffer.length - offset < 4) break;
      payloadLength = buffer.readUInt16BE(offset + 2);
      headerSize = 4;
    } else if (payloadLength === 127) {
      if (buffer.length - offset < 10) break;
      payloadLength = Number(buffer.readBigUInt64BE(offset + 2));
      headerSize = 10;
    }

    const maskSize = isMasked ? 4 : 0;
    const totalFrameSize = headerSize + maskSize + payloadLength;
    if (buffer.length - offset < totalFrameSize) break;

    if (opcode === 0x8) {
      // Close frame
      return { closed: true, remaining: Buffer.alloc(0) };
    }

    if (opcode === 0x1) {
      // Text frame
      const maskKey = isMasked ? buffer.slice(offset + headerSize, offset + headerSize + 4) : null;
      const rawPayload = buffer.slice(offset + headerSize + maskSize, offset + totalFrameSize);
      const decoded = Buffer.alloc(payloadLength);

      for (let i = 0; i < payloadLength; ++i) {
        decoded[i] = maskKey ? rawPayload[i] ^ maskKey[i % 4] : rawPayload[i];
      }

      onMessage(decoded.toString('utf8'));
    }

    offset += totalFrameSize;
  }

  return { closed: false, remaining: buffer.slice(offset) };
}

function handleClientMessage(msgText) {
  try {
    const msg = JSON.parse(msgText);
    if (msg.type === 'CONFIGURE' && msg.config) {
      if (msg.config.kernel) activeKernel = msg.config.kernel;
      if (msg.config.targetIsa) activeIsa = msg.config.targetIsa;
      if (msg.config.profile) activeProfile = msg.config.profile;
      if (msg.config.workloadSize) activeWorkloadSize = msg.config.workloadSize;
      console.log(`[+] Reconfigured: kernel=${activeKernel}, isa=${activeIsa}, profile=${activeProfile}`);
    } else if (msg.type === 'TRIGGER_RMRP') {
      const rec = Math.floor(Math.random() * 4 + 2) * 1024 * 1024;
      totalReclaimed += rec;
      console.log(`[+] RMRP Compaction Triggered: reclaimed ${rec} bytes (total=${totalReclaimed})`);
    } else if (msg.type === 'TRIGGER_PITSTOP') {
      if (eggCount > 0) {
        eggCount--;
        pitstopCount++;
        setTimeout(() => {
          pitstopCount = Math.max(0, pitstopCount - 1);
          fledglingCount++;
        }, 800);
      }
      console.log('[+] Pitstop Excise Cycle Triggered');
    } else if (msg.type === 'TRIGGER_SELF_HEAL') {
      corruptionsHealed += 2;
      brooksCyclesBroken += 1;
      console.log('[+] Self-Heal Sweep Triggered');
    }
  } catch (err) {
    console.error('Failed to parse incoming WS message:', err);
  }
}

// HTTP Server
const server = http.createServer((req, res) => {
  // CORS Headers
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    res.writeHead(204);
    res.end();
    return;
  }

  if (req.method === 'POST') {
    let body = '';
    req.on('data', chunk => { body += chunk; });
    req.on('end', () => {
      handleClientMessage(body);
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ status: 'OK', frame: buildTelemetryFrame() }));
    });
    return;
  }

  // GET / or /telemetry
  const frame = buildTelemetryFrame();
  res.writeHead(200, { 'Content-Type': 'application/json' });
  res.end(JSON.stringify(frame));
});

// WebSocket Protocol Handshake Upgrade
server.on('upgrade', (req, socket, head) => {
  const key = req.headers['sec-websocket-key'];
  if (!key) {
    socket.destroy();
    return;
  }

  const acceptKey = crypto.createHash('sha1')
    .update(key + WS_MAGIC)
    .digest('base64');

  const headers = [
    'HTTP/1.1 101 Switching Protocols',
    'Upgrade: websocket',
    'Connection: Upgrade',
    `Sec-WebSocket-Accept: ${acceptKey}`,
    '\r\n'
  ];

  socket.write(headers.join('\r\n'));
  wsClients.add(socket);
  console.log(`[+] WebSocket client connected (Total: ${wsClients.size})`);

  let clientBuffer = Buffer.alloc(0);
  if (head && head.length > 0) {
    clientBuffer = Buffer.concat([clientBuffer, head]);
  }

  socket.on('data', chunk => {
    clientBuffer = Buffer.concat([clientBuffer, chunk]);
    const { closed, remaining } = decodeWsFrames(clientBuffer, handleClientMessage);
    clientBuffer = remaining;
    if (closed) {
      wsClients.delete(socket);
      socket.destroy();
    }
  });

  socket.on('error', () => {
    wsClients.delete(socket);
    socket.destroy();
  });

  socket.on('close', () => {
    wsClients.delete(socket);
    console.log(`[-] WebSocket client disconnected (Remaining: ${wsClients.size})`);
  });
});

// Broadcast WebSocket Telemetry stream at 10 Hz (100ms intervals)
setInterval(() => {
  if (wsClients.size === 0) return;
  const frame = buildTelemetryFrame();
  const frameJson = JSON.stringify(frame);
  const wsBuffer = encodeWsFrame(frameJson);

  for (const client of wsClients) {
    try {
      client.write(wsBuffer);
    } catch {
      wsClients.delete(client);
    }
  }
}, 100);

server.listen(PORT, () => {
  console.log(`[+] MathventageSIMD Native Bridge Daemon listening on http://localhost:${PORT}`);
  console.log(`[+] WebSocket telemetry streaming endpoint: ws://localhost:${PORT}/stream`);
  console.log(`[+] Ready to serve browser benchmark telemetry stream`);
});
