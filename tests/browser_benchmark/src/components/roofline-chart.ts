import { HardwareCeilings, RooflineKernelPoint } from '../types/telemetry';

export class RooflineChartComponent {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private points: RooflineKernelPoint[] = [];
  private ceilings: HardwareCeilings;

  constructor(container: HTMLElement, ceilings: HardwareCeilings) {
    this.ceilings = ceilings;
    this.canvas = document.createElement('canvas');
    this.canvas.className = 'roofline-canvas';
    container.appendChild(this.canvas);
    this.ctx = this.canvas.getContext('2d')!;
    this.resize();
    window.addEventListener('resize', () => this.resize());
  }

  public resize(): void {
    const rect = this.canvas.parentElement?.getBoundingClientRect();
    if (rect) {
      const dpr = window.devicePixelRatio || 1;
      this.canvas.width = rect.width * dpr;
      this.canvas.height = (rect.height || 320) * dpr;
      this.ctx.scale(dpr, dpr);
      this.render();
    }
  }

  public updatePoints(points: RooflineKernelPoint[]): void {
    this.points = points;
    this.render();
  }

  private xToCanvas(ai: number, width: number, padding: number): number {
    // Log scale for Arithmetic Intensity: 0.05 to 100 FLOP/Byte
    const minLog = Math.log10(0.05);
    const maxLog = Math.log10(100);
    const logVal = Math.log10(Math.max(0.05, Math.min(100, ai)));
    const ratio = (logVal - minLog) / (maxLog - minLog);
    return padding + ratio * (width - 2 * padding);
  }

  private yToCanvas(gflops: number, height: number, padding: number): number {
    // Log scale for GFLOPS: 1 to 5000 GFLOPS
    const minLog = Math.log10(1);
    const maxLog = Math.log10(5000);
    const logVal = Math.log10(Math.max(1, Math.min(5000, gflops)));
    const ratio = (logVal - minLog) / (maxLog - minLog);
    return height - padding - ratio * (height - 2 * padding);
  }

  public render(): void {
    const width = this.canvas.clientWidth;
    const height = this.canvas.clientHeight;
    if (width === 0 || height === 0) return;

    const ctx = this.ctx;
    ctx.clearRect(0, 0, width, height);

    const padding = 55;

    // Background Grid & axes
    ctx.fillStyle = '#0a0e17';
    ctx.fillRect(0, 0, width, height);

    // Subtle grid lines
    ctx.strokeStyle = '#1e293b';
    ctx.lineWidth = 1;
    ctx.setLineDash([2, 4]);

    const aiTicks = [0.1, 0.5, 1, 2, 5, 10, 20, 50, 100];
    ctx.font = '10px "JetBrains Mono", monospace';
    ctx.fillStyle = '#64748b';
    ctx.textAlign = 'center';

    for (const tick of aiTicks) {
      const x = this.xToCanvas(tick, width, padding);
      ctx.beginPath();
      ctx.moveTo(x, padding);
      ctx.lineTo(x, height - padding);
      ctx.stroke();
      ctx.fillText(`${tick}`, x, height - padding + 16);
    }

    const gflopsTicks = [10, 50, 100, 500, 1000, 2000, 4000];
    ctx.textAlign = 'right';
    ctx.textBaseline = 'middle';
    for (const tick of gflopsTicks) {
      const y = this.yToCanvas(tick, height, padding);
      ctx.beginPath();
      ctx.moveTo(padding, y);
      ctx.lineTo(width - padding, y);
      ctx.stroke();
      ctx.fillText(`${tick}`, padding - 8, y);
    }
    ctx.setLineDash([]);

    // Axis Labels
    ctx.fillStyle = '#94a3b8';
    ctx.font = '11px "JetBrains Mono", monospace';
    ctx.textAlign = 'center';
    ctx.fillText('Arithmetic Intensity [FLOPs / Byte]', width / 2, height - 12);

    ctx.save();
    ctx.translate(15, height / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.fillText('Attainable Performance [GFLOPS]', 0, 0);
    ctx.restore();

    // 1. Plot Memory Bandwidth Slopes (Attainable GFLOPS = Bandwidth_GBs * Arithmetic_Intensity)
    const drawBandwidthCeiling = (bwGBs: number, color: string, label: string, peakGflops: number) => {
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.5;
      ctx.beginPath();

      const aiStart = 0.05;
      const aiKnee = peakGflops / bwGBs;
      const x0 = this.xToCanvas(aiStart, width, padding);
      const y0 = this.yToCanvas(aiStart * bwGBs, height, padding);

      const xKnee = this.xToCanvas(aiKnee, width, padding);
      const yKnee = this.yToCanvas(peakGflops, height, padding);
      const xEnd = this.xToCanvas(100, width, padding);

      ctx.moveTo(x0, y0);
      ctx.lineTo(xKnee, yKnee);
      ctx.lineTo(xEnd, yKnee);
      ctx.stroke();

      ctx.fillStyle = color;
      ctx.font = '9px "JetBrains Mono", monospace';
      ctx.textAlign = 'left';
      ctx.fillText(`${label} (${bwGBs} GB/s)`, x0 + 10, y0 - 6);
    };

    // DRAM Bandwidth
    drawBandwidthCeiling(this.ceilings.dramBandwidthGBs, '#38bdf8', 'DRAM BW', this.ceilings.avx2PeakGflops);
    // L3 Bandwidth
    drawBandwidthCeiling(this.ceilings.l3BandwidthGBs, '#a855f7', 'L3 Cache BW', this.ceilings.avx512PeakGflops);
    // L1 Bandwidth
    drawBandwidthCeiling(this.ceilings.l1dBandwidthGBs, '#ec4899', 'L1d Cache BW', this.ceilings.avx512PeakGflops);

    // 2. Draw Compute Ceilings (Horizontal flat lines)
    const drawComputeCeiling = (gflops: number, color: string, label: string) => {
      const y = this.yToCanvas(gflops, height, padding);
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.2;
      ctx.setLineDash([4, 4]);
      ctx.beginPath();
      ctx.moveTo(padding, y);
      ctx.lineTo(width - padding, y);
      ctx.stroke();
      ctx.setLineDash([]);

      ctx.fillStyle = color;
      ctx.font = '10px "JetBrains Mono", monospace';
      ctx.textAlign = 'right';
      ctx.fillText(`── ${label} [${gflops} GFLOPS]`, width - padding - 8, y - 6);
    };

    drawComputeCeiling(this.ceilings.scalarPeakGflops, '#94a3b8', 'Scalar FP32');
    drawComputeCeiling(this.ceilings.avx2PeakGflops, '#00ff9d', 'AVX2 256-bit FMA');
    drawComputeCeiling(this.ceilings.avx512PeakGflops, '#00f3ff', 'AVX-512 512-bit FMA');
    if (this.ceilings.npuPeakGflops > 0) {
      drawComputeCeiling(this.ceilings.npuPeakGflops, '#ffb800', 'OpenVINO / NPU Peak');
    }

    // 3. Plot Active Benchmark Points
    for (const pt of this.points) {
      const x = this.xToCanvas(pt.arithmeticIntensity, width, padding);
      const y = this.yToCanvas(pt.performanceGflops, height, padding);

      const color = pt.boundType === 'compute_bound' ? '#00ff9d' : '#00f3ff';

      // Halo glow
      ctx.shadowBlur = 10;
      ctx.shadowColor = color;

      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.arc(x, y, 6, 0, Math.PI * 2);
      ctx.fill();

      ctx.shadowBlur = 0;
      ctx.strokeStyle = '#ffffff';
      ctx.lineWidth = 1.5;
      ctx.stroke();

      // Tooltip / label next to point
      ctx.font = 'bold 11px "JetBrains Mono", monospace';
      ctx.fillStyle = '#ffffff';
      ctx.textAlign = 'left';
      ctx.fillText(`${pt.name} [${pt.performanceGflops} GF]`, x + 10, y + 4);

      ctx.font = '9px "JetBrains Mono", monospace';
      ctx.fillStyle = pt.boundType === 'compute_bound' ? '#00ff9d' : '#38bdf8';
      ctx.fillText(`• ${pt.boundType.toUpperCase()} (AI: ${pt.arithmeticIntensity})`, x + 10, y + 16);
    }
  }
}
