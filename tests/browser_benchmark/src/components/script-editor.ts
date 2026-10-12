import { validateSimdForgeScript, ValidationResult } from '../services/script-linter';
import { nativeBridge } from '../services/native-bridge';

export class ScriptEditorComponent {
  private container: HTMLElement;
  private editorTextarea!: HTMLTextAreaElement;
  private statusBanner!: HTMLElement;
  private diagnosticsList!: HTMLElement;
  private executionStats!: HTMLElement;
  private runBtn!: HTMLButtonElement;
  private lastValidation: ValidationResult | null = null;

  private readonly defaultScript = `# =========================================================================
# SimdForge Dynamic Hardware Benchmark Script
# Syntax: Space-separated commands (semicolons optional)
# Validated live against MathventageSIMD.h & SacculusForge.h
# =========================================================================

# 1. Allocate tensors in SFVM Motherline (Egg Zone)
alloc a f32 16 1
alloc b f32 16 1
alloc c f32 16 1

# 2. Benchmark SIMD Matrix Kernel via Hardware C-ABI
repeat 50000 mat4f_mul a b c

# 3. Trigger SFVM Generational Lifecycle & Safety Gates
rmrp plump
pitstop
self_heal
gc_collect

# 4. Clean up allocated resources
free a
free b
free c
`;

  constructor(container: HTMLElement) {
    this.container = container;
    this.render();
    this.attachEvents();
    this.validate();
  }

  private render(): void {
    const card = document.createElement('div');
    card.className = 'script-editor-card';

    card.innerHTML = `
      <div class="editor-header">
        <div class="editor-title-group">
          <span class="editor-title">SIMDFORGE SCRIPT RUNNER & REAL-TIME SPELL-CHECKER</span>
          <span class="editor-sub">Space-separated syntax. Every keystroke is analyzed against MathventageSIMD.h & SacculusForge.h</span>
        </div>
        <div class="editor-actions">
          <button class="btn-execute-script" id="btn-run-script">
            <span>▶ ÇALIŞTIR (EXECUTE)</span>
          </button>
        </div>
      </div>

      <div class="editor-body-split">
        <!-- Script Textarea -->
        <div class="editor-input-pane">
          <textarea class="script-textarea" id="script-code" spellcheck="false"></textarea>
        </div>

        <!-- Real-Time Diagnostics & Execution Output Pane -->
        <div class="editor-diag-pane">
          <div class="diag-header">
            <span>SÖZDİZİMİ DENETÇİSİ & ÇALIŞTIRMA İZLEYİCİSİ</span>
            <span class="exec-stats" id="script-exec-stats">Hazır</span>
          </div>
          <div class="diag-status-banner" id="script-status-banner">
            <span class="banner-text">Semboller taranıyor...</span>
          </div>
          <div class="diag-list" id="script-diagnostics-list"></div>
        </div>
      </div>
    `;

    this.container.appendChild(card);

    this.editorTextarea = card.querySelector('#script-code') as HTMLTextAreaElement;
    this.statusBanner = card.querySelector('#script-status-banner') as HTMLElement;
    this.diagnosticsList = card.querySelector('#script-diagnostics-list') as HTMLElement;
    this.executionStats = card.querySelector('#script-exec-stats') as HTMLElement;
    this.runBtn = card.querySelector('#btn-run-script') as HTMLButtonElement;

    this.editorTextarea.value = this.defaultScript;
  }

  private attachEvents(): void {
    // Real-time spell checking on every keystroke
    this.editorTextarea.addEventListener('input', () => {
      this.validate();
    });

    this.runBtn.addEventListener('click', () => {
      this.execute();
    });

    // Listen for incoming script execution results from native bridge daemon
    nativeBridge.onScriptResult((res) => {
      if (res && res.type === 'SCRIPT_RESULT') {
        this.runBtn.disabled = false;
        this.runBtn.innerHTML = '<span>▶ ÇALIŞTIR (EXECUTE)</span>';
        this.executionStats.innerHTML = `
          <strong style="color: #88d49e">${res.gflops} GFLOPS</strong> | 
          <strong style="color: #f6d585">${res.latencyNs} ns/op</strong> | 
          <strong style="color: #7eb0d5">${res.cycles} cycles</strong>
        `;

        this.appendOutputLog(`[✓ SFVM Native Run] ${res.operationsRun} operasyon başarıyla tamamlandı via libsfmsimd.dll (${res.gflops} GFLOPS, ${res.latencyNs} ns/op).`, 'success');
      }
    });
  }

  private validate(): void {
    const code = this.editorTextarea.value;
    const result = validateSimdForgeScript(code);
    this.lastValidation = result;

    this.diagnosticsList.innerHTML = '';

    if (result.isValid) {
      this.statusBanner.className = 'diag-status-banner banner-valid';
      this.statusBanner.innerHTML = `
        <span class="status-icon">✓</span>
        <span class="status-msg">
          <strong>GEÇERLİ SÖZDİZİMİ</strong> — MathventageSIMD.h & SacculusForge.h ile %100 doğrulandı (${result.commandCount} operasyon hazır).
        </span>
      `;
      this.runBtn.disabled = false;
      this.runBtn.classList.remove('btn-disabled');
    } else {
      const errorCount = result.diagnostics.filter(d => d.severity === 'ERROR').length;
      this.statusBanner.className = 'diag-status-banner banner-error';
      this.statusBanner.innerHTML = `
        <span class="status-icon">✕</span>
        <span class="status-msg">
          <strong>${errorCount} YAZIM / SÖZDİZİMİ HATASI BULUNDU</strong> — Tanımsız komut veya kernel mevcut.
        </span>
      `;
      this.runBtn.disabled = true;
      this.runBtn.classList.add('btn-disabled');

      result.diagnostics.forEach(diag => {
        const item = document.createElement('div');
        item.className = 'diag-item diag-err';
        item.innerHTML = `
          <span class="diag-line">Satır ${diag.line}:</span>
          <span class="diag-message">${diag.message}</span>
          ${diag.suggestion ? `<span class="diag-suggest">${diag.suggestion}</span>` : ''}
        `;
        this.diagnosticsList.appendChild(item);
      });
    }
  }

  private execute(): void {
    if (!this.lastValidation || !this.lastValidation.isValid) {
      this.validate();
      return;
    }

    const code = this.editorTextarea.value;
    this.runBtn.disabled = true;
    this.runBtn.innerHTML = '<span>⚡ ÇALIŞTIRILIYOR...</span>';
    this.executionStats.textContent = 'Yürütülüyor...';

    this.appendOutputLog(`[Native Bridge] SimdForge scripti libsfmsimd.dll C-ABI arayüzüne gönderiliyor...`, 'info');

    nativeBridge.executeScript(code).then(res => {
      if (res && res.sent) {
        // Handled via WebSocket callback
      } else if (res && res.success) {
        this.runBtn.disabled = false;
        this.runBtn.innerHTML = '<span>▶ ÇALIŞTIR (EXECUTE)</span>';
        this.executionStats.innerHTML = `
          <strong style="color: #88d49e">${res.gflops} GFLOPS</strong> | 
          <strong style="color: #f6d585">${res.latencyNs} ns/op</strong> | 
          <strong style="color: #7eb0d5">${res.cycles} cycles</strong>
        `;
        this.appendOutputLog(`[✓ HTTP C-ABI Run] ${res.operationsRun} operasyon tamamlandı: ${res.gflops} GFLOPS, ${res.latencyNs} ns/op via libsfmsimd.dll.`, 'success');
      } else if (res && res.error) {
        this.runBtn.disabled = false;
        this.runBtn.innerHTML = '<span>▶ ÇALIŞTIR (EXECUTE)</span>';
        this.executionStats.textContent = 'Hata!';
        this.appendOutputLog(`[✕ Hata] Yürütme başarısız: ${res.error}`, 'error');
      }
    });
  }

  private appendOutputLog(msg: string, type: 'info' | 'success' | 'error'): void {
    const row = document.createElement('div');
    row.className = `diag-item diag-${type}`;
    row.textContent = msg;
    this.diagnosticsList.appendChild(row);
    this.diagnosticsList.scrollTop = this.diagnosticsList.scrollHeight;
  }
}
