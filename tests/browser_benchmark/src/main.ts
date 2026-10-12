import './styles.css';
import { nativeBridge } from './services/native-bridge';
import { RooflineChartComponent } from './components/roofline-chart';
import { LatencyHistogramComponent } from './components/latency-histogram';
import { ThroughputMeterComponent } from './components/throughput-meter';
import { MemoryTopographyComponent } from './components/memory-topography';
import { TelemetryStreamComponent } from './components/telemetry-stream';
import { BenchmarkControllerComponent } from './components/benchmark-controller';
import { LiveTelemetryFrame } from './types/telemetry';

document.addEventListener('DOMContentLoaded', () => {
  const containerMeters = document.getElementById('throughput-meters-container');
  const containerRoofline = document.getElementById('roofline-container');
  const containerHistogram = document.getElementById('histogram-container');
  const containerTopography = document.getElementById('topography-container');
  const containerController = document.getElementById('controller-container');
  const containerTelemetry = document.getElementById('telemetry-container');

  if (
    !containerMeters ||
    !containerRoofline ||
    !containerHistogram ||
    !containerTopography ||
    !containerController ||
    !containerTelemetry
  ) {
    console.error('Failed to resolve all DOM containers for MathventageSIMD benchmark');
    return;
  }

  // Initialize UI Components
  const metersComponent = new ThroughputMeterComponent(containerMeters);
  const rooflineComponent = new RooflineChartComponent(containerRoofline, nativeBridge.hardwareCeilings);
  const histogramComponent = new LatencyHistogramComponent(containerHistogram);
  const topographyComponent = new MemoryTopographyComponent(containerTopography);
  const telemetryComponent = new TelemetryStreamComponent(containerTelemetry);
  const controllerComponent = new BenchmarkControllerComponent(containerController);

  // Status updates (Connected Native vs Simulation)
  nativeBridge.onStatusChange((status) => {
    telemetryComponent.updateStatus(status);
  });

  // Handle in-browser Wasm benchmark result
  controllerComponent.onWasmResult((res) => {
    telemetryComponent.appendLog(
      `[WASM-BENCH] ${res.kernelName} -> ${res.gflops} GFLOPS, ${res.bandwidthGBs} GB/s in ${res.elapsedMs.toFixed(2)} ms (${res.simdType})`
    );
  });

  // Wire live telemetry frames
  nativeBridge.onFrame((frame: LiveTelemetryFrame) => {
    // 1. Throughput & tensor meters
    metersComponent.update(frame.metrics);

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
