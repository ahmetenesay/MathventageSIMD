export interface LogEntry {
  timestamp: string;
  category: 'SYS' | 'GC' | 'RUNWAY' | 'PITSTOP' | 'HEAL' | 'VFS' | 'BENCH';
  level: 'INFO' | 'WARN' | 'ERROR' | 'SUCCESS';
  message: string;
}

export class TerminalConsoleComponent {
  private container: HTMLElement;
  private logListElement!: HTMLElement;
  private categoryFilter: string = 'ALL';
  private logs: LogEntry[] = [];
  private readonly maxLogs = 250;
  private autoScroll: boolean = true;

  constructor(container: HTMLElement) {
    this.container = container;
    this.render();
    this.seedInitialLogs();
  }

  private render(): void {
    const card = document.createElement('div');
    card.className = 'terminal-console-card';

    card.innerHTML = `
      <div class="terminal-header">
        <div class="terminal-title-group">
          <span class="terminal-title">SFVM DIAGNOSTIC TERMINAL & LOGGER</span>
          <span class="terminal-sub">Native C-ABI Diagnostic Stream & Subsystem Activity</span>
        </div>
        <div class="terminal-controls">
          <label class="filter-label">Filter:</label>
          <select class="terminal-category-select" id="terminal-filter-select">
            <option value="ALL">All Categories</option>
            <option value="SYS">[SYS] Runtime Nexus</option>
            <option value="GC">[GC] Garbage Collector</option>
            <option value="RUNWAY">[RUNWAY] Memory Topography</option>
            <option value="PITSTOP">[PITSTOP] ISA Re-targeting</option>
            <option value="HEAL">[HEAL] Self-Healing Engine</option>
            <option value="VFS">[VFS] Virtual File System</option>
            <option value="BENCH">[BENCH] Benchmark Harness</option>
          </select>
          <button class="btn-terminal-action" id="btn-clear-terminal">Clear</button>
          <button class="btn-terminal-action" id="btn-copy-terminal">Copy</button>
        </div>
      </div>
      <div class="terminal-body" id="terminal-log-output"></div>
    `;

    this.container.appendChild(card);

    this.logListElement = card.querySelector('#terminal-log-output') as HTMLElement;
    const filterSelect = card.querySelector('#terminal-filter-select') as HTMLSelectElement;
    filterSelect.addEventListener('change', () => {
      this.categoryFilter = filterSelect.value;
      this.refreshLogDisplay();
    });

    const clearBtn = card.querySelector('#btn-clear-terminal') as HTMLButtonElement;
    clearBtn.addEventListener('click', () => {
      this.logs = [];
      this.refreshLogDisplay();
    });

    const copyBtn = card.querySelector('#btn-copy-terminal') as HTMLButtonElement;
    copyBtn.addEventListener('click', () => {
      const text = this.logs.map(l => `[${l.timestamp}] [${l.category}] [${l.level}] ${l.message}`).join('\n');
      navigator.clipboard.writeText(text);
      copyBtn.textContent = 'Copied!';
      setTimeout(() => { copyBtn.textContent = 'Copy'; }, 1500);
    });
  }

  private seedInitialLogs(): void {
    const now = () => new Date().toLocaleTimeString('en-US', { hour12: false });
    this.addLog({
      timestamp: now(),
      category: 'SYS',
      level: 'INFO',
      message: 'SacculusForge SFVM runtime initialized via C-ABI at 64-bit boundary.'
    });
    this.addLog({
      timestamp: now(),
      category: 'SYS',
      level: 'INFO',
      message: 'Discovered hardware profile: AMD/Intel Hybrid with AVX2 & AVX512 capability.'
    });
    this.addLog({
      timestamp: now(),
      category: 'RUNWAY',
      level: 'SUCCESS',
      message: 'Motherline split initialized: Egg zone allocated, Fledgling zone ready.'
    });
    this.addLog({
      timestamp: now(),
      category: 'PITSTOP',
      level: 'INFO',
      message: 'Pitstop buffer armed. Empty box protocol ready for non-compacting migrations.'
    });
    this.addLog({
      timestamp: now(),
      category: 'VFS',
      level: 'INFO',
      message: 'Virtual hierarchy /heap/on_heap mounted with strict fail-fast validation.'
    });
  }

  public addLog(entry: LogEntry): void {
    // Avoid spamming exact duplicate consecutive lines
    if (this.logs.length > 0) {
      const last = this.logs[this.logs.length - 1];
      if (last.message === entry.message && Math.abs(Number(new Date(last.timestamp)) - Number(new Date(entry.timestamp))) < 1000) {
        return;
      }
    }

    this.logs.push(entry);
    if (this.logs.length > this.maxLogs) {
      this.logs.shift();
    }
    this.appendSingleLog(entry);
  }

  private appendSingleLog(entry: LogEntry): void {
    if (this.categoryFilter !== 'ALL' && entry.category !== this.categoryFilter) {
      return;
    }

    const row = document.createElement('div');
    row.className = `term-row cat-${entry.category.toLowerCase()} lvl-${entry.level.toLowerCase()}`;

    const timeSpan = document.createElement('span');
    timeSpan.className = 'term-time';
    timeSpan.textContent = `[${entry.timestamp}]`;

    const catSpan = document.createElement('span');
    catSpan.className = `term-cat badge-${entry.category.toLowerCase()}`;
    catSpan.textContent = `[${entry.category}]`;

    const lvlSpan = document.createElement('span');
    lvlSpan.className = `term-lvl lvl-badge-${entry.level.toLowerCase()}`;
    lvlSpan.textContent = `[${entry.level}]`;

    const msgSpan = document.createElement('span');
    msgSpan.className = 'term-msg';
    msgSpan.textContent = entry.message;

    row.appendChild(timeSpan);
    row.appendChild(catSpan);
    row.appendChild(lvlSpan);
    row.appendChild(msgSpan);

    this.logListElement.appendChild(row);

    if (this.autoScroll) {
      this.logListElement.scrollTop = this.logListElement.scrollHeight;
    }
  }

  private refreshLogDisplay(): void {
    this.logListElement.innerHTML = '';
    const filtered = this.categoryFilter === 'ALL'
      ? this.logs
      : this.logs.filter(l => l.category === this.categoryFilter);

    filtered.forEach(entry => this.appendSingleLog(entry));
  }
}
