import { SystemTelemetry } from '../types/telemetry';

export class SystemTelemetryChartComponent {
  private container: HTMLElement;
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private history: {
    time: number;
    cpu: number;
    gpu: number;
    ram: number;
    disk: number;
    net: number;
  }[] = [];
  private maxPoints = 60;

  // Toggle visibility of each series
  private activeSeries = {
    cpu: true,
    gpu: true,
    ram: true,
    disk: true,
    net: true
  };

  // DOM Elements for live value chips
  private elCpuBadge!: HTMLElement;
  private elGpuBadge!: HTMLElement;
  private elRamBadge!: HTMLElement;
  private elDiskBadge!: HTMLElement;
  private elNetBadge!: HTMLElement;
  private elMemBarSystem!: HTMLElement;
  private elMemBarSfvm!: HTMLElement;
  private elMemBarFree!: HTMLElement;
  private elMemText!: HTMLElement;
  private elCoresContainer!: HTMLElement;

  constructor(container: HTMLElement) {
    this.container = container;
    this.container.innerHTML = this.renderTemplate();
    this.canvas = this.container.querySelector('#system-telemetry-canvas') as HTMLCanvasElement;
    this.ctx = this.canvas.getContext('2d')!;

    this.bindDomElements();
    this.bindEvents();
    this.resizeCanvas();
    window.addEventListener('resize', () => this.resizeCanvas());

    this.startRenderLoop();
  }

  private renderTemplate(): string {
    return `
      <div class="system-telemetry-card">
        <div class="system-card-header">
          <div class="system-card-title-group">
            <div class="system-icon">⚡</div>
            <div>
              <div class="system-card-title">SİSTEM DONANIM AKIŞI & KAYNAK TELEMETRİSİ</div>
              <div class="system-card-sub">CPU(lar) • GPU(lar) • Bellek (Host vs SFVM) • Diskler • Ağ Gerçek Zamanlı Eğri Grafiği</div>
            </div>
          </div>
          <div class="system-series-toggles">
            <button class="series-toggle-pill active cpu-pill" data-series="cpu">
              <span class="series-dot dot-cpu"></span> CPU: <span id="chip-val-cpu">0%</span>
            </button>
            <button class="series-toggle-pill active gpu-pill" data-series="gpu">
              <span class="series-dot dot-gpu"></span> GPU: <span id="chip-val-gpu">0%</span>
            </button>
            <button class="series-toggle-pill active ram-pill" data-series="ram">
              <span class="series-dot dot-ram"></span> RAM: <span id="chip-val-ram">0%</span>
            </button>
            <button class="series-toggle-pill active disk-pill" data-series="disk">
              <span class="series-dot dot-disk"></span> DİSK: <span id="chip-val-disk">0%</span>
            </button>
            <button class="series-toggle-pill active net-pill" data-series="net">
              <span class="series-dot dot-net"></span> AĞ: <span id="chip-val-net">0 KB/s</span>
            </button>
          </div>
        </div>

        <!-- Physical RAM Isolation & Audit Bar -->
        <div class="system-memory-audit-panel">
          <div class="mem-audit-header">
            <span class="audit-label">FİZİKSEL BELLEK AYRIŞTIRMA & DAĞILIM ANALİZİ (32.1 GB Toplam)</span>
            <span class="audit-stats" id="mem-audit-text">Sistem & Arkaplan: 0 MB | SFVM On-Heap: 0 MB | Boş: 0 MB</span>
          </div>
          <div class="mem-bar-track">
            <div class="mem-bar-segment seg-system" id="mem-seg-system" style="width: 50%" title="Windows ve Arkaplan Uygulamaları"></div>
            <div class="mem-bar-segment seg-sfvm" id="mem-seg-sfvm" style="width: 1%" title="SFVM On-Heap Tahsisat"></div>
            <div class="mem-bar-segment seg-free" id="mem-seg-free" style="width: 49%" title="Kullanılabilir Boş Bellek"></div>
          </div>
          <div class="mem-bar-legend">
            <span><span class="legend-color-box box-system"></span> İşletim Sistemi & Diğer Arkaplan Süreçleri</span>
            <span><span class="legend-color-box box-sfvm"></span> SFVM On-Heap Tensör Tahsisatı (sfGC Kontrolünde)</span>
            <span><span class="legend-color-box box-free"></span> Boş / Kullanılabilir Fiziksel RAM</span>
          </div>
        </div>

        <!-- Main Multi-Resource Line Chart Canvas -->
        <div class="system-canvas-wrapper">
          <canvas id="system-telemetry-canvas"></canvas>
        </div>

        <!-- Hardware Footprint Matrix -->
        <div class="system-sub-footprint">
          <div class="footprint-block cpu-cores-block">
            <div class="footprint-block-title">CPU ÇEKİRDEK DAĞILIMI (14 Threads: 6P + 8E)</div>
            <div class="cpu-cores-grid" id="cpu-cores-container">
              <!-- Dynamically populated core bars -->
            </div>
          </div>
          <div class="footprint-block hardware-spec-block">
            <div class="footprint-block-title">DONANIM AYRINTILARI & AKTİF BİLEŞENLER</div>
            <div class="spec-items-grid" id="hw-spec-details">
              <div class="spec-item">
                <span class="spec-k">GPU 0</span>
                <span class="spec-v" id="spec-gpu0">NVIDIA RTX 3060 (12GB)</span>
              </div>
              <div class="spec-item">
                <span class="spec-k">DİSK C:</span>
                <span class="spec-v" id="spec-disk-c">908 / 931 GB (%97.5)</span>
              </div>
              <div class="spec-item">
                <span class="spec-k">DİSK E:</span>
                <span class="spec-v" id="spec-disk-e">675 / 699 GB (%96.6)</span>
              </div>
              <div class="spec-item">
                <span class="spec-k">AĞ ADAPTÖRÜ</span>
                <span class="spec-v" id="spec-net-adapter">Ethernet Gbps</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    `;
  }

  private bindDomElements(): void {
    this.elCpuBadge = this.container.querySelector('#chip-val-cpu')!;
    this.elGpuBadge = this.container.querySelector('#chip-val-gpu')!;
    this.elRamBadge = this.container.querySelector('#chip-val-ram')!;
    this.elDiskBadge = this.container.querySelector('#chip-val-disk')!;
    this.elNetBadge = this.container.querySelector('#chip-val-net')!;
    this.elMemBarSystem = this.container.querySelector('#mem-seg-system')!;
    this.elMemBarSfvm = this.container.querySelector('#mem-seg-sfvm')!;
    this.elMemBarFree = this.container.querySelector('#mem-seg-free')!;
    this.elMemText = this.container.querySelector('#mem-audit-text')!;
    this.elCoresContainer = this.container.querySelector('#cpu-cores-container')!;
  }

  private bindEvents(): void {
    const toggles = this.container.querySelectorAll<HTMLButtonElement>('.series-toggle-pill');
    toggles.forEach(btn => {
      btn.addEventListener('click', () => {
        const series = btn.dataset.series as keyof typeof this.activeSeries;
        if (series && this.activeSeries[series] !== undefined) {
          this.activeSeries[series] = !this.activeSeries[series];
          btn.classList.toggle('active', this.activeSeries[series]);
        }
      });
    });
  }

  private resizeCanvas(): void {
    const rect = this.canvas.parentElement?.getBoundingClientRect();
    if (!rect) return;
    const dpr = window.devicePixelRatio || 1;
    this.canvas.width = rect.width * dpr;
    this.canvas.height = 240 * dpr;
    this.canvas.style.width = `${rect.width}px`;
    this.canvas.style.height = `240px`;
    this.ctx.scale(dpr, dpr);
  }

  public update(telemetry?: SystemTelemetry): void {
    if (!telemetry) return;

    const cpuPct = telemetry.cpu?.totalLoad || 0;
    const gpuPct = telemetry.gpu?.[0]?.load || 0;
    const ramPct = telemetry.memory?.percent || 0;
    const diskPct = telemetry.disks?.[0]?.usePercent || 0;
    const netRx = telemetry.network?.rxSecKB || 0;
    const netTx = telemetry.network?.txSecKB || 0;
    const netNormalized = Math.min(100, Math.round(((netRx + netTx) / 2048) * 100)); // Scaled to ~2 MB/s max

    // Update historical queue
    this.history.push({
      time: telemetry.timestamp,
      cpu: cpuPct,
      gpu: gpuPct,
      ram: ramPct,
      disk: diskPct,
      net: netNormalized
    });
    if (this.history.length > this.maxPoints) {
      this.history.shift();
    }

    // Update Badge text
    if (this.elCpuBadge) this.elCpuBadge.textContent = `${cpuPct}%`;
    if (this.elGpuBadge) this.elGpuBadge.textContent = `${gpuPct}%`;
    if (this.elRamBadge) {
      const usedGB = (telemetry.memory.usedMB / 1024).toFixed(1);
      const totalGB = (telemetry.memory.totalMB / 1024).toFixed(1);
      this.elRamBadge.textContent = `${ramPct}% (${usedGB}/${totalGB} GB)`;
    }
    if (this.elDiskBadge) this.elDiskBadge.textContent = `${diskPct}%`;
    if (this.elNetBadge) this.elNetBadge.textContent = `↓${netRx} ↑${netTx} KB/s`;

    // Update RAM bar breakdown
    if (telemetry.memory) {
      const total = telemetry.memory.totalMB || 32104;
      const sfvm = telemetry.memory.sfvmOnHeapMB || 128;
      const totalUsed = telemetry.memory.usedMB || 20000;
      const systemUsed = Math.max(0, totalUsed - sfvm);
      const free = Math.max(0, total - totalUsed);

      const pSystem = ((systemUsed / total) * 100).toFixed(1);
      const pSfvm = ((sfvm / total) * 100).toFixed(1);
      const pFree = ((free / total) * 100).toFixed(1);

      this.elMemBarSystem.style.width = `${pSystem}%`;
      this.elMemBarSfvm.style.width = `${Math.max(0.8, Number(pSfvm))}%`;
      this.elMemBarFree.style.width = `${pFree}%`;

      this.elMemText.textContent = `OS & Diğerleri: ${(systemUsed / 1024).toFixed(1)} GB (%${pSystem}) | SFVM On-Heap: ${sfvm} MB (%${pSfvm}) | Boş: ${(free / 1024).toFixed(1)} GB (%${pFree})`;
    }

    // Update CPU core bars
    if (telemetry.cpu?.cores && this.elCoresContainer) {
      this.updateCpuCores(telemetry.cpu.cores);
    }

    // Update hardware details
    if (telemetry.gpu?.[0]) {
      const elGpu0 = document.getElementById('spec-gpu0');
      if (elGpu0) elGpu0.textContent = `${telemetry.gpu[0].name} (${telemetry.gpu[0].load}%)`;
    }
    if (telemetry.disks?.[0]) {
      const elDiskC = document.getElementById('spec-disk-c');
      if (elDiskC) elDiskC.textContent = `${telemetry.disks[0].fs} ${telemetry.disks[0].usedGB}/${telemetry.disks[0].sizeGB} GB (%${telemetry.disks[0].usePercent})`;
    }
    if (telemetry.disks?.[1]) {
      const elDiskE = document.getElementById('spec-disk-e');
      if (elDiskE) elDiskE.textContent = `${telemetry.disks[1].fs} ${telemetry.disks[1].usedGB}/${telemetry.disks[1].sizeGB} GB (%${telemetry.disks[1].usePercent})`;
    }
    if (telemetry.network) {
      const elNetAd = document.getElementById('spec-net-adapter');
      if (elNetAd) elNetAd.textContent = `${telemetry.network.iface} (↓${netRx} KB/s, ↑${netTx} KB/s)`;
    }
  }

  private updateCpuCores(cores: number[]): void {
    if (this.elCoresContainer.children.length !== cores.length) {
      this.elCoresContainer.innerHTML = '';
      cores.forEach((_, idx) => {
        const item = document.createElement('div');
        item.className = 'cpu-core-chip';
        item.innerHTML = `
          <div class="core-chip-label">C${idx}</div>
          <div class="core-chip-bar"><div class="core-chip-fill" id="core-fill-${idx}"></div></div>
          <div class="core-chip-pct" id="core-pct-${idx}">0%</div>
        `;
        this.elCoresContainer.appendChild(item);
      });
    }

    cores.forEach((pct, idx) => {
      const fill = document.getElementById(`core-fill-${idx}`);
      const text = document.getElementById(`core-pct-${idx}`);
      if (fill) fill.style.height = `${pct}%`;
      if (text) text.textContent = `${pct}%`;
    });
  }

  private startRenderLoop(): void {
    const render = () => {
      this.drawChart();
      requestAnimationFrame(render);
    };
    requestAnimationFrame(render);
  }

  private drawChart(): void {
    const width = this.canvas.parentElement?.clientWidth || 800;
    const height = 240;
    const ctx = this.ctx;

    ctx.clearRect(0, 0, width, height);

    // Padding
    const padL = 40;
    const padR = 20;
    const padT = 16;
    const padB = 28;
    const plotW = width - padL - padR;
    const plotH = height - padT - padB;

    // Background Grid
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.05)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    for (let p = 0; p <= 100; p += 25) {
      const y = padT + plotH - (p / 100) * plotH;
      ctx.moveTo(padL, y);
      ctx.lineTo(padL + plotW, y);
    }
    ctx.stroke();

    // Y Axis Labels
    ctx.fillStyle = '#64748B';
    ctx.font = '10px JetBrains Mono, monospace';
    ctx.textAlign = 'right';
    for (let p = 0; p <= 100; p += 25) {
      const y = padT + plotH - (p / 100) * plotH;
      ctx.fillText(`${p}%`, padL - 8, y + 3);
    }

    if (this.history.length < 2) {
      ctx.fillStyle = '#475569';
      ctx.textAlign = 'center';
      ctx.fillText('Canlı donanım telemetrisi toplanıyor...', width / 2, height / 2);
      return;
    }

    const seriesConfig = [
      { key: 'cpu', color: '#3B82F6', fill: 'rgba(59, 130, 246, 0.08)' },
      { key: 'gpu', color: '#10B981', fill: 'rgba(16, 185, 129, 0.06)' },
      { key: 'ram', color: '#F59E0B', fill: 'rgba(245, 158, 11, 0.06)' },
      { key: 'disk', color: '#A855F7', fill: 'rgba(168, 85, 247, 0.04)' },
      { key: 'net', color: '#06B6D4', fill: 'rgba(6, 182, 212, 0.04)' }
    ] as const;

    seriesConfig.forEach(({ key, color, fill }) => {
      if (!this.activeSeries[key]) return;

      const pts: { x: number; y: number }[] = [];
      const step = plotW / Math.max(1, this.maxPoints - 1);

      this.history.forEach((h, i) => {
        const val = Math.min(100, Math.max(0, h[key]));
        const x = padL + (this.maxPoints - this.history.length + i) * step;
        const y = padT + plotH - (val / 100) * plotH;
        pts.push({ x, y });
      });

      if (pts.length < 2) return;

      // Draw gradient area
      ctx.fillStyle = fill;
      ctx.beginPath();
      ctx.moveTo(pts[0].x, padT + plotH);
      pts.forEach(p => ctx.lineTo(p.x, p.y));
      ctx.lineTo(pts[pts.length - 1].x, padT + plotH);
      ctx.closePath();
      ctx.fill();

      // Draw crisp line
      ctx.strokeStyle = color;
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.moveTo(pts[0].x, pts[0].y);
      for (let i = 1; i < pts.length; i++) {
        // Smooth line
        const prev = pts[i - 1];
        const curr = pts[i];
        const midX = (prev.x + curr.x) / 2;
        ctx.bezierCurveTo(midX, prev.y, midX, curr.y, curr.x, curr.y);
      }
      ctx.stroke();

      // Current pulse head point
      const last = pts[pts.length - 1];
      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.arc(last.x, last.y, 3.5, 0, Math.PI * 2);
      ctx.fill();
    });

    // X Axis baseline
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.12)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(padL, padT + plotH);
    ctx.lineTo(padL + plotW, padT + plotH);
    ctx.stroke();

    // X Axis Time Labels
    ctx.fillStyle = '#64748B';
    ctx.textAlign = 'left';
    ctx.fillText('-60s', padL, padT + plotH + 16);
    ctx.textAlign = 'right';
    ctx.fillText('ŞİMDİ (0s)', padL + plotW, padT + plotH + 16);
  }
}
