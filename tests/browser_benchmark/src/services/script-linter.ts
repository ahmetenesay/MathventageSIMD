import headerSymbolsData from './header-symbols.json';

export interface LintDiagnostic {
  line: number;
  column: number;
  severity: 'ERROR' | 'WARN';
  message: string;
  suggestion?: string;
  token: string;
}

export interface ScriptCommand {
  line: number;
  type: 'ALLOC' | 'FREE' | 'KERNEL' | 'RMRP' | 'PITSTOP' | 'SELF_HEAL' | 'GC_COLLECT' | 'FOCUS' | 'BENCH';
  args: string[];
  repeatCount?: number;
  raw: string;
}

export interface ValidationResult {
  isValid: boolean;
  diagnostics: LintDiagnostic[];
  commandCount: number;
  compiledCommands: ScriptCommand[];
}

// Build index of symbols directly from MathventageSIMD.h and SacculusForge.h
const msimdExactSet = new Set<string>(headerSymbolsData.msimd);
const sfExactSet = new Set<string>(headerSymbolsData.sf);

// Also index short forms without "msimd_" prefix for ultra-convenient typing
const msimdShortMap = new Map<string, string>();
for (const fn of headerSymbolsData.msimd) {
  if (fn.startsWith('msimd_')) {
    msimdShortMap.set(fn.substring(6), fn);
  }
}

// Short SFVM commands
const sfShortCommands = new Set<string>([
  'alloc', 'free', 'rmrp', 'pitstop', 'self_heal', 'selfheal',
  'gc_collect', 'collect', 'zones', 'thread_budget', 'budget',
  'upclock', 'focus', 'set_focus', 'repeat', 'bench', 'vfs_mkdir',
  'vfs_bind', 'vfs_evict', 'vfs_ls'
]);

const validDtypes = new Set<string>([
  'f32', 'f64', 'f16', 'f8', 'i32', 'i64', 'i16', 'i8', 'u8',
  'float', 'double', 'int32', 'int64', 'uint8', 'fp32', 'fp64'
]);

const validRmrpMethods = new Set<string>(['plump', 'sequential', 'installment', 'prune']);
const validFocusModes = new Set<string>(['auto', 'scp', 'mcp', 'hybrid_dual']);

function levenshtein(a: string, b: string): number {
  const an = a.length;
  const bn = b.length;
  if (an === 0) return bn;
  if (bn === 0) return an;
  const matrix: number[][] = [];
  for (let i = 0; i <= bn; ++i) matrix[i] = [i];
  for (let j = 0; j <= an; ++j) matrix[0][j] = j;

  for (let i = 1; i <= bn; ++i) {
    for (let j = 1; j <= an; ++j) {
      if (b.charAt(i - 1) === a.charAt(j - 1)) {
        matrix[i][j] = matrix[i - 1][j - 1];
      } else {
        matrix[i][j] = Math.min(
          matrix[i - 1][j - 1] + 1, // substitution
          matrix[i][j - 1] + 1,     // insertion
          matrix[i - 1][j] + 1      // deletion
        );
      }
    }
  }
  return matrix[bn][an];
}

export function findClosestSymbol(unknownWord: string): string | null {
  const lower = unknownWord.toLowerCase();
  let bestCandidate: string | null = null;
  let minDistance = 5; // max tolerance

  // Check SF short commands
  for (const cmd of sfShortCommands) {
    const d = levenshtein(lower, cmd);
    if (d < minDistance) {
      minDistance = d;
      bestCandidate = cmd;
    }
  }

  // Check MSIMD short names
  for (const [shortName, fullName] of msimdShortMap.entries()) {
    const d = levenshtein(lower, shortName.toLowerCase());
    if (d < minDistance) {
      minDistance = d;
      bestCandidate = shortName;
    }
    const dFull = levenshtein(lower, fullName.toLowerCase());
    if (dFull < minDistance) {
      minDistance = dFull;
      bestCandidate = fullName;
    }
  }

  // Check exact SF functions
  for (const sfFn of headerSymbolsData.sf) {
    const d = levenshtein(lower, sfFn.toLowerCase());
    if (d < minDistance) {
      minDistance = d;
      bestCandidate = sfFn;
    }
  }

  return bestCandidate;
}

export function validateSimdForgeScript(sourceCode: string): ValidationResult {
  const diagnostics: LintDiagnostic[] = [];
  const compiledCommands: ScriptCommand[] = [];
  const declaredTensors = new Set<string>();

  const rawLines = sourceCode.split('\n');

  for (let lineIdx = 0; lineIdx < rawLines.length; ++lineIdx) {
    const lineNum = lineIdx + 1;
    let line = rawLines[lineIdx].trim();

    // Remove comments
    const commentIdx = line.indexOf('#');
    if (commentIdx !== -1) line = line.substring(0, commentIdx).trim();
    const slashCommentIdx = line.indexOf('//');
    if (slashCommentIdx !== -1) line = line.substring(0, slashCommentIdx).trim();

    // Semicolons are optional statement separators
    const statements = line.split(';').map(s => s.trim()).filter(Boolean);

    for (const stmt of statements) {
      if (!stmt) continue;

      // Tokenize by whitespace
      const tokens = stmt.split(/\s+/).filter(Boolean);
      if (tokens.length === 0) continue;

      const firstToken = tokens[0];
      const firstLower = firstToken.toLowerCase();

      // 1. ALLOC statement: alloc <var_name> <dtype> [dims...]
      if (firstLower === 'alloc') {
        if (tokens.length < 3) {
          diagnostics.push({
            line: lineNum,
            column: 1,
            severity: 'ERROR',
            token: firstToken,
            message: `Eksik 'alloc' argümanları. Kullanım: alloc <isim> <f32|f64|i32...> [boyutlar...]`
          });
          continue;
        }

        const tensorName = tokens[1];
        if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(tensorName)) {
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(tensorName) + 1,
            severity: 'ERROR',
            token: tensorName,
            message: `Geçersiz tensör değişken adı '${tensorName}'. Geçerli bir tanımlayıcı olmalıdır.`
          });
        }

        const dtype = tokens[2].toLowerCase();
        if (!validDtypes.has(dtype)) {
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(tokens[2]) + 1,
            severity: 'ERROR',
            token: tokens[2],
            message: `Tanımsız veri tipi '${tokens[2]}'. Desteklenen: f32, f64, f16, f8, i32, i64, u8`
          });
        }

        // Validate dimensions
        for (let i = 3; i < tokens.length; ++i) {
          const dimVal = parseInt(tokens[i], 10);
          if (isNaN(dimVal) || dimVal <= 0) {
            diagnostics.push({
              line: lineNum,
              column: stmt.indexOf(tokens[i]) + 1,
              severity: 'ERROR',
              token: tokens[i],
              message: `Tensör boyutu '${tokens[i]}' pozitif bir tamsayı olmalıdır.`
            });
          }
        }

        declaredTensors.add(tensorName);
        compiledCommands.push({
          line: lineNum,
          type: 'ALLOC',
          args: tokens.slice(1),
          raw: stmt
        });
        continue;
      }

      // 2. FREE statement: free <var_name>
      if (firstLower === 'free') {
        if (tokens.length < 2) {
          diagnostics.push({
            line: lineNum,
            column: 1,
            severity: 'ERROR',
            token: firstToken,
            message: `Eksik 'free' argümanı. Kullanım: free <tensör_adı>`
          });
          continue;
        }
        compiledCommands.push({
          line: lineNum,
          type: 'FREE',
          args: [tokens[1]],
          raw: stmt
        });
        continue;
      }

      // 3. REPEAT / BENCH statement: repeat <count> <kernel/cmd> [args...]
      if (firstLower === 'repeat' || firstLower === 'bench') {
        if (tokens.length < 3) {
          diagnostics.push({
            line: lineNum,
            column: 1,
            severity: 'ERROR',
            token: firstToken,
            message: `Eksik '${firstLower}' argümanları. Kullanım: ${firstLower} <tekrar_sayısı> <kernel_adı> [argümanlar...]`
          });
          continue;
        }

        const countVal = parseInt(tokens[1], 10);
        if (isNaN(countVal) || countVal <= 0) {
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(tokens[1]) + 1,
            severity: 'ERROR',
            token: tokens[1],
            message: `Tekrar sayısı '${tokens[1]}' geçerli bir pozitif tamsayı olmalıdır.`
          });
        }

        const subKernel = tokens[2];
        const subKernelResolved = resolveKernelOrCommand(subKernel);
        if (!subKernelResolved.valid) {
          const suggestion = findClosestSymbol(subKernel);
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(subKernel) + 1,
            severity: 'ERROR',
            token: subKernel,
            message: `Tanımsız matematik kerneli '${subKernel}'.`,
            suggestion: suggestion ? `Şunu mu demek istediniz: '${suggestion}'? (MathventageSIMD.h & SacculusForge.h)` : undefined
          });
        }

        compiledCommands.push({
          line: lineNum,
          type: firstLower === 'bench' ? 'BENCH' : 'KERNEL',
          repeatCount: countVal || 1000,
          args: [subKernelResolved.canonicalName, ...tokens.slice(3)],
          raw: stmt
        });
        continue;
      }

      // 4. RMRP Compaction: rmrp [plump|sequential]
      if (firstLower === 'rmrp') {
        const method = (tokens[1] || 'plump').toLowerCase();
        if (!validRmrpMethods.has(method)) {
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(tokens[1]) + 1,
            severity: 'ERROR',
            token: tokens[1],
            message: `Bilinmeyen RMRP yöntemi '${tokens[1]}'. Geçerli: plump, sequential`
          });
        }
        compiledCommands.push({
          line: lineNum,
          type: 'RMRP',
          args: [method],
          raw: stmt
        });
        continue;
      }

      // 5. PITSTOP ISA Re-targeting
      if (firstLower === 'pitstop' || firstLower === 'pitstop_promote' || firstLower === 'pitstop_test') {
        compiledCommands.push({
          line: lineNum,
          type: 'PITSTOP',
          args: tokens.slice(1),
          raw: stmt
        });
        continue;
      }

      // 6. SELF_HEAL engine check
      if (firstLower === 'self_heal' || firstLower === 'selfheal') {
        compiledCommands.push({
          line: lineNum,
          type: 'SELF_HEAL',
          args: [],
          raw: stmt
        });
        continue;
      }

      // 7. GC_COLLECT
      if (firstLower === 'gc_collect' || firstLower === 'collect' || firstLower === 'gc') {
        compiledCommands.push({
          line: lineNum,
          type: 'GC_COLLECT',
          args: [],
          raw: stmt
        });
        continue;
      }

      // 8. FOCUS / SET_FOCUS mode
      if (firstLower === 'focus' || firstLower === 'set_focus') {
        const mode = (tokens[1] || 'auto').toLowerCase();
        if (!validFocusModes.has(mode)) {
          diagnostics.push({
            line: lineNum,
            column: stmt.indexOf(tokens[1]) + 1,
            severity: 'ERROR',
            token: tokens[1],
            message: `Bilinmeyen execution focus modu '${tokens[1]}'. Geçerli: auto, scp, mcp, hybrid_dual`
          });
        }
        compiledCommands.push({
          line: lineNum,
          type: 'FOCUS',
          args: [mode],
          raw: stmt
        });
        continue;
      }

      // 9. Direct MathventageSIMD kernel call: <kernel_name> [args...]
      const kernelRes = resolveKernelOrCommand(firstToken);
      if (kernelRes.valid) {
        compiledCommands.push({
          line: lineNum,
          type: 'KERNEL',
          repeatCount: 1,
          args: [kernelRes.canonicalName, ...tokens.slice(1)],
          raw: stmt
        });
        continue;
      }

      // If nothing matched, this is an undefined symbol!
      const suggestion = findClosestSymbol(firstToken);
      diagnostics.push({
        line: lineNum,
        column: 1,
        severity: 'ERROR',
        token: firstToken,
        message: `Tanımsız komut veya sembol '${firstToken}'.`,
        suggestion: suggestion ? `Şunu mu demek istediniz: '${suggestion}'? (MathventageSIMD.h & SacculusForge.h)` : undefined
      });
    }
  }

  const isValid = diagnostics.filter(d => d.severity === 'ERROR').length === 0;

  return {
    isValid,
    diagnostics,
    commandCount: compiledCommands.length,
    compiledCommands
  };
}

function resolveKernelOrCommand(token: string): { valid: boolean; canonicalName: string } {
  // 1. Check exact match in MSIMD header
  if (msimdExactSet.has(token)) {
    return { valid: true, canonicalName: token };
  }

  // 2. Check short name without msimd_
  if (msimdShortMap.has(token)) {
    return { valid: true, canonicalName: msimdShortMap.get(token)! };
  }

  // 3. Check with "msimd_" prefix prepended
  const prepended = 'msimd_' + token;
  if (msimdExactSet.has(prepended)) {
    return { valid: true, canonicalName: prepended };
  }

  // 4. Check SF exact match
  if (sfExactSet.has(token)) {
    return { valid: true, canonicalName: token };
  }

  // 5. Check SF short commands
  if (sfShortCommands.has(token.toLowerCase())) {
    return { valid: true, canonicalName: token.toLowerCase() };
  }

  return { valid: false, canonicalName: token };
}
