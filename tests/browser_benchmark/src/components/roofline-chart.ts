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
      this.canvas.height = (rect.height || 290) * dpr;
      this.ctx.scale(dpr, dpr);
      this.render();
    }
  }

  public updatePoints(points: RooflineKernelPoint[]): void {
    this.points = points;
    this.render();
  }

  private xToCanvas(ai: number, width: number, padding: number): number {
    const minLog = Math.log10(0.05);
    const maxLog = Math.log10(100);
    const logVal = Math.log10(Math.max(0.05, Math.min(100, ai)));
    const ratio = (logVal - minLog) / (maxLog - minLog);
    return padding + ratio * (width - 2 * padding);
  }

  private yToCanvas(gflops: number, height: number, padding: number): number {
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

    const padding = 50;

    // Charcoal Dark Slate Background
    ctx.fillStyle = '#1d222b';
    ctx.fillRect(0, 0, width, height);

    // Grid lines (subtle slate)
    ctx.strokeStyle = '#2b3240';
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
      ctx.fillText(`${tick}`, x, height - padding + 15);
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
    ctx.fillStyle = '#8e9bb0';
    ctx.font = '11px "JetBrains Mono", monospace';
    ctx.textAlign = 'center';
    ctx.fillText('Arithmetic Intensity [FLOPs / Byte]', width / 2, height - 12);

    ctx.save();
    ctx.translate(14, height / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.fillText('Attainable Performance [GFLOPS]', 0, 0);
    ctx.restore();

    // 1. Plot Memory Bandwidth Slopes with Pastel Colors
    const drawBandwidthCeiling = (bwGBs: number, color: string, label: string, peakGflops: number) => {
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.6;
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
      ctx.fillText(`${label} (${bwGBs} GB/s)`, x0 + 8, y0 - 6);
    };

    // DRAM Bandwidth (Pastel Coral #e5989b)
    drawBandwidthCeiling(this.ceilings.dramBandwidthGBs, '#e5989b', 'DRAM BW', this.ceilings.avx2PeakGflops);
    // L3 Bandwidth (Pastel Lavender #b8a9c9)
    drawBandwidthCeiling(this.ceilings.l3BandwidthGBs, '#b8a9c9', 'L3 Cache BW', this.ceilings.avx512PeakGflops);
    // L1 Bandwidth (Pastel Mint #88d49e)
    drawBandwidthCeiling(this.ceilings.l1dBandwidthGBs, '#88d49e', 'L1d Cache BW', this.ceilings.avx512PeakGflops);

    // 2. Draw Compute Ceilings (Horizontal flat lines in Pastel Sky & Gold)
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
      ctx.font = '9px "JetBrains Mono", monospace';
      ctx.textAlign = 'right';
      ctx.fillText(`${label} [${gflops} GF]`, width - padding - 6, y - 5);
    };

    drawComputeCeiling(this.ceilings.scalarPeakGflops, '#64748b', 'Scalar Peak');
    drawComputeCeiling(this.ceilings.avx2PeakGflops, '#d4a373', 'AVX2 Peak');
    drawComputeCeiling(this.ceilings.avx512PeakGflops, '#88d49e', 'AVX-512 Peak');

    // 3. Render Active Kernel Operating Points
    for (const pt of this.points) {
      const cx = this.xToCanvas(pt.arithmeticIntensity, width, padding);
      const cy = this.yToCanvas(pt.performanceGflops, height, padding);

      // Pastel outer halo
      ctx.fillStyle = 'rgba(212, 163, 115, 0.2)';
      ctx.beginPath();
      ctx.arc(cx, cy, 9, 0, 2 * Math.PI);
      ctx.fill();

      // Inner point
      ctx.fillStyle = pt.boundType === 'compute_bound' ? '#88d49e' : '#e5989b';
      ctx.strokeStyle = '#ffffff';
      ctx.lineWidth = 1.5;
      ctx.beginPath();
      ctx.arc(cx, cy, 5, 0, 2 * Math.PI);
      ctx.fill();
      ctx.stroke();

      // Label
      ctx.fillStyle = '#f1f5f9';
      ctx.font = 'bold 10px "JetBrains Mono", monospace';
      ctx.textAlign = 'left';
      ctx.fillText(`${pt.name} (${pt.performanceGflops} GF)`, cx + 12, cy + 3);
    }
  }
}
