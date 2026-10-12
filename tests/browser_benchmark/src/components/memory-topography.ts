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
          <span class="topo-subtitle">Zero-Syscall In-Place Excised Object Flow</span>
        </div>

        <div class="topo-domains-grid">
          <!-- 1. RUNWAY DOMAIN -->
          <div class="topo-card runway-card">
            <div class="card-tag">RUNWAY (YOUNG & NURSERY DOMAIN)</div>
            
            <div class="subzone-group">
              <div class="subzone-header">
                <span>MOTHERLINE</span>
                <span class="badge-count" id="topo-motherline-total">0 objs</span>
              </div>
              <div class="zone-split">
                <div class="split-box egg-box">
                  <div class="split-title">Egg Zone (Newborn)</div>
                  <div class="split-val" id="topo-egg-val">0</div>
                  <div class="split-desc">Unoptimized candidates</div>
                </div>
                <div class="split-arrow">➔</div>
                <div class="split-box fledgling-box">
                  <div class="split-title">Fledgling Zone</div>
                  <div class="split-val" id="topo-fledgling-val">0</div>
                  <div class="split-desc">Validated ISA intrinsics</div>
                </div>
              </div>
            </div>

            <div class="subzone-group mt-2">
              <div class="subzone-header">
                <span>FALLINE (EMPTY BOX CHAIN)</span>
                <span class="badge-count yellow" id="topo-falline-val">0 boxes</span>
              </div>
              <div class="empty-box-strip" id="topo-emptybox-visual">
                <!-- Visual box tokens populated dynamically -->
              </div>
            </div>
          </div>

          <!-- 2. PITSTOP & PUZZLE ARENA -->
          <div class="topo-card pitstop-card">
            <div class="card-tag yellow-tag">PITSTOP & YAPBOZCU ARENA</div>
            
            <div class="pitstop-hero">
              <div class="pitstop-circle">
                <span class="pit-val" id="topo-pitstop-val">0</span>
                <span class="pit-lbl">IN PITSTOP</span>
              </div>
              <div class="pit-desc">
                <strong>In-Place Excised Mutation:</strong>
                Tensors temporarily excised into Pitstop for dynamic ISA auto-tuning and cycle-gain evaluation without memory compaction.
              </div>
            </div>

            <div class="puzzle-strip">
              <div class="puzzle-lbl">Yapbozcu Puzzle Arena:</div>
              <div class="puzzle-status">
                <span class="pulse-dot"></span>
                <span id="topo-puzzle-status">2 Active Kernel Contenders</span>
              </div>
            </div>
          </div>

          <!-- 3. TANK & LANDFILL DOMAINS -->
          <div class="topo-card tank-card">
            <div class="card-tag blue-tag">OFF-RUNWAY DOMAINS</div>
            
            <div class="off-domain-row">
              <div class="domain-mini">
                <span class="mini-lbl">TANK REGIONS (HUMONGOUS)</span>
                <span class="mini-val cyan" id="topo-tank-val">0 MB</span>
                <span class="mini-desc">Bypasses generational sliding compaction</span>
              </div>
              <div class="domain-mini">
                <span class="mini-lbl">LANDFILL (MATURE DOMAIN)</span>
                <span class="mini-val purple" id="topo-landfill-val">0 MB</span>
                <span class="mini-desc">Evacuation to swap pagefile</span>
              </div>
            </div>

            <div class="rmrp-stat-bar">
              <div class="rmrp-txt">
                <span>RMRP Reclaimed:</span>
                <strong class="green" id="topo-rmrp-val">0 MB</strong>
              </div>
              <div class="rmrp-txt">
                <span>Total Managed Arena:</span>
                <strong class="white" id="topo-total-arena">0 MB</strong>
              </div>
            </div>
          </div>
        </div>
      </div>
    `;
  }

  public update(state: MemoryTopographyState): void {
    const elEgg = document.getElementById('topo-egg-val');
    const elFled = document.getElementById('topo-fledgling-val');
    const elMotherTotal = document.getElementById('topo-motherline-total');
    const elFalline = document.getElementById('topo-falline-val');
    const elEmptyStrip = document.getElementById('topo-emptybox-visual');
    const elPitstop = document.getElementById('topo-pitstop-val');
    const elTank = document.getElementById('topo-tank-val');
    const elLandfill = document.getElementById('topo-landfill-val');
    const elRmrp = document.getElementById('topo-rmrp-val');
    const elTotal = document.getElementById('topo-total-arena');
    const elPuzzle = document.getElementById('topo-puzzle-status');

    if (elEgg) elEgg.textContent = state.eggCount.toLocaleString();
    if (elFled) elFled.textContent = state.fledglingCount.toLocaleString();
    if (elMotherTotal) {
      elMotherTotal.textContent = `${(state.eggCount + state.fledglingCount).toLocaleString()} objs`;
    }
    if (elFalline) elFalline.textContent = `${state.fallineEmptyBoxes} boxes`;

    if (elEmptyStrip) {
      const boxCount = Math.min(24, Math.max(4, Math.floor(state.fallineEmptyBoxes / 3)));
      let html = '';
      for (let i = 0; i < boxCount; ++i) {
        html += `<span class="box-token" title="Segregated Empty Box ${i}">⬡</span>`;
      }
      elEmptyStrip.innerHTML = html;
    }

    if (elPitstop) elPitstop.textContent = state.pitstopCount.toString();
    if (elTank) elTank.textContent = `${(state.tankRegionsBytes / (1024 * 1024)).toFixed(1)} MB`;
    if (elLandfill) elLandfill.textContent = `${(state.landfillEvacuatedBytes / (1024 * 1024)).toFixed(1)} MB`;
    if (elRmrp) elRmrp.textContent = `${(state.rmrpReclaimedBytes / (1024 * 1024)).toFixed(1)} MB`;
    if (elTotal) elTotal.textContent = `${(state.totalAllocatedBytes / (1024 * 1024)).toFixed(1)} MB`;

    if (elPuzzle) {
      elPuzzle.textContent = `${state.puzzleArenaActiveTests} Active Kernel Contenders`;
    }
  }
}
