import './styles.css';
import { nativeBridge } from './services/native-bridge';
import { RooflineChartComponent } from './components/roofline-chart';
import { LatencyHistogramComponent } from './components/latency-histogram';
import { ThroughputMeterComponent } from './components/throughput-meter';
import { MemoryTopographyComponent } from './components/memory-topography';
import { TelemetryStreamComponent } from './components/telemetry-stream';
import { ScriptEditorComponent } from './components/script-editor';
import { TerminalConsoleComponent } from './components/terminal-console';
import { SiliconTopologyComponent } from './components/silicon-topology';
import { SystemTelemetryChartComponent } from './components/system-telemetry-chart';
import { LiveTelemetryFrame } from './types/telemetry';

document.addEventListener('DOMContentLoaded', () => {
  const containerSystem = document.getElementById('system-telemetry-container');
  const containerMeters = document.getElementById('throughput-meters-container');
  const containerRoofline = document.getElementById('roofline-container');
  const containerHistogram = document.getElementById('histogram-container');
  const containerTopology = document.getElementById('silicon-topology-container');
  const containerTopography = document.getElementById('topography-container');
  const containerScriptEditor = document.getElementById('script-editor-container');
  const containerTerminal = document.getElementById('terminal-console-container');
  const containerTelemetry = document.getElementById('telemetry-container');

  if (
    !containerSystem ||
    !containerMeters ||
    !containerRoofline ||
    !containerHistogram ||
    !containerTopology ||
    !containerTopography ||
    !containerScriptEditor ||
    !containerTerminal ||
    !containerTelemetry
  ) {
    console.error('Failed to resolve all DOM containers for MathventageSIMD benchmark');
    return;
  }

  // Initialize UI Components
  const systemComponent = new SystemTelemetryChartComponent(containerSystem);
  const metersComponent = new ThroughputMeterComponent(containerMeters);
  const rooflineComponent = new RooflineChartComponent(containerRoofline, nativeBridge.hardwareCeilings);
  const histogramComponent = new LatencyHistogramComponent(containerHistogram);
  new SiliconTopologyComponent(containerTopology);
  const topographyComponent = new MemoryTopographyComponent(containerTopography);
  new ScriptEditorComponent(containerScriptEditor);
  const terminalComponent = new TerminalConsoleComponent(containerTerminal);
  const telemetryComponent = new TelemetryStreamComponent(containerTelemetry);

  // Status updates (Connected Native vs Simulation)
  nativeBridge.onStatusChange((status) => {
    telemetryComponent.updateStatus(status);
  });

  // Event-driven live log streaming: ONLY print when a real signal/log arrives from SFVM C-ABI
  nativeBridge.onLog((entry) => {
    terminalComponent.addLog(entry);
  });

  // Wire live telemetry frames (pure numerical metrics, no log sampling)
  nativeBridge.onFrame((frame: LiveTelemetryFrame) => {
    // 0. System Hardware Telemetry Chart (CPU, GPU, RAM, Disks, Network)
    if (frame.system) {
      systemComponent.update(frame.system);
    }
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

    // Update Header Badges
    const elTier = document.getElementById('header-tier-badge');
    const elProfile = document.getElementById('header-profile-badge');
    if (elTier) elTier.textContent = `TIER: ${nativeBridge.hardwareCeilings.tier} (${nativeBridge.hardwareCeilings.pCores}P+${nativeBridge.hardwareCeilings.eCores}E)`;
    if (elProfile) elProfile.textContent = `PROFILE: ${frame.metrics.activeProfile}`;
  });

  console.log('MathventageSIMD & SacculusForge VM Browser Benchmark initialized.');
});
