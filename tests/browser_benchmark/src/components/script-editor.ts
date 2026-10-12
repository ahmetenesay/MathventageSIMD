import { TS_ScriptRunner } from '../services/native-api';
import { nativeBridge } from '../services/native-bridge';

export class ScriptEditorComponent {
  private container: HTMLElement;
  private editorTextarea!: HTMLTextAreaElement;
  private outputConsole!: HTMLElement;
  private presetSelect!: HTMLSelectElement;
  private executionStats!: HTMLElement;

  private presets: Record<string, string> = {
    gemm: `// Dense Matrix Multiplication (F32) with SFVM Dispatch
const a = new Float32Array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]);
const b = new Float32Array([1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]);
const c = new Float32Array(16);

console.log("[MSIMD] Executing 50,000 passes of 4x4 GEMM...");
for (let i = 0; i < 50000; ++i) {
  MSIMD.mat4f_mul(a, b, c);
}

const zones = SFVM.queryZones();
console.log("[SFVM] Runway Topology -> Egg:", zones.eggCount, "Fledgling:", zones.fledglingCount, "Pitstop:", zones.pitstopCount);
console.log("[SFVM] Total ALU Ops Dispatched:", SFVM.getAluDispatchedCount());
console.log("[MSIMD] Result Matrix diagonal:", c[0], c[5], c[10], c[15]);
`,
    culling: `// Frustum Culling on 4,096 Bounding Boxes
const boxCount = 4096;
const planes = new Float32Array(24);
// Define 6 frustum planes (normals & distances)
planes.set([0, 1, 0, 10,  0, -1, 0, 10,  1, 0, 0, 10,  -1, 0, 0, 10,  0, 0, 1, 50,  0, 0, -1, 1]);

const mins = new Float32Array(boxCount * 3);
const maxs = new Float32Array(boxCount * 3);
for (let i = 0; i < boxCount * 3; ++i) {
  mins[i] = (Math.random() - 0.5) * 40;
  maxs[i] = mins[i] + Math.random() * 2.0;
}
const visible = new Uint8Array(boxCount);

console.log("[MSIMD] Testing 4,096 AABBs against 6 Frustum Planes...");
const visibleCount = MSIMD.cull_frustum_aabbs(planes, mins, maxs, boxCount, visible);

console.log("[MSIMD] Culling completed. Visible AABBs:", visibleCount, "/", boxCount);
console.log("[MSIMD] Rejection Rate:", ((1 - visibleCount / boxCount) * 100).toFixed(1) + "%");
`,
    pitstop: `// Dynamic Pitstop ISA Hot-Swap Simulation
console.log("[SFVM] Inspecting Motherline Runway Zones...");
const before = SFVM.queryZones();
console.log("[SFVM] Current Egg Zone count:", before.eggCount);

console.log("[SFVM] Triggering Pitstop Promotion on unoptimized object...");
SFVM.pitstopPromote();

console.log("[SFVM] Object excised from Motherline into Pitstop empty box.");
console.log("[SFVM] Yapbozcu re-targeting ladder: AVX2 -> AVX10.2 Converged 512");

setTimeout(() => {
  const after = SFVM.queryZones();
  console.log("[SFVM] Promotion finalized. Fledgling count:", after.fledglingCount);
}, 600);
`,
    rmrp: `// Reduce Memory Resident Propagation (RMRP) Heap Compaction
console.log("[SFVM] Initiating RMRP Plump Compaction pass...");
const reclaimed = SFVM.runRmrp('PLUMP');
console.log("[SFVM] Reclaimed Heap Space:", (reclaimed / 1024 / 1024).toFixed(2), "MB");

const budget = SFVM.queryThreadBudget();
console.log("[SFVM] Adaptive Thread Budget -> GC:", budget.gcThreads, "Puzzle:", budget.puzzleThreads, "VM:", budget.vmThreads);
`,
    vfs: `// Virtual File System (VFS) Hierarchy Explorer
console.log("[VFS] Listing root virtual mount points:");
const rootNodes = SFVM.vfsLs('/heap/on_heap');
rootNodes.forEach(node => console.log("  /heap/on_heap/" + node));

console.log("[VFS] Listing execution boot branches:");
const bootNodes = SFVM.vfsLs('/code_boot');
bootNodes.forEach(node => console.log("  /code_boot/" + node));
`
  };

  constructor(container: HTMLElement) {
    this.container = container;
    this.render();
  }

  private render(): void {
    const card = document.createElement('div');
    card.className = 'script-editor-card';

    card.innerHTML = `
      <div class="editor-header">
        <div class="editor-title-group">
          <span class="editor-title">INTERACTIVE TS SCRIPT PLAYGROUND</span>
          <span class="editor-sub">Write and run live code calling SacculusForge_TS (SFVM) and MathventageSIMD_TS (MSIMD)</span>
        </div>
        <div class="editor-actions">
          <label class="preset-label">Preset:</label>
          <select class="editor-preset-select" id="script-preset-select">
            <option value="gemm">Dense 4x4 GEMM (msimd_mat4f_mul)</option>
            <option value="culling">Frustum vs AABB Culling (4,096 boxes)</option>
            <option value="pitstop">Pitstop ISA Hot-Swap Protocol</option>
            <option value="rmrp">RMRP Heap Compaction & Thread Budget</option>
            <option value="vfs">VFS Hierarchy Tree Explorer</option>
          </select>
          <button class="btn-execute-script" id="btn-run-script">
            <span>▶ EXECUTE SCRIPT</span>
          </button>
        </div>
      </div>

      <div class="editor-body">
        <div class="code-pane">
          <textarea class="script-textarea" id="script-code" spellcheck="false"></textarea>
        </div>
        <div class="output-pane">
          <div class="output-header">
            <span>EXECUTION CONSOLE & LOG TRACE</span>
            <span class="exec-stats" id="script-exec-stats">Ready</span>
          </div>
          <div class="output-console" id="script-console-out"></div>
        </div>
      </div>
    `;

    this.container.appendChild(card);

    this.editorTextarea = card.querySelector('#script-code') as HTMLTextAreaElement;
    this.outputConsole = card.querySelector('#script-console-out') as HTMLElement;
    this.presetSelect = card.querySelector('#script-preset-select') as HTMLSelectElement;
    this.executionStats = card.querySelector('#script-exec-stats') as HTMLElement;

    // Load initial preset
    this.editorTextarea.value = this.presets.gemm;

    // Wire events
    this.presetSelect.addEventListener('change', () => {
      const code = this.presets[this.presetSelect.value];
      if (code) {
        this.editorTextarea.value = code;
      }
    });

    const runBtn = card.querySelector('#btn-run-script') as HTMLButtonElement;
    runBtn.addEventListener('click', () => this.execute());
  }

  private execute(): void {
    const code = this.editorTextarea.value;
    this.outputConsole.innerHTML = '<div class="console-line line-dim">Executing script in TS sandbox...</div>';
    this.executionStats.textContent = 'Running...';

    // Also trigger native bridge benchmark pass so hardware counters react
    nativeBridge.runTsBenchmark('custom_script', 65536);

    setTimeout(() => {
      const { result, logs } = TS_ScriptRunner.executeScript(code);

      this.outputConsole.innerHTML = '';
      if (logs.length === 0) {
        this.outputConsole.innerHTML = '<div class="console-line line-dim">[No stdout logs produced]</div>';
      } else {
        logs.forEach(log => {
          const div = document.createElement('div');
          div.className = 'console-line';
          if (log.includes('[ERROR]') || log.includes('[RUNTIME EXCEPTION]')) {
            div.classList.add('line-error');
          } else if (log.includes('[WARN]')) {
            div.classList.add('line-warn');
          } else if (log.includes('[SFVM]')) {
            div.classList.add('line-sfvm');
          } else if (log.includes('[MSIMD]')) {
            div.classList.add('line-msimd');
          } else if (log.includes('[VFS]')) {
            div.classList.add('line-vfs');
          }
          div.textContent = log;
          this.outputConsole.appendChild(div);
        });
      }

      this.executionStats.innerHTML = `
        <span style="color: #88d49e">${result.gflops} GFLOPS</span> | 
        <span style="color: #f6d585">${result.latencyNs} ns/op</span> | 
        <span>${result.totalTimeMs} ms</span>
      `;
      this.outputConsole.scrollTop = this.outputConsole.scrollHeight;
    }, 20);
  }
}
