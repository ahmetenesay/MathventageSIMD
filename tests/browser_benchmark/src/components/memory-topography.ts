import { MemoryTopographyState } from '../types/telemetry';

export class MemoryTopographyComponent {
  private container: HTMLElement;

  constructor(container: HTMLElement) {
    this.container = container;
    this.renderInitialSkeleton();
  }

  private renderInitialSkeleton(): void {
    this.container.innerHTML = `
      <div class="topography-container">
        <div class="topo-header">
          <span class="topo-title">SFVM MULTI-GENERATIONAL MEMORY TOPOGRAPHY</span>
          <span class="topo-subtitle">Zero-Compaction In-Place Excised Object Flow</span>
        </div>

        <div class="topo-domains-grid">
          <!-- 1. RUNWAY DOMAIN -->
          <div class="topo-card">
            <div class="card-tag">RUNWAY (YOUNG & NURSERY DOMAIN)</div>
            
            <div class="subzone-group">
              <div class="subzone-header">
                <span>MOTHERLINE ACTIVE RUNWAY</span>
                <span class="badge-pill gold" id="topo-motherline-total">0 objs</span>
              </div>
              <div class="zone-split">
                <div class="split-box egg-box">
                  <div class="split-title">Egg Zone (Newborn)</div>
                  <div class="split-val" id="topo-egg-val">0</div>
                  <div class="split-desc">Unoptimized tasks</div>
                </div>
                <div class="split-arrow">➔</div>
                <div class="split-box fledgling-box">
                  <div class="split-title">Fledgling Zone</div>
                  <div class="split-val" id="topo-fledgling-val">0</div>
                  <div class="split-desc">Peak-SIMD validated</div>
                </div>
              </div>
            </div>

            <div class="subzone-group" style="margin-top: 8px;">
              <div class="subzone-header">
                <span>FALLINE (EMPTY-BOX CHAIN)</span>
                <span class="badge-pill" id="topo-falline-val" style="color: #d4a373">0 boxes</span>
              </div>
              <div class="empty-box-strip" id="topo-emptybox-visual">
                <!-- Dynamically populated tokens -->
              </div>
            </div>
          </div>

          <!-- 2. PITSTOP & PUZZLE ARENA -->
          <div class="topo-card">
            <div class="card-tag">PITSTOP & YAPBOZCU ARENA</div>
            
            <div class="pitstop-hero">
              <div class="pitstop-circle" id="topo-pitstop-val">0</div>
              <div class="pitstop-label">In-Flight ISA Re-Steering Slot</div>
              <div class="pitstop-desc">Empty-box placeholder written to Motherline; no memory compaction</div>
            </div>

            <div class="subzone-group" style="margin-top: 4px;">
              <div class="subzone-header">
                <span>YAPBOZCU JIT PROFILER</span>
                <span class="badge-pill" id="topo-tests-val" style="color: #88d49e">3 Active Tests</span>
              </div>
              <div class="region-row">
                <span>Promotion Rate</span>
                <span class="region-val" style="color: #88d49e">98.4% to Fledgling</span>
              </div>
            </div>
          </div>

          <!-- 3. TANK, LANDFILL & RMRP REBATE -->
          <div class="topo-card">
            <div class="card-tag">TANK & LANDFILL REGIONS</div>
            
            <div class="regions-list">
              <div class="region-row">
                <span>Tank Humongous Tensors</span>
                <span class="region-val" id="topo-tank-val">0 MB</span>
              </div>
              <div class="region-row">
                <span>Total Heap Resident</span>
                <span class="region-val" id="topo-alloc-val">0 MB</span>
              </div>
              <div class="region-row">
                <span>Landfill Evacuated</span>
                <span class="region-val" id="topo-landfill-val" style="color: #e5989b">0 MB</span>
              </div>
              <div class="region-row">
                <span>RMRP Reclaimed (Pruned)</span>
                <span class="region-val" id="topo-rmrp-val" style="color: #88d49e">0 MB</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    `;

    this.renderEmptyBoxStrip(64);
  }

  private renderEmptyBoxStrip(count: number): void {
    const strip = document.getElementById('topo-emptybox-visual');
    if (!strip) return;
    strip.innerHTML = '';
    const displayCount = Math.min(36, count);
    for (let i = 0; i < displayCount; ++i) {
      const box = document.createElement('div');
      box.className = 'empty-box-token' + (i % 3 === 0 ? ' used' : '');
      box.title = `Falline Slot #${i + 1} (Size Class: 64B - 4KB)`;
      strip.appendChild(box);
    }
  }

  public update(state: MemoryTopographyState): void {
    const elEgg = document.getElementById('topo-egg-val');
    const elFledgling = document.getElementById('topo-fledgling-val');
    const elTotalMotherline = document.getElementById('topo-motherline-total');
    const elFalline = document.getElementById('topo-falline-val');
    const elPitstop = document.getElementById('topo-pitstop-val');
    const elTests = document.getElementById('topo-tests-val');
    const elTank = document.getElementById('topo-tank-val');
    const elAlloc = document.getElementById('topo-alloc-val');
    const elLandfill = document.getElementById('topo-landfill-val');
    const elRmrp = document.getElementById('topo-rmrp-val');

    if (elEgg) elEgg.textContent = state.eggCount.toString();
    if (elFledgling) elFledgling.textContent = state.fledglingCount.toString();
    if (elTotalMotherline) {
      elTotalMotherline.textContent = `${state.eggCount + state.fledglingCount} objs`;
    }
    if (elFalline) elFalline.textContent = `${state.fallineEmptyBoxes} boxes`;
    if (elPitstop) elPitstop.textContent = state.pitstopCount.toString();
    if (elTests) elTests.textContent = `${state.puzzleArenaActiveTests} Active`;

    const formatMB = (bytes: number) => `${(bytes / (1024 * 1024)).toFixed(1)} MB`;

    if (elTank) elTank.textContent = formatMB(state.tankRegionsBytes);
    if (elAlloc) elAlloc.textContent = formatMB(state.totalAllocatedBytes);
    if (elLandfill) elLandfill.textContent = formatMB(state.landfillEvacuatedBytes);
    if (elRmrp) elRmrp.textContent = formatMB(state.rmrpReclaimedBytes);
  }
}
