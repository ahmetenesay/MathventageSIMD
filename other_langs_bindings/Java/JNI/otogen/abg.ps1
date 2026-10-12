<#
.SYNOPSIS
    abg.ps1 - Automated Binding Generator (ABG) for MathventageSIMD & SacculusForge JNI
.DESCRIPTION
    Parses SacculusForge.h and MathventageSIMD.h in other_langs_bindings/Java/JNI/src/
    Generates:
    1. MSIMD_JNI_Loader.java (Cross-platform unpacker & loader for libsfmsimd & sfmsimd_jni)
    2. SacculusForge.java (Full public native bindings for SFVM & sfGC)
    3. MathventageSIMD.java (Full public native bindings for all math & SIMD kernels)
    4. msimd_jni_bridge.cpp (High-performance JNI C++ wrapper bridging Java to C API)
#>

[CmdletBinding()]
param()

# Set InvariantCulture so all ASCII regexes and string comparisons work deterministically
[System.Threading.Thread]::CurrentThread.CurrentCulture = [System.Globalization.CultureInfo]::InvariantCulture
[System.Threading.Thread]::CurrentThread.CurrentUICulture = [System.Globalization.CultureInfo]::InvariantCulture

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$JniRootDir = Split-Path -Parent $ScriptDir
$SrcDir = Join-Path $JniRootDir "src"
$OutDir = Join-Path $JniRootDir "out"

if (-not (Test-Path $OutDir)) {
    New-Item -ItemType Directory -Path $OutDir | Out-Null
}

$SfHeaderPath = Join-Path $SrcDir "SacculusForge.h"
$MsimdHeaderPath = Join-Path $SrcDir "MathventageSIMD.h"

Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host "   MSIMD AUTOMATED JNI BINDINGS GENERATOR (ABG)                   " -ForegroundColor Cyan
Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host "Input Headers :" -ForegroundColor Yellow
Write-Host "  [1] $SfHeaderPath"
Write-Host "  [2] $MsimdHeaderPath"
Write-Host "Output Target : $OutDir" -ForegroundColor Yellow

$KnownEnums = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
@(
    "msimd_status_t",
    "msimd_isa_t",
    "msimd_msc2020_domain_t",
    "msimd_backend_t",
    "msimd_precision_t",
    "msimd_profile_mode_t",
    "SF_custominiVm_dtype_t",
    "SF_custominiVm_tensor_flag_t",
    "SF_custominiVm_tensor_status_t",
    "SF_custominiVm_gc_run_mode_t",
    "SF_custominiVm_gc_heuristic_t",
    "SF_custominiVm_gc_execution_mode_t",
    "SF_custominiVm_scgc_type_t",
    "SF_custominiVm_hookpoint_event_t",
    "SF_custominiVm_yapbozcu_morph_e",
    "SF_custominiVm_hardware_tier_t",
    "SF_custominiVm_event_category_e",
    "SF_custominiVm_severity_e",
    "SF_custominiVm_gc_hook_event_e",
    "SF_custominiVm_cpu_topology_class_t",
    "SF_custominiVm_core_tier_t",
    "SF_custominiVm_mem_architecture_t",
    "SF_custominiVm_smt_mode_t",
    "SF_custominiVm_workload_affinity_policy_t",
    "SF_custominiVm_sram_regime_t",
    "SF_custominiVm_execution_focus_t",
    "SF_custominiVm_host_gc_mode_t",
    "SF_custominiVm_controlpoint_action_t",
    "SF_custominiVm_emu_schema_t"
) | ForEach-Object { [void]$KnownEnums.Add($_) }

# Helper to map C types to Java and JNI types
function Map-CTypeToJava([string]$cType) {
    $cType = $cType.Trim()
    $clean = $cType -replace '^const\s+', ''
    $clean = $clean -replace '\s+const$', ''
    $clean = $clean.Trim()

    # 1. Pointers, arrays, or function pointer types
    if ($clean -match '(\*|\[|\(\*\))') {
        return @{ Java="long"; Jni="jlong"; Sig="J"; IsPointer=$true; IsStructVal=$false; IsEnum=$false }
    }

    # 2. Opaque pointers / handles / callbacks / hooks
    if ($clean -match '^(msimd_context|msimd_lattice|sf_object_ready_callback_t|SF_custominiVm_gc_hook_fn_t|sf_telemetry_stream_fn_t|msimd_parallel_for_fn|sf_vfs_panic_hook_t|.*_fn|.*_fn_t|.*_callback_t|.*_hook_t)$') {
        return @{ Java="long"; Jni="jlong"; Sig="J"; IsPointer=$true; IsStructVal=$false; IsEnum=$false }
    }

    # 3. Known Enums
    if ($KnownEnums.Contains($clean) -or $clean.EndsWith("_e")) {
        return @{ Java="int"; Jni="jint"; Sig="I"; IsPointer=$false; IsStructVal=$false; IsEnum=$true }
    }

    # 4. Standard Primitives
    switch -Regex ($clean) {
        "^void$" { return @{ Java="void"; Jni="void"; Sig="V"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^bool$" { return @{ Java="boolean"; Jni="jboolean"; Sig="Z"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^(char|int8_t|uint8_t)$" { return @{ Java="byte"; Jni="jbyte"; Sig="B"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^(short|int16_t|uint16_t)$" { return @{ Java="short"; Jni="jshort"; Sig="S"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^(int|int32_t|uint32_t|long|unsigned\s+int)$" { return @{ Java="int"; Jni="jint"; Sig="I"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^(long long|int64_t|uint64_t|size_t|uintptr_t|unsigned\s+long\s+long)$" { return @{ Java="long"; Jni="jlong"; Sig="J"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^float$" { return @{ Java="float"; Jni="jfloat"; Sig="F"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        "^double$" { return @{ Java="double"; Jni="jdouble"; Sig="D"; IsPointer=$false; IsStructVal=$false; IsEnum=$false } }
        
        default {
            # 5. Structs by value
            return @{ Java="long"; Jni="jlong"; Sig="J"; IsPointer=$false; IsStructVal=$true; IsEnum=$false }
        }
    }
}

# Function to extract C functions matching MSIMD_API / SF_custominiVm_* / msimd_*
function Parse-HeaderFunctions([string]$headerPath) {
    $content = Get-Content -Path $headerPath -Raw -Encoding UTF8
    
    # Strip comments so commented out code is ignored
    $content = $content -replace '(?s)/\*.*?\*/', '' -replace '(?m)//.*$', ''

    $functions = [System.Collections.Generic.List[PSObject]]::new()
    $seenNames = [System.Collections.Generic.HashSet[string]]::new()

    # Match functions with MSIMD_API
    $pattern = '(?m)^\s*(?:extern\s+"C"\s+)?MSIMD_API\s+([\w\*\s]+?)\s+([a-zA-Z_]\w*)\s*\(([\s\S]*?)\)\s*;'
    $matches = [regex]::Matches($content, $pattern)

    foreach ($m in $matches) {
        $retType = $m.Groups[1].Value.Trim()
        $fnName = $m.Groups[2].Value.Trim()
        $paramsRaw = $m.Groups[3].Value.Trim()

        # Skip typedefs or function pointer declarations
        if ($fnName.StartsWith("(") -or $retType.Contains("typedef")) {
            continue
        }

        # Deduplicate function names (C API functions have unique global symbols)
        if ($seenNames.Contains($fnName)) {
            continue
        }
        $seenNames.Add($fnName) | Out-Null

        $paramList = [System.Collections.Generic.List[PSObject]]::new()
        if ($paramsRaw -and $paramsRaw -ne "void") {
            # Balanced split by comma outside parentheses
            $rawArgs = [System.Collections.Generic.List[string]]::new()
            $parenDepth = 0
            $curArg = [System.Text.StringBuilder]::new()
            for ($ci = 0; $ci -lt $paramsRaw.Length; $ci++) {
                $ch = $paramsRaw[$ci]
                if ($ch -eq '(') {
                    $parenDepth++
                    [void]$curArg.Append($ch)
                } elseif ($ch -eq ')') {
                    $parenDepth--
                    [void]$curArg.Append($ch)
                } elseif ($ch -eq ',' -and $parenDepth -eq 0) {
                    $rawArgs.Add($curArg.ToString().Trim())
                    [void]$curArg.Clear()
                } else {
                    [void]$curArg.Append($ch)
                }
            }
            if ($curArg.Length -gt 0) {
                $rawArgs.Add($curArg.ToString().Trim())
            }

            $argIdx = 0
            foreach ($arg in $rawArgs) {
                $argClean = $arg.Trim()
                if (-not $argClean) { continue }
                
                # Strip default parameter assignment or macros like MSIMD_DEFAULT_PRECISION or = ...
                $argClean = $argClean -replace '\s+MSIMD_DEFAULT_PRECISION\b', ''
                $argClean = $argClean -replace '\s*=[^,)]+', ''

                # Check if it's an inline function pointer parameter: ret (*name)(args)
                $fnPtrMatch = [regex]::Match($argClean, '\(\s*\*+\s*([a-zA-Z_]\w*)\s*\)')
                if ($fnPtrMatch.Success) {
                    $pName = $fnPtrMatch.Groups[1].Value
                    $pat = '\(\s*\*+\s*' + [regex]::Escape($pName) + '\s*\)'
                    $pType = $argClean -replace $pat, '(*)'
                    $paramList.Add([PSCustomObject]@{
                        Name = $pName
                        CType = $pType
                    })
                    $argIdx++
                    continue
                }

                # Check for array parameter: e.g. const float ox[8], double T[4][4], const double gamma[4][4][4]
                $arrMatch = [regex]::Match($argClean, '^([\w\s\*]+?)\s+([a-zA-Z_]\w*)((?:\s*\[\s*\d*\s*\])+)$')
                if ($arrMatch.Success) {
                    $baseType = $arrMatch.Groups[1].Value.Trim()
                    $pName = $arrMatch.Groups[2].Value.Trim()
                    $bracketSuffix = $arrMatch.Groups[3].Value.Trim()

                    if ($pName -match '^(double|float|int|long|short|byte|boolean|char|class|default|break|continue|return)$') {
                        $pName = "p_$pName"
                    }

                    # C/C++ decay rule: first dimension decays to pointer (*), remaining dimensions stay
                    $firstBracketRemoved = $bracketSuffix -replace '^\s*\[\s*\d*\s*\]', ''
                    if ($firstBracketRemoved) {
                        $decayedType = "$baseType (*)$firstBracketRemoved"
                    } else {
                        $decayedType = "$baseType*"
                    }

                    $paramList.Add([PSCustomObject]@{
                        Name = $pName
                        CType = $decayedType
                    })
                    $argIdx++
                    continue
                }

                # Extract arg name and type
                $tokens = $argClean -split '\s+'
                $pName = "arg$argIdx"
                $pType = $argClean
                if ($tokens.Count -gt 1) {
                    $candidateName = $tokens[-1].Trim('*', '&', '[', ']')
                    # Guard reserved keywords in Java
                    if ($candidateName -match '^(double|float|int|long|short|byte|boolean|char|class|default|break|continue|return)$') {
                        $candidateName = "p_$candidateName"
                    }
                    if ($candidateName -match '^[a-zA-Z_]\w*$') {
                        $pName = $candidateName
                        $pType = ($tokens[0..($tokens.Count-2)] -join ' ')
                        if ($tokens[-1].StartsWith('*')) {
                            $pType += "*"
                        }
                    }
                }
                $paramList.Add([PSCustomObject]@{
                    Name = $pName
                    CType = $pType
                })
                $argIdx++
            }
        }

        $functions.Add([PSCustomObject]@{
            Name = $fnName
            ReturnType = $retType
            Params = $paramList
        })
    }
    return $functions
}

# 1. GENERATE MSIMD_JNI_Loader.java
$loaderCode = @"
package net.chiretallyn.mathventagesimd;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Universal Zero-Overhead JNI Native Library Loader for MathventageSIMD & SacculusForge.
 * Automatically resolves and loads libsfmsimd and sfmsimd_jni across Windows, Linux and macOS.
 */
public final class MSIMD_JNI_Loader {

    private static final AtomicBoolean LOADED = new AtomicBoolean(false);

    private MSIMD_JNI_Loader() {}

    public static void load() {
        if (LOADED.get()) return;
        synchronized (LOADED) {
            if (LOADED.get()) return;

            String osName = System.getProperty("os.name", "").toLowerCase(Locale.ROOT);
            String osArch = System.getProperty("os.arch", "").toLowerCase(Locale.ROOT);

            String osFolder;
            if (osName.contains("win")) {
                osFolder = "windows";
            } else if (osName.contains("mac") || osName.contains("darwin")) {
                osFolder = "macos";
            } else {
                osFolder = "linux";
            }

            String archFolder;
            if (osArch.contains("aarch64") || osArch.contains("arm64")) {
                archFolder = "arm64";
            } else {
                archFolder = "x64";
            }

            String targetPlatform = osFolder + "-" + archFolder;
            loadNativeLibraries(targetPlatform);
            LOADED.set(true);
        }
    }

    private static void loadNativeLibraries(String platform) {
        String[] libNames;
        String ext;
        if (platform.startsWith("windows")) {
            libNames = new String[] { "libsfmsimd.dll", "sfmsimd_jni.dll" };
            ext = ".dll";
        } else if (platform.startsWith("macos")) {
            libNames = new String[] { "libsfmsimd.dylib", "libsfmsimd_jni.dylib" };
            ext = ".dylib";
        } else {
            libNames = new String[] { "libsfmsimd.so", "libsfmsimd_jni.so" };
            ext = ".so";
        }

        for (String libName : libNames) {
            try {
                // Try direct System.loadLibrary first (if on java.library.path)
                String baseName = libName.replaceFirst("^lib", "").replaceFirst("\\Q" + ext + "\\E$", "");
                System.loadLibrary(baseName);
            } catch (UnsatisfiedLinkError e1) {
                // Extract from JAR resources
                try {
                    String resourcePath = "/natives/" + platform + "/" + libName;
                    InputStream in = MSIMD_JNI_Loader.class.getResourceAsStream(resourcePath);
                    if (in == null) {
                        resourcePath = "/natives/" + libName;
                        in = MSIMD_JNI_Loader.class.getResourceAsStream(resourcePath);
                    }
                    if (in == null) {
                        // Fallback to local ./out directory or JNI out directory
                        File localFile = new File("out/" + platform + "/" + libName);
                        if (!localFile.exists()) {
                            localFile = new File("other_langs_bindings/Java/JNI/out/" + libName);
                        }
                        if (localFile.exists()) {
                            System.load(localFile.getAbsolutePath());
                            continue;
                        }
                        throw new UnsatisfiedLinkError("Could not find native resource: " + libName);
                    }

                    File tempFile = File.createTempFile("sfmsimd_", "_" + libName);
                    tempFile.deleteOnExit();
                    try (OutputStream out = new FileOutputStream(tempFile)) {
                        byte[] buf = new byte[65536];
                        int r;
                        while ((r = in.read(buf)) != -1) {
                            out.write(buf, 0, r);
                        }
                    }
                    System.load(tempFile.getAbsolutePath());
                } catch (Exception e2) {
                    throw new RuntimeException("Failed to load native library: " + libName, e2);
                }
            }
        }
    }
}
"@

$LoaderOutFile = Join-Path $OutDir "MSIMD_JNI_Loader.java"
$Utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText($LoaderOutFile, $loaderCode, $Utf8NoBom)
Write-Host "  [+] Generated: MSIMD_JNI_Loader.java" -ForegroundColor Green

# 2. PARSE HEADERS
Write-Host "Parsing SacculusForge.h..." -ForegroundColor Cyan
$sfFunctions = Parse-HeaderFunctions $SfHeaderPath
Write-Host "  --> Found $($sfFunctions.Count) SacculusForge C API functions." -ForegroundColor Green

Write-Host "Parsing MathventageSIMD.h..." -ForegroundColor Cyan
$msimdFunctions = Parse-HeaderFunctions $MsimdHeaderPath
Write-Host "  --> Found $($msimdFunctions.Count) MathventageSIMD C API functions." -ForegroundColor Green

# 3. GENERATE SacculusForge.java
$sbSfJava = [System.Text.StringBuilder]::new()
[void]$sbSfJava.AppendLine("package net.chiretallyn.mathventagesimd;")
[void]$sbSfJava.AppendLine("")
[void]$sbSfJava.AppendLine("/**")
[void]$sbSfJava.AppendLine(" * SacculusForge Virtual Machine (SFVM) & sfGC Runtime JNI Native Interface.")
[void]$sbSfJava.AppendLine(" */")
[void]$sbSfJava.AppendLine("public final class SacculusForge {")
[void]$sbSfJava.AppendLine("")
[void]$sbSfJava.AppendLine("    static {")
[void]$sbSfJava.AppendLine("        MSIMD_JNI_Loader.load();")
[void]$sbSfJava.AppendLine("    }")
[void]$sbSfJava.AppendLine("")
[void]$sbSfJava.AppendLine("    private SacculusForge() {}")
[void]$sbSfJava.AppendLine("")

foreach ($fn in $sfFunctions) {
    $retMap = Map-CTypeToJava $fn.ReturnType
    $jArgs = [System.Collections.Generic.List[string]]::new()
    foreach ($p in $fn.Params) {
        $pMap = Map-CTypeToJava $p.CType
        $jArgs.Add("$($pMap.Java) $($p.Name)")
    }
    $jArgStr = $jArgs -join ", "
    [void]$sbSfJava.AppendLine("    public static native $($retMap.Java) $($fn.Name)($jArgStr);")
}

[void]$sbSfJava.AppendLine("}")

$SfOutFile = Join-Path $OutDir "SacculusForge.java"
[System.IO.File]::WriteAllText($SfOutFile, $sbSfJava.ToString(), $Utf8NoBom)
Write-Host "  [+] Generated: SacculusForge.java ($($sfFunctions.Count) native methods)" -ForegroundColor Green

# 4. GENERATE MathventageSIMD.java
$sbMsimdJava = [System.Text.StringBuilder]::new()
[void]$sbMsimdJava.AppendLine("package net.chiretallyn.mathventagesimd;")
[void]$sbMsimdJava.AppendLine("")
[void]$sbMsimdJava.AppendLine("/**")
[void]$sbMsimdJava.AppendLine(" * MathventageSIMD High-Performance Mathematical & Waveform Kernel JNI Native Interface.")
[void]$sbMsimdJava.AppendLine(" */")
[void]$sbMsimdJava.AppendLine("public final class MathventageSIMD {")
[void]$sbMsimdJava.AppendLine("")
[void]$sbMsimdJava.AppendLine("    static {")
[void]$sbMsimdJava.AppendLine("        MSIMD_JNI_Loader.load();")
[void]$sbMsimdJava.AppendLine("    }")
[void]$sbMsimdJava.AppendLine("")
[void]$sbMsimdJava.AppendLine("    private MathventageSIMD() {}")
[void]$sbMsimdJava.AppendLine("")

foreach ($fn in $msimdFunctions) {
    $retMap = Map-CTypeToJava $fn.ReturnType
    $jArgs = [System.Collections.Generic.List[string]]::new()
    foreach ($p in $fn.Params) {
        $pMap = Map-CTypeToJava $p.CType
        $jArgs.Add("$($pMap.Java) $($p.Name)")
    }
    $jArgStr = $jArgs -join ", "
    [void]$sbMsimdJava.AppendLine("    public static native $($retMap.Java) $($fn.Name)($jArgStr);")
}

[void]$sbMsimdJava.AppendLine("}")

$MsimdOutFile = Join-Path $OutDir "MathventageSIMD.java"
[System.IO.File]::WriteAllText($MsimdOutFile, $sbMsimdJava.ToString(), $Utf8NoBom)
Write-Host "  [+] Generated: MathventageSIMD.java ($($msimdFunctions.Count) native methods)" -ForegroundColor Green

# 5. GENERATE C++ JNI BRIDGE (msimd_jni_bridge.cpp)
$sbCpp = [System.Text.StringBuilder]::new()
[void]$sbCpp.AppendLine('/* ========================================================================= */')
[void]$sbCpp.AppendLine('/* MathventageSIMD - JNI Bridge Implementation                              */')
[void]$sbCpp.AppendLine('/* Autogenerated via abg.ps1 (Automated Binding Generator)                  */')
[void]$sbCpp.AppendLine('/* ========================================================================= */')
[void]$sbCpp.AppendLine('#include <jni.h>')
[void]$sbCpp.AppendLine('#include <cstdint>')
[void]$sbCpp.AppendLine('#include "SacculusForge.h"')
[void]$sbCpp.AppendLine('#include "MathventageSIMD.h"')
[void]$sbCpp.AppendLine('')
[void]$sbCpp.AppendLine('#ifdef __cplusplus')
[void]$sbCpp.AppendLine('extern "C" {')
[void]$sbCpp.AppendLine('#endif')
[void]$sbCpp.AppendLine('')

# Helper to generate JNI function implementation
function Emit-JniCppFunction([string]$className, $fn) {
    $retMap = Map-CTypeToJava $fn.ReturnType
    $cleanFnName = $fn.Name.Replace("_", "_1")
    $jniFnName = "Java_net_chiretallyn_mathventagesimd_${className}_${cleanFnName}"

    $cppParams = [System.Collections.Generic.List[string]]::new()
    $cppParams.Add("JNIEnv* env")
    $cppParams.Add("jclass clazz")

    $callArgs = [System.Collections.Generic.List[string]]::new()
    foreach ($p in $fn.Params) {
        $pMap = Map-CTypeToJava $p.CType
        $cppParams.Add("$($pMap.Jni) $($p.Name)")

        if ($pMap.IsStructVal) {
            $callArgs.Add("*reinterpret_cast<const $($p.CType)*>($($p.Name))")
        } elseif ($pMap.IsPointer) {
            $callArgs.Add("reinterpret_cast<$($p.CType)>($($p.Name))")
        } else {
            $callArgs.Add("static_cast<$($p.CType)>($($p.Name))")
        }
    }
    $cppParamStr = $cppParams -join ", "
    $callArgsStr = $callArgs -join ", "

    $out = [System.Text.StringBuilder]::new()
    [void]$out.AppendLine("JNIEXPORT $($retMap.Jni) JNICALL ${jniFnName}($cppParamStr) {")
    if ($retMap.Jni -eq "void") {
        [void]$out.AppendLine("    $($fn.Name)($callArgsStr);")
    } elseif ($retMap.IsStructVal) {
        [void]$out.AppendLine("    auto _res = $($fn.Name)($callArgsStr);")
        [void]$out.AppendLine("    return static_cast<jlong>(reinterpret_cast<uintptr_t>(new $($fn.ReturnType)(_res)));")
    } elseif ($retMap.IsPointer) {
        [void]$out.AppendLine("    return static_cast<jlong>(reinterpret_cast<uintptr_t>($($fn.Name)($callArgsStr)));")
    } else {
        [void]$out.AppendLine("    return static_cast<$($retMap.Jni)>($($fn.Name)($callArgsStr));")
    }
    [void]$out.AppendLine("}")
    [void]$out.AppendLine("")
    return $out.ToString()
}

# Emit SacculusForge JNI C++ methods
foreach ($fn in $sfFunctions) {
    [void]$sbCpp.Append((Emit-JniCppFunction "SacculusForge" $fn))
}

# Emit MathventageSIMD JNI C++ methods
foreach ($fn in $msimdFunctions) {
    [void]$sbCpp.Append((Emit-JniCppFunction "MathventageSIMD" $fn))
}

[void]$sbCpp.AppendLine("#ifdef __cplusplus")
[void]$sbCpp.AppendLine("}")
[void]$sbCpp.AppendLine("#endif")

$CppOutFile = Join-Path $SrcDir "msimd_jni_bridge.cpp"
[System.IO.File]::WriteAllText($CppOutFile, $sbCpp.ToString(), $Utf8NoBom)
Write-Host "  [+] Generated: msimd_jni_bridge.cpp ($($sfFunctions.Count + $msimdFunctions.Count) JNI bridge implementations)" -ForegroundColor Green

Write-Host "=================================================================" -ForegroundColor Cyan
Write-Host "   ABG COMPLETED CLEANLY: 3 Java Classes + 1 C++ Bridge File     " -ForegroundColor Green
Write-Host "=================================================================" -ForegroundColor Cyan
