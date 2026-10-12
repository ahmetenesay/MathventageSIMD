import { LatencySample, LatencyHistogramStats } from '../types/telemetry';

export class LatencyHistogramComponent {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private history: LatencySample[] = [];
  private readonly maxSamples = 500;
  private stats: LatencyHistogramStats | null = null;
  private statsOverlay: HTMLElement;

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

    this.ctx = this.canvas.getContext('2d')!;
    this.resize();
    window.addEventListener('resize', () => this.resize());
  }

  public resize(): void {
    const rect = this.canvas.parentElement?.getBoundingClientRect();
    if (rect) {
      const dpr = window.devicePixelRatio || 1;
      this.canvas.width = rect.width * dpr;
      this.canvas.height = (rect.height || 260) * dpr;
      this.ctx.scale(dpr, dpr);
      this.render();
    }
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
    this.render();
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

    // Build 24 histogram bins
    const numBins = 24;
    const binWidth = Math.max(1, (maxNs - minNs) / numBins);
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
      bins[bIdx].count++;
    }

    this.stats = {
      minNs,
      maxNs,
      avgNs,
      p50Ns,
      p90Ns,
      p95Ns,
      p99Ns,
      p999Ns,
      stdDevNs,
      jitterNs,
      sampleCount: n,
      bins
    };
  }

  private updateStatsOverlay(): void {
    if (!this.stats) return;
    this.statsOverlay.innerHTML = `
      <div class="stat-badge"><span class="lbl">p50</span><span class="val cyan">${this.stats.p50Ns.toLocaleString()} ns</span></div>
      <div class="stat-badge"><span class="lbl">p95</span><span class="val yellow">${this.stats.p95Ns.toLocaleString()} ns</span></div>
      <div class="stat-badge"><span class="lbl">p99</span><span class="val red">${this.stats.p99Ns.toLocaleString()} ns</span></div>
      <div class="stat-badge"><span class="lbl">Avg</span><span class="val green">${Math.round(this.stats.avgNs).toLocaleString()} ns</span></div>
      <div class="stat-badge"><span class="lbl">Jitter</span><span class="val dim">±${Math.round(this.stats.jitterNs / 2).toLocaleString()} ns</span></div>
    `;
  }

  public render(): void {
    const width = this.canvas.clientWidth;
    const height = this.canvas.clientHeight;
    if (width === 0 || height === 0 || !this.stats || this.stats.bins.length === 0) return;

    const ctx = this.ctx;
    ctx.clearRect(0, 0, width, height);

    ctx.fillStyle = '#0a0e17';
    ctx.fillRect(0, 0, width, height);

    const padding = { left: 45, right: 25, top: 40, bottom: 35 };
    const plotW = width - padding.left - padding.right;
    const plotH = height - padding.top - padding.bottom;

    const maxCount = Math.max(...this.stats.bins.map(b => b.count), 1);
    const barW = plotW / this.stats.bins.length;

    // Draw Histogram Bars
    for (let i = 0; i < this.stats.bins.length; ++i) {
      const b = this.stats.bins[i];
      const barH = (b.count / maxCount) * plotH;
      const x = padding.left + i * barW;
      const y = height - padding.bottom - barH;

      // Gradient color based on latency
      const grad = ctx.createLinearGradient(0, y, 0, height - padding.bottom);
      grad.addColorStop(0, '#00f3ff');
      grad.addColorStop(1, 'rgba(0, 243, 255, 0.15)');

      ctx.fillStyle = grad;
      ctx.fillRect(x + 1, y, barW - 2, barH);

      ctx.strokeStyle = '#00f3ff';
      ctx.lineWidth = 1;
      ctx.strokeRect(x + 1, y, barW - 2, barH);
    }

    // Draw Percentile Lines
    const drawPercentileLine = (ns: number, label: string, color: string) => {
      if (!this.stats) return;
      const ratio = (ns - this.stats.minNs) / (this.stats.maxNs - this.stats.minNs || 1);
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

    drawPercentileLine(this.stats.p50Ns, 'p50', '#00f3ff');
    drawPercentileLine(this.stats.p95Ns, 'p95', '#ffb800');
    drawPercentileLine(this.stats.p99Ns, 'p99', '#ff0055');

    // Axes
    ctx.strokeStyle = '#334155';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(padding.left, height - padding.bottom);
    ctx.lineTo(width - padding.right, height - padding.bottom);
    ctx.moveTo(padding.left, padding.top);
    ctx.lineTo(padding.left, height - padding.bottom);
    ctx.stroke();

    // X-axis min / max labels
    ctx.fillStyle = '#64748b';
    ctx.font = '10px "JetBrains Mono", monospace';
    ctx.textAlign = 'left';
    ctx.fillText(`${this.stats.minNs.toLocaleString()} ns`, padding.left, height - padding.bottom + 18);
    ctx.textAlign = 'right';
    ctx.fillText(`${this.stats.maxNs.toLocaleString()} ns`, width - padding.right, height - padding.bottom + 18);
    ctx.textAlign = 'center';
    ctx.fillText('RDTSC Hardware Execution Latency', width / 2, height - 10);
  }
}
