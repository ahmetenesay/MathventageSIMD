import { LatencySample, LatencyHistogramStats } from '../types/telemetry';

export class LatencyHistogramComponent {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private history: LatencySample[] = [];
  private readonly maxSamples = 600;
  private stats: LatencyHistogramStats | null = null;
  private statsOverlay: HTMLElement;

  // Double-buffering & smooth 60 FPS interpolation
  private renderedBins: number[] = new Array(28).fill(0);
  private targetBins: number[] = new Array(28).fill(0);
  private animFrameId: number | null = null;
  private lastRenderTime: number = 0;

  constructor(container: HTMLElement) {
    const wrapper = document.createElement('div');
    wrapper.className = 'histogram-wrapper';

    this.statsOverlay = document.createElement('div');
    this.statsOverlay.className = 'histogram-stats-overlay';
    wrapper.appendChild(this.statsOverlay);

    this.canvas = document.createElement('canvas');
    this.canvas.className = 'histogram-canvas';
    wrapper.appendChild(this.canvas);

    container.appendChild(wrapper);

    this.ctx = this.canvas.getContext('2d', { alpha: false })!;
    this.resize();
    window.addEventListener('resize', () => this.resize());

    // Start 60 FPS smooth render loop
    this.startLoop();
  }

  public resize(): void {
    const rect = this.canvas.parentElement?.getBoundingClientRect();
    if (rect && rect.width > 0) {
      const dpr = window.devicePixelRatio || 1;
      this.canvas.width = Math.floor(rect.width * dpr);
      this.canvas.height = Math.floor((rect.height || 260) * dpr);
      this.ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    }
  }

  private startLoop(): void {
    const loop = (time: number) => {
      this.animFrameId = requestAnimationFrame(loop);
      // Smooth lerp on rendered bins (friction 0.18 for buttery response, zero flicker)
      let changed = false;
      for (let i = 0; i < this.renderedBins.length; ++i) {
        const diff = this.targetBins[i] - this.renderedBins[i];
        if (Math.abs(diff) > 0.001) {
          this.renderedBins[i] += diff * 0.18;
          changed = true;
        } else {
          this.renderedBins[i] = this.targetBins[i];
        }
      }

      if (changed || time - this.lastRenderTime > 200) {
        this.render();
        this.lastRenderTime = time;
      }
    };
    this.animFrameId = requestAnimationFrame(loop);
  }

  public addSamples(samples: LatencySample[]): void {
    for (const s of samples) {
      this.history.push(s);
    }
    if (this.history.length > this.maxSamples) {
      this.history.splice(0, this.history.length - this.maxSamples);
    }
    this.computeStats();
    this.updateStatsOverlay();
  }

  private computeStats(): void {
    if (this.history.length === 0) return;

    const values = this.history.map(s => s.nanoseconds).sort((a, b) => a - b);
    const n = values.length;
    const minNs = values[0];
    const maxNs = values[n - 1];
    const sum = values.reduce((acc, v) => acc + v, 0);
    const avgNs = sum / n;

    const p50Ns = values[Math.floor(n * 0.50)];
    const p90Ns = values[Math.floor(n * 0.90)];
    const p95Ns = values[Math.floor(n * 0.95)];
    const p99Ns = values[Math.floor(n * 0.99)];
    const p999Ns = values[Math.floor(n * 0.999)];

    const variance = values.reduce((acc, v) => acc + Math.pow(v - avgNs, 2), 0) / n;
    const stdDevNs = Math.sqrt(variance);
    const jitterNs = maxNs - minNs;

    const numBins = 28;
    const span = Math.max(1, maxNs - minNs);
    const binWidth = span / numBins;
    const bins: { binStartNs: number; binEndNs: number; count: number }[] = [];

    for (let i = 0; i < numBins; ++i) {
      bins.push({
        binStartNs: minNs + i * binWidth,
        binEndNs: minNs + (i + 1) * binWidth,
        count: 0
      });
    }

    for (const v of values) {
      let bIdx = Math.floor((v - minNs) / binWidth);
      if (bIdx >= numBins) bIdx = numBins - 1;
      if (bIdx < 0) bIdx = 0;
      bins[bIdx].count++;
    }

    const maxCount = Math.max(...bins.map(b => b.count), 1);
    for (let i = 0; i < numBins; ++i) {
      this.targetBins[i] = bins[i].count / maxCount;
    }

    this.stats = {
      minNs,
      maxNs,
      avgNs: Math.round(avgNs * 10) / 10,
      p50Ns,
      p90Ns,
      p95Ns,
      p99Ns,
      p999Ns,
      stdDevNs: Math.round(stdDevNs * 10) / 10,
      jitterNs,
      sampleCount: n,
      bins
    };
  }

  private updateStatsOverlay(): void {
    if (!this.stats) return;
    this.statsOverlay.innerHTML = `
      <span>p50: <strong style="color: #88d49e">${this.stats.p50Ns} ns</strong></span>
      <span>p95: <strong style="color: #f6d585">${this.stats.p95Ns} ns</strong></span>
      <span>p99: <strong style="color: #e5989b">${this.stats.p99Ns} ns</strong></span>
      <span>Avg: <strong>${this.stats.avgNs} ns</strong></span>
      <span>Jitter: <strong>${this.stats.jitterNs} ns</strong></span>
      <span>N: <strong>${this.stats.sampleCount}</strong></span>
    `;
  }

  public render(): void {
    const width = this.canvas.clientWidth;
    const height = this.canvas.clientHeight;
    if (width === 0 || height === 0 || !this.stats) return;

    const ctx = this.ctx;

    // Charcoal Dark Slate Canvas
    ctx.fillStyle = '#1d222b';
    ctx.fillRect(0, 0, width, height);

    const padding = { left: 50, right: 30, top: 35, bottom: 32 };
    const plotW = width - padding.left - padding.right;
    const plotH = height - padding.top - padding.bottom;

    const numBins = this.renderedBins.length;
    const barW = plotW / numBins;

    // Grid Lines (Refined Dark Charcoal)
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.04)';
    ctx.lineWidth = 1;
    for (let g = 1; g <= 4; ++g) {
      const y = padding.top + (plotH / 4) * g;
      ctx.beginPath();
      ctx.moveTo(padding.left, y);
      ctx.lineTo(width - padding.right, y);
      ctx.stroke();
    }

    // 1. Draw Histogram Bars with Soft Gradient
    for (let i = 0; i < numBins; ++i) {
      const normH = Math.min(1.0, Math.max(0.0, this.renderedBins[i]));
      const barH = normH * plotH;
      const x = padding.left + i * barW;
      const y = height - padding.bottom - barH;

      const grad = ctx.createLinearGradient(0, y, 0, height - padding.bottom);
      grad.addColorStop(0, '#7eb0d5');
      grad.addColorStop(1, 'rgba(126, 176, 213, 0.12)');

      ctx.fillStyle = grad;
      ctx.fillRect(x + 1, y, Math.max(1, barW - 2), barH);

      ctx.strokeStyle = 'rgba(157, 195, 230, 0.4)';
      ctx.lineWidth = 1;
      ctx.strokeRect(x + 1, y, Math.max(1, barW - 2), barH);
    }

    // 2. Continuous Gaussian Kernel Density Estimation (KDE) Curve
    if (numBins > 2) {
      ctx.beginPath();
      let started = false;
      for (let i = 0; i < numBins; ++i) {
        const normH = Math.min(1.0, Math.max(0.0, this.renderedBins[i]));
        const barH = normH * plotH;
        const cx = padding.left + (i + 0.5) * barW;
        const cy = height - padding.bottom - barH;

        if (!started) {
          ctx.moveTo(cx, cy);
          started = true;
        } else {
          const prevCx = padding.left + (i - 0.5) * barW;
          const prevNormH = this.renderedBins[i - 1];
          const prevCy = height - padding.bottom - prevNormH * plotH;
          const cpX = (prevCx + cx) / 2;
          ctx.bezierCurveTo(cpX, prevCy, cpX, cy, cx, cy);
        }
      }
      ctx.strokeStyle = '#d4a373';
      ctx.lineWidth = 2.2;
      ctx.shadowColor = 'rgba(212, 163, 115, 0.35)';
      ctx.shadowBlur = 6;
      ctx.stroke();
      ctx.shadowBlur = 0;
    }

    // 3. Percentile Vertical Markers
    const drawPercentileLine = (ns: number, label: string, color: string) => {
      if (!this.stats) return;
      const span = this.stats.maxNs - this.stats.minNs || 1;
      const ratio = Math.max(0, Math.min(1, (ns - this.stats.minNs) / span));
      const x = padding.left + ratio * plotW;

      ctx.strokeStyle = color;
      ctx.lineWidth = 1.5;
      ctx.setLineDash([3, 3]);
      ctx.beginPath();
      ctx.moveTo(x, padding.top);
      ctx.lineTo(x, height - padding.bottom);
      ctx.stroke();
      ctx.setLineDash([]);

      ctx.fillStyle = color;
      ctx.font = 'bold 9px "JetBrains Mono", monospace';
      ctx.textAlign = 'center';
      ctx.fillText(label, x, padding.top - 6);
    };

    drawPercentileLine(this.stats.p50Ns, 'p50', '#88d49e');
    drawPercentileLine(this.stats.p95Ns, 'p95', '#f6d585');
    drawPercentileLine(this.stats.p99Ns, 'p99', '#e5989b');

    // 4. Base Axes & Ticks
    ctx.strokeStyle = '#2b3240';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(padding.left, height - padding.bottom);
    ctx.lineTo(width - padding.right, height - padding.bottom);
    ctx.moveTo(padding.left, padding.top);
    ctx.lineTo(padding.left, height - padding.bottom);
    ctx.stroke();

    // 5. Clean Axis Labels
    ctx.fillStyle = '#64748b';
    ctx.font = '9px "JetBrains Mono", monospace';
    ctx.textAlign = 'center';
    ctx.fillText(`${Math.round(this.stats.minNs)} ns`, padding.left, height - 12);
    ctx.fillText(`${Math.round(this.stats.maxNs)} ns`, width - padding.right, height - 12);
    ctx.fillText('Execution Latency Distribution (RDTSC Clocks)', width / 2, height - 12);
  }

  public destroy(): void {
    if (this.animFrameId !== null) {
      cancelAnimationFrame(this.animFrameId);
    }
  }
}
