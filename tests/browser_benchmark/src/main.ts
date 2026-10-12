import './styles.css';
import { nativeBridge } from './services/native-bridge';
import { RooflineChartComponent } from './components/roofline-chart';
import { LatencyHistogramComponent } from './components/latency-histogram';
import { ThroughputMeterComponent } from './components/throughput-meter';
import { MemoryTopographyComponent } from './components/memory-topography';
import { TelemetryStreamComponent } from './components/telemetry-stream';
import { BenchmarkControllerComponent } from './components/benchmark-controller';
import { ScriptEditorComponent } from './components/script-editor';
import { TerminalConsoleComponent } from './components/terminal-console';
import { SiliconTopologyComponent } from './components/silicon-topology';
import { LiveTelemetryFrame } from './types/telemetry';

document.addEventListener('DOMContentLoaded', () => {
  const containerMeters = document.getElementById('throughput-meters-container');
  const containerRoofline = document.getElementById('roofline-container');
  const containerHistogram = document.getElementById('histogram-container');
  const containerTopology = document.getElementById('silicon-topology-container');
  const containerTopography = document.getElementById('topography-container');
  const containerScriptEditor = document.getElementById('script-editor-container');
  const containerController = document.getElementById('controller-container');
  const containerTerminal = document.getElementById('terminal-console-container');
  const containerTelemetry = document.getElementById('telemetry-container');

  if (
    !containerMeters ||
    !containerRoofline ||
    !containerHistogram ||
    !containerTopology ||
    !containerTopography ||
    !containerScriptEditor ||
    !containerController ||
    !containerTerminal ||
    !containerTelemetry
  ) {
    console.error('Failed to resolve all DOM containers for MathventageSIMD benchmark');
    return;
  }

  // Initialize UI Components
  const metersComponent = new ThroughputMeterComponent(containerMeters);
  const rooflineComponent = new RooflineChartComponent(containerRoofline, nativeBridge.hardwareCeilings);
  const histogramComponent = new LatencyHistogramComponent(containerHistogram);
  new SiliconTopologyComponent(containerTopology);
  const topographyComponent = new MemoryTopographyComponent(containerTopography);
  new ScriptEditorComponent(containerScriptEditor);
  new BenchmarkControllerComponent(containerController);
  const terminalComponent = new TerminalConsoleComponent(containerTerminal);
  const telemetryComponent = new TelemetryStreamComponent(containerTelemetry);

  // Status updates (Connected Native vs Simulation)
  nativeBridge.onStatusChange((status) => {
    telemetryComponent.updateStatus(status);
    const isConnected = status === 'CONNECTED_NATIVE';
    terminalComponent.addLog({
      timestamp: new Date().toLocaleTimeString('en-US', { hour12: false }),
      category: 'SYS',
      level: isConnected ? 'SUCCESS' : 'WARN',
      message: isConnected
        ? 'Connected to native bridge daemon on ws://localhost:9099/stream (Native C-ABI Direct)'
        : `Connection status changed: ${status}. Running high-precision in-browser client simulation.`
    });
  });

  // Wire live telemetry frames
  nativeBridge.onFrame((frame: LiveTelemetryFrame) => {
    // 1. Throughput & tensor meters
    metersComponent.update(
      frame.metrics,
      frame.topography?.eggCount || 0,
      frame.topography?.fledglingCount || 0,
      frame.topography?.pitstopCount || 0
    );

    // 2. Roofline points
    if (frame.rooflinePoints && frame.rooflinePoints.length > 0) {
      rooflineComponent.updatePoints(frame.rooflinePoints);
    }

    // 3. RDTSC Latency Histogram
    if (frame.recentLatencies && frame.recentLatencies.length > 0) {
      histogramComponent.addSamples(frame.recentLatencies);
    }

    // 4. Memory Topography
    topographyComponent.update(frame.topography);

    // 5. Telemetry log stream & 4-point coordination
    telemetryComponent.update(frame);

    // 6. Push real native logs into Diagnostic Terminal
    if (frame.recentLogs && frame.recentLogs.length > 0) {
      frame.recentLogs.forEach(entry => {
        terminalComponent.addLog(entry);
      });
    }

    // Update Header Badges
    const elTier = document.getElementById('header-tier-badge');
    const elProfile = document.getElementById('header-profile-badge');
    if (elTier) elTier.textContent = `TIER: ${nativeBridge.hardwareCeilings.tier} (${nativeBridge.hardwareCeilings.pCores}P+${nativeBridge.hardwareCeilings.eCores}E)`;
    if (elProfile) elProfile.textContent = `PROFILE: ${frame.metrics.activeProfile}`;
  });

  console.log('MathventageSIMD & SacculusForge VM Browser Benchmark initialized.');
});
