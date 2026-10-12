export interface LogEntry {
  timestamp: string;
  category: 'SYS' | 'GC' | 'RUNWAY' | 'PITSTOP' | 'HEAL' | 'VFS' | 'BENCH';
  level: 'INFO' | 'WARN' | 'ERROR' | 'SUCCESS';
  message: string;
  rawAnsi?: string;
}

function ansiToHtml(raw: string): string {
  let escaped = raw
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;');

  const colorMap: Record<string, string> = {
    '1;30': 'ansi-bold ansi-bright-black',
    '1;31': 'ansi-bold ansi-bright-red',
    '1;32': 'ansi-bold ansi-bright-green',
    '1;33': 'ansi-bold ansi-bright-yellow',
    '1;34': 'ansi-bold ansi-bright-blue',
    '1;35': 'ansi-bold ansi-bright-magenta',
    '1;36': 'ansi-bold ansi-bright-cyan',
    '1;37': 'ansi-bold ansi-bright-white',
    '30': 'ansi-black',
    '31': 'ansi-red',
    '32': 'ansi-green',
    '33': 'ansi-yellow',
    '34': 'ansi-blue',
    '35': 'ansi-magenta',
    '36': 'ansi-cyan',
    '37': 'ansi-white',
    '1': 'ansi-bold'
  };

  let openSpans = 0;
  escaped = escaped.replace(/\x1B\[([0-9;]+)m/g, (_match, code) => {
    if (code === '0') {
      let close = '';
      while (openSpans > 0) {
        close += '</span>';
        openSpans--;
      }
      return close;
    }
    const cls = colorMap[code] || 'ansi-styled';
    openSpans++;
    return `<span class="${cls}">`;
  });

  while (openSpans > 0) {
    escaped += '</span>';
    openSpans--;
  }

  return escaped;
}

export class TerminalConsoleComponent {
  private container: HTMLElement;
  private logListElement!: HTMLElement;
  private categoryFilter: string = 'ALL';
  private logs: LogEntry[] = [];
  private readonly maxLogs = 500;
  private autoScroll: boolean = true;

  constructor(container: HTMLElement) {
    this.container = container;
    this.render();
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
      const text = this.logs.map(l => l.rawAnsi || `[${l.timestamp}] [${l.category}] [${l.level}] ${l.message}`).join('\n');
      navigator.clipboard.writeText(text);
      copyBtn.textContent = 'Copied!';
      setTimeout(() => { copyBtn.textContent = 'Copy'; }, 1500);
    });
  }

  public addLog(entry: LogEntry): void {
    if (!entry) return;

    // Deduplicate exact consecutive duplicates
    if (this.logs.length > 0) {
      const last = this.logs[this.logs.length - 1];
      if (last.message === entry.message && last.category === entry.category && last.timestamp === entry.timestamp) {
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
    row.appendChild(timeSpan);

    if (entry.rawAnsi) {
      const rawSpan = document.createElement('span');
      rawSpan.className = 'term-raw-content';
      rawSpan.innerHTML = ansiToHtml(entry.rawAnsi);
      row.appendChild(rawSpan);
    } else {
      const catSpan = document.createElement('span');
      catSpan.className = `term-cat badge-${entry.category.toLowerCase()}`;
      catSpan.textContent = `[${entry.category}]`;

      const lvlSpan = document.createElement('span');
      lvlSpan.className = `term-lvl lvl-badge-${entry.level.toLowerCase()}`;
      lvlSpan.textContent = `[${entry.level}]`;

      const msgSpan = document.createElement('span');
      msgSpan.className = 'term-msg';
      msgSpan.textContent = entry.message;

      row.appendChild(catSpan);
      row.appendChild(lvlSpan);
      row.appendChild(msgSpan);
    }

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
