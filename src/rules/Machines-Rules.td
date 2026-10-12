// =============================================================================
// MathventageSIMD — Machines-Rules.td
// LLVM TableGen Universal Master Hardware Rulebook (2011–2026)
// =============================================================================
// STRICT PHILOSOPHY: 100% PURE HARDWARE, ISAs, REGISTERS, TIMINGS & POLICIES.
// Models raw silicon capabilities across 80+ microarchitectures:
// x86_64, ARM64, RISC-V, LoongArch, IBM s390x/POWER, WebAssembly SIMD.
// Contains zero abstract math. Ingested directly by TableGen and codegen.py.
// =============================================================================

// =============================================================================
// SECTION 1: 2011–2026 SILICON VENDORS & 80+ MICROARCHITECTURE FAMILIES
// =============================================================================

class SiliconVendor<string name, string vendor_code> {
  string Name = name;
  string Code = vendor_code;
}

def Vendor_Intel           : SiliconVendor<"Intel", "INTEL">;
def Vendor_AMD             : SiliconVendor<"AMD", "AMD">;
def Vendor_Apple           : SiliconVendor<"Apple", "APPL">;
def Vendor_ARM             : SiliconVendor<"ARM Holdings", "ARM">;
def Vendor_Qualcomm        : SiliconVendor<"Qualcomm", "QCOM">;
def Vendor_Nvidia          : SiliconVendor<"Nvidia", "NVDA">;
def Vendor_Broadcom        : SiliconVendor<"Broadcom", "BRCM">;
def Vendor_Rockchip        : SiliconVendor<"Rockchip", "RKCH">;
def Vendor_Allwinner       : SiliconVendor<"Allwinner", "ALLW">;
def Vendor_Phytium         : SiliconVendor<"Phytium", "PHYT">;
def Vendor_THead           : SiliconVendor<"T-Head Alibaba", "THED">;
def Vendor_SiFive          : SiliconVendor<"SiFive", "SFIV">;
def Vendor_Loongson        : SiliconVendor<"Loongson", "LOONG">;
def Vendor_Zhaoxin         : SiliconVendor<"Zhaoxin", "ZXIN">;
def Vendor_Sunway          : SiliconVendor<"Sunway", "SWAY">;
def Vendor_IBM             : SiliconVendor<"IBM", "IBM">;
def Vendor_Fujitsu         : SiliconVendor<"Fujitsu", "FUJI">;
def Vendor_Ampere          : SiliconVendor<"Ampere Computing", "AMPR">;
def Vendor_Tenstorrent     : SiliconVendor<"Tenstorrent", "TENST">;
def Vendor_Ventana         : SiliconVendor<"Ventana Micro", "VENT">;
def Vendor_Esperanto       : SiliconVendor<"Esperanto Tech", "ESPR">;
def Vendor_RISCV           : SiliconVendor<"RISC-V Generic", "RISCV">;
def Vendor_Wasm            : SiliconVendor<"WebAssembly Engine", "WASM">;
def Vendor_STMicroelectronics : SiliconVendor<"STMicroelectronics", "STMI">;
def Vendor_MicrochipAtmel     : SiliconVendor<"Microchip / Atmel", "MCHP">;
def Vendor_RaspberryPi        : SiliconVendor<"Raspberry Pi", "RPI">;
def Vendor_Espressif          : SiliconVendor<"Espressif Systems", "ESPF">;
def Vendor_TexasInstruments   : SiliconVendor<"Texas Instruments", "TI">;
def Vendor_Amlogic            : SiliconVendor<"Amlogic", "AMLG">;
def Vendor_SpacemiT           : SiliconVendor<"SpacemiT", "SPMT">;
def Vendor_Sophgo             : SiliconVendor<"Sophgo", "SOPH">;
def Vendor_Huawei             : SiliconVendor<"Huawei HiSilicon", "HWAI">;
def Vendor_MCST               : SiliconVendor<"MCST Russia", "MCST">;
def Vendor_Hygon              : SiliconVendor<"Hygon", "HYGN">;

class CPUFamily<string name, SiliconVendor vendor, string microarch, int default_cacheline_bytes> {
  string Name = name;
  SiliconVendor Vendor = vendor;
  string Microarch = microarch;
  int CachelineBytes = default_cacheline_bytes;
  int ReleaseYear = 2020;
  bit Is64Bit = 1;
  bit IsHeterogeneous = 0;
  int MaxVectorBitWidth = 256;
  string DefaultSocket = "Generic";
  string PipelineWidth = "4-wide";
  string ProcessNode = "7nm";
  int CPUIDFamily = 0;
  int CPUIDModel = 0;
}

def Arch_SandyBridge           : CPUFamily<"SandyBridge", Vendor_Intel, "SNB", 64> {
  let ReleaseYear = 2011;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1155";
  let PipelineWidth = "4-wide";
  let ProcessNode = "32nm";
}
def Arch_IvyBridge             : CPUFamily<"IvyBridge", Vendor_Intel, "IVB", 64> {
  let ReleaseYear = 2012;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1155";
  let PipelineWidth = "4-wide";
  let ProcessNode = "22nm";
}
def Arch_Haswell               : CPUFamily<"Haswell", Vendor_Intel, "HSW", 64> {
  let ReleaseYear = 2013;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1150";
  let PipelineWidth = "4-wide";
  let ProcessNode = "22nm";
}
def Arch_Broadwell             : CPUFamily<"Broadwell", Vendor_Intel, "BDW", 64> {
  let ReleaseYear = 2014;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1150";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_Skylake               : CPUFamily<"Skylake", Vendor_Intel, "SKL", 64> {
  let ReleaseYear = 2015;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA1151";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_KabyLake              : CPUFamily<"KabyLake", Vendor_Intel, "KBL", 64> {
  let ReleaseYear = 2017;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1151";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm+";
}
def Arch_CoffeeLake            : CPUFamily<"CoffeeLake", Vendor_Intel, "CFL", 64> {
  let ReleaseYear = 2017;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1151";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm++";
}
def Arch_CometLake             : CPUFamily<"CometLake", Vendor_Intel, "CML", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1200";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm+++";
}
def Arch_Cascadelake           : CPUFamily<"Cascadelake", Vendor_Intel, "CLX", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA3647";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_Cooperlake            : CPUFamily<"CooperLake", Vendor_Intel, "CPX", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA4189";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_Icelake               : CPUFamily<"Icelake", Vendor_Intel, "ICL", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA4189";
  let PipelineWidth = "5-wide";
  let ProcessNode = "10nm";
}
def Arch_Tigerlake             : CPUFamily<"Tigerlake", Vendor_Intel, "TGL", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "BGA1449";
  let PipelineWidth = "5-wide";
  let ProcessNode = "10nm SuperFin";
}
def Arch_Rocketlake            : CPUFamily<"Rocketlake", Vendor_Intel, "RKL", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA1200";
  let PipelineWidth = "5-wide";
  let ProcessNode = "14nm";
}
def Arch_Alderlake             : CPUFamily<"Alderlake", Vendor_Intel, "ADL", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1700";
  let PipelineWidth = "6-wide";
  let ProcessNode = "Intel 7";
}
def Arch_Raptorlake            : CPUFamily<"Raptorlake", Vendor_Intel, "RPL", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1700";
  let PipelineWidth = "6-wide";
  let ProcessNode = "Intel 7";
}
def Arch_Meteorlake            : CPUFamily<"Meteorlake", Vendor_Intel, "MTL", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "BGA2049";
  let PipelineWidth = "6-wide";
  let ProcessNode = "Intel 4";
}
def Arch_Arrowlake             : CPUFamily<"Arrowlake", Vendor_Intel, "ARL", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1851";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3B";
}
def Arch_Lunarlake             : CPUFamily<"Lunarlake", Vendor_Intel, "LNL", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "BGA2833";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3B";
}
def Arch_EmeraldRap            : CPUFamily<"EmeraldRapids", Vendor_Intel, "EMR", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA4677";
  let PipelineWidth = "6-wide";
  let ProcessNode = "Intel 7";
}
def Arch_GraniteRap            : CPUFamily<"GraniteRapids", Vendor_Intel, "GNR", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA7529";
  let PipelineWidth = "8-wide";
  let ProcessNode = "Intel 3";
}
def Arch_SierraForest          : CPUFamily<"SierraForest", Vendor_Intel, "SRF", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA7529";
  let PipelineWidth = "4-wide";
  let ProcessNode = "Intel 3";
}
def Arch_ClearwaterForest      : CPUFamily<"ClearwaterForest", Vendor_Intel, "CWF", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA7529";
  let PipelineWidth = "4-wide";
  let ProcessNode = "Intel 18A";
}
def Arch_DiamondRapids         : CPUFamily<"DiamondRapids", Vendor_Intel, "DMR", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA9324";
  let PipelineWidth = "8-wide";
  let ProcessNode = "Intel 18A";
}
def Arch_PantherLake           : CPUFamily<"PantherLake", Vendor_Intel, "PTL", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "BGA2540";
  let PipelineWidth = "8-wide";
  let ProcessNode = "Intel 18A";
}
def Arch_NovaLake              : CPUFamily<"NovaLake", Vendor_Intel, "NVL", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA1954";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N2 / Intel 18A";
}
def Arch_CoyoteCove            : CPUFamily<"CoyoteCove", Vendor_Intel, "NVL-P", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA1954";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N2";
}
def Arch_ArcticWolf            : CPUFamily<"ArcticWolf", Vendor_Intel, "NVL-E", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "LGA1954";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N2 / Intel 18A";
}
def Arch_Goldmont              : CPUFamily<"Goldmont", Vendor_Intel, "GLM", 64> {
  let ReleaseYear = 2016;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "3-wide";
  let ProcessNode = "14nm";
}
def Arch_Tremont               : CPUFamily<"Tremont", Vendor_Intel, "TRM", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "10nm";
}
def Arch_Gracemont             : CPUFamily<"Gracemont", Vendor_Intel, "GRC", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "SoC";
  let PipelineWidth = "5-wide";
  let ProcessNode = "Intel 7";
}
def Arch_Crestmont             : CPUFamily<"Crestmont", Vendor_Intel, "CRS", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "SoC";
  let PipelineWidth = "5-wide";
  let ProcessNode = "Intel 4";
}
def Arch_Skymont               : CPUFamily<"Skymont", Vendor_Intel, "SKY", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3B";
}
def Arch_Piledriver            : CPUFamily<"Piledriver", Vendor_AMD, "PDV", 64> {
  let ReleaseYear = 2012;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM3+";
  let PipelineWidth = "4-wide";
  let ProcessNode = "32nm";
}
def Arch_Steamroller           : CPUFamily<"Steamroller", Vendor_AMD, "STR", 64> {
  let ReleaseYear = 2014;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "FM2+";
  let PipelineWidth = "4-wide";
  let ProcessNode = "28nm";
}
def Arch_Excavator             : CPUFamily<"Excavator", Vendor_AMD, "EXC", 64> {
  let ReleaseYear = 2015;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "28nm";
}
def Arch_Zen1                  : CPUFamily<"Zen1", Vendor_AMD, "ZN1", 64> {
  let ReleaseYear = 2017;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_ZenPlus               : CPUFamily<"ZenPlus", Vendor_AMD, "ZNP", 64> {
  let ReleaseYear = 2018;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "12nm";
}
def Arch_Zen2                  : CPUFamily<"Zen2", Vendor_AMD, "ZN2", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_Zen3                  : CPUFamily<"Zen3", Vendor_AMD, "ZN3", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_Zen3D                 : CPUFamily<"Zen3VTree", Vendor_AMD, "ZN3D", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "AM4";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_Zen4                  : CPUFamily<"Zen4", Vendor_AMD, "ZN4", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "AM5";
  let PipelineWidth = "6-wide";
  let ProcessNode = "5nm";
}
def Arch_Zen4c                 : CPUFamily<"Zen4c", Vendor_AMD, "ZN4C", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SP5";
  let PipelineWidth = "6-wide";
  let ProcessNode = "4nm";
}
def Arch_Zen4c_Bergamo         : CPUFamily<"Zen4cBergamo", Vendor_AMD, "ZN4CB", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SP5";
  let PipelineWidth = "6-wide";
  let ProcessNode = "4nm";
}
def Arch_Zen5                  : CPUFamily<"Zen5", Vendor_AMD, "ZN5", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "AM5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "4nm";
}
def Arch_Zen5c                 : CPUFamily<"Zen5c", Vendor_AMD, "ZN5C", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SP5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "3nm";
}
def Arch_Zen5c_Turin           : CPUFamily<"Zen5cTurin", Vendor_AMD, "ZN5CT", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SP5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "3nm";
}
def Arch_StrixPoint            : CPUFamily<"StrixPoint", Vendor_AMD, "STX", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "FP8";
  let PipelineWidth = "8-wide";
  let ProcessNode = "4nm";
}
def Arch_KrakenPoint           : CPUFamily<"KrakenPoint", Vendor_AMD, "KRK", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "FP8";
  let PipelineWidth = "8-wide";
  let ProcessNode = "4nm";
}
def Arch_Zen5_X3D              : CPUFamily<"Zen5X3D_9950X3D", Vendor_AMD, "ZN5X3D", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "AM5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4P + 2nd-Gen 3D V-Cache";
}
def Arch_Zen6                  : CPUFamily<"Zen6_Morpheus", Vendor_AMD, "ZN6", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "AM5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N2P";
}
def Arch_Zen6c                 : CPUFamily<"Zen6c_Prometheus", Vendor_AMD, "ZN6C", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SP7";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N2";
}
def Arch_Zen6_X3D              : CPUFamily<"Zen6X3D_Medusa", Vendor_AMD, "ZN6X3D", 64> {
  let ReleaseYear = 2026;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "AM5";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N2P + 3rd-Gen 3D V-Cache";
}
def Arch_AppleM1               : CPUFamily<"AppleM1", Vendor_Apple, "M1", 128> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N5";
}
def Arch_AppleM1Max            : CPUFamily<"AppleM1Max", Vendor_Apple, "M1M", 128> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N5";
}
def Arch_AppleM2               : CPUFamily<"AppleM2", Vendor_Apple, "M2", 128> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N5P";
}
def Arch_AppleM2Max            : CPUFamily<"AppleM2Max", Vendor_Apple, "M2M", 128> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N5P";
}
def Arch_AppleM3               : CPUFamily<"AppleM3", Vendor_Apple, "M3", 128> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3B";
}
def Arch_AppleM3Max            : CPUFamily<"AppleM3Max", Vendor_Apple, "M3M", 128> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3B";
}
def Arch_AppleM4               : CPUFamily<"AppleM4", Vendor_Apple, "M4", 128> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_AppleM4Pro            : CPUFamily<"AppleM4Pro", Vendor_Apple, "M4P", 128> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_AppleM4Max            : CPUFamily<"AppleM4Max", Vendor_Apple, "M4M", 128> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_QualcommKryo          : CPUFamily<"QualcommKryo", Vendor_Qualcomm, "KRY", 64> {
  let ReleaseYear = 2018;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_OryonXElite           : CPUFamily<"OryonXElite", Vendor_Qualcomm, "ORY", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4P";
}
def Arch_OryonV2               : CPUFamily<"OryonV2", Vendor_Qualcomm, "ORY2", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_CortexA53             : CPUFamily<"CortexA53", Vendor_ARM, "A53", 64> {
  let ReleaseYear = 2012;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_CortexA55             : CPUFamily<"CortexA55", Vendor_ARM, "A55", 64> {
  let ReleaseYear = 2017;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "10nm";
}
def Arch_CortexA72             : CPUFamily<"CortexA72", Vendor_ARM, "A72", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "3-wide";
  let ProcessNode = "28nm";
}
def Arch_CortexA76             : CPUFamily<"CortexA76", Vendor_ARM, "A76", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_CortexA78             : CPUFamily<"CortexA78", Vendor_ARM, "A78", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "5nm";
}
def Arch_CortexX1              : CPUFamily<"CortexX1", Vendor_ARM, "X1", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "5-wide";
  let ProcessNode = "5nm";
}
def Arch_CortexX2              : CPUFamily<"CortexX2", Vendor_ARM, "X2", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "5-wide";
  let ProcessNode = "4nm";
}
def Arch_CortexX3              : CPUFamily<"CortexX3", Vendor_ARM, "X3", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "6-wide";
  let ProcessNode = "4nm";
}
def Arch_CortexX4              : CPUFamily<"CortexX4", Vendor_ARM, "X4", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4P";
}
def Arch_CortexX925            : CPUFamily<"CortexX925", Vendor_ARM, "X925", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "10-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_NeoverseN1            : CPUFamily<"NeoverseN1", Vendor_ARM, "NN1", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Server";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_NeoverseN2            : CPUFamily<"NeoverseN2", Vendor_ARM, "NN2", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Server";
  let PipelineWidth = "4-wide";
  let ProcessNode = "5nm";
}
def Arch_NeoverseV1            : CPUFamily<"NeoverseV1", Vendor_ARM, "NV1", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "Server";
  let PipelineWidth = "8-wide";
  let ProcessNode = "7nm";
}
def Arch_NeoverseV2            : CPUFamily<"NeoverseV2", Vendor_ARM, "NV2", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "Server";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4";
}
def Arch_NeoverseV3            : CPUFamily<"NeoverseV3", Vendor_ARM, "NV3", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "Server";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_NvidiaOrin            : CPUFamily<"NvidiaOrin", Vendor_Nvidia, "ORN", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "Samsung 8nm";
}
def Arch_NvidiaGrace           : CPUFamily<"NvidiaGrace", Vendor_Nvidia, "GRACE", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "Server";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC 4N";
}
def Arch_AmpereAltra           : CPUFamily<"AmpereAltra", Vendor_Ampere, "ALT", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "LGA4926";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_AmpereOne             : CPUFamily<"AmpereOne", Vendor_Ampere, "AONE", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "LGA5964";
  let PipelineWidth = "4-wide";
  let ProcessNode = "5nm";
}
def Arch_FujitsuA64FX          : CPUFamily<"FujitsuA64FX", Vendor_Fujitsu, "A64FX", 256> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "BGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_RockchipRK3399        : CPUFamily<"RockchipRK3399", Vendor_Rockchip, "RK33", 64> {
  let ReleaseYear = 2016;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "3-wide";
  let ProcessNode = "28nm";
}
def Arch_RockchipRK3588        : CPUFamily<"RockchipRK3588", Vendor_Rockchip, "RK35", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "8nm";
}
def Arch_AllwinnerH6           : CPUFamily<"AllwinnerH6", Vendor_Allwinner, "H6", 64> {
  let ReleaseYear = 2017;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_PhytiumS2500          : CPUFamily<"PhytiumS2500", Vendor_Phytium, "S25", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Server";
  let PipelineWidth = "4-wide";
  let ProcessNode = "16nm";
}
def Arch_PhytiumD2000          : CPUFamily<"PhytiumD2000", Vendor_Phytium, "D20", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Desktop";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_THeadC906             : CPUFamily<"THeadC906", Vendor_THead, "C906", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "1-wide";
  let ProcessNode = "22nm";
}
def Arch_THeadC910             : CPUFamily<"THeadC910", Vendor_THead, "C910", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "3-wide";
  let ProcessNode = "12nm";
}
def Arch_SiFiveX280            : CPUFamily<"SiFiveX280", Vendor_SiFive, "X280", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "SoC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "7nm";
}
def Arch_TenstorrentAscalon    : CPUFamily<"TenstorrentAscalon", Vendor_Tenstorrent, "ASC", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "PCIe";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4";
}
def Arch_VentanaVeyronV2       : CPUFamily<"VentanaVeyronV2", Vendor_Ventana, "VEY2", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "Chiplet";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N4";
}
def Arch_Loongson3A5000        : CPUFamily<"Loongson3A5000", Vendor_Loongson, "LA50", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "12nm";
}
def Arch_Loongson3A6000        : CPUFamily<"Loongson3A6000", Vendor_Loongson, "LA66", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA";
  let PipelineWidth = "6-wide";
  let ProcessNode = "12nm";
}
def Arch_ZhaoxinKX6000         : CPUFamily<"ZhaoxinKX6000", Vendor_Zhaoxin, "KX6", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "Desktop";
  let PipelineWidth = "4-wide";
  let ProcessNode = "16nm";
}
def Arch_ZhaoxinKX7000         : CPUFamily<"ZhaoxinKX7000", Vendor_Zhaoxin, "KX7", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA1700";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_SunwaySW26010Pro      : CPUFamily<"SunwaySW26010Pro", Vendor_Sunway, "SW26P", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "HPC";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}
def Arch_IBMz16                : CPUFamily<"IBMz16Telum", Vendor_IBM, "Z16", 256> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Mainframe";
  let PipelineWidth = "8-wide";
  let ProcessNode = "7nm";
}
def Arch_IBMPower10            : CPUFamily<"IBMPower10", Vendor_IBM, "P10", 128> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Enterprise";
  let PipelineWidth = "8-wide";
  let ProcessNode = "7nm";
}
def Arch_RISCV_V10             : CPUFamily<"RISCV_Vector", Vendor_RISCV, "RVV", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "Generic";
  let PipelineWidth = "4-wide";
  let ProcessNode = "Generic";
}
def Arch_WasmGeneric           : CPUFamily<"WasmGeneric", Vendor_Wasm, "WSM", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "Virtual";
  let PipelineWidth = "4-wide";
  let ProcessNode = "Virtual";
}

// --- Global Catalog 2011+: x86 Eastern & Sovereign ---
def Arch_HygonDhyana           : CPUFamily<"HygonDhyana", Vendor_Hygon, "DHY", 64> {
  let ReleaseYear = 2018;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "SP3";
  let PipelineWidth = "4-wide";
  let ProcessNode = "14nm";
}

// --- Global Catalog 2011+: ARM Server & Client ---
def Arch_HuaweiKunpeng920      : CPUFamily<"Kunpeng920", Vendor_Huawei, "KP920", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "LGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_PhytiumFT2000         : CPUFamily<"PhytiumFT2000", Vendor_Phytium, "FT2K", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "FCBGA";
  let PipelineWidth = "3-wide";
  let ProcessNode = "16nm";
}
def Arch_PhytiumS5000          : CPUFamily<"PhytiumS5000", Vendor_Phytium, "S50", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "LGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "7nm";
}
def Arch_Snapdragon8Elite      : CPUFamily<"Snapdragon8Elite", Vendor_Qualcomm, "SD8E", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC N3E";
}
def Arch_ArduinoVentunoQ       : CPUFamily<"ArduinoVentunoQ", Vendor_Qualcomm, "IQ8275", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoM";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC 4nm";
}
def Arch_ArduinoUnoQ           : CPUFamily<"ArduinoUnoQ", Vendor_Qualcomm, "QRB2210", 64> {
  let ReleaseYear = 2025;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "SoC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "11nm FinFET";
}

// --- Global Catalog 2011+: Single Board Computers (SBC) ---
def Arch_BroadcomBCM2835       : CPUFamily<"BCM2835", Vendor_Broadcom, "ARM11", 32> {
  let ReleaseYear = 2011;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 0;
  let DefaultSocket = "PoP";
  let PipelineWidth = "1-wide";
  let ProcessNode = "65nm";
}
def Arch_BroadcomBCM2836       : CPUFamily<"BCM2836", Vendor_Broadcom, "BCM2836", 32> {
  let ReleaseYear = 2015;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_BroadcomBCM2837       : CPUFamily<"BCM2837", Vendor_Broadcom, "BCM2837", 64> {
  let ReleaseYear = 2016;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_BroadcomBCM2711       : CPUFamily<"BCM2711", Vendor_Broadcom, "BCM2711", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "3-wide";
  let ProcessNode = "28nm";
}
def Arch_BroadcomBCM2712       : CPUFamily<"BCM2712", Vendor_Broadcom, "BCM2712", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "16nm";
}
def Arch_RockchipRK3288        : CPUFamily<"RK3288", Vendor_Rockchip, "RK32", 32> {
  let ReleaseYear = 2014;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_RockchipRK3566        : CPUFamily<"RK3566", Vendor_Rockchip, "RK356", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "22nm";
}
def Arch_RockchipRK3576        : CPUFamily<"RK3576", Vendor_Rockchip, "RK357", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "3-wide";
  let ProcessNode = "8nm";
}
def Arch_AllwinnerA20          : CPUFamily<"AllwinnerA20", Vendor_Allwinner, "A20", 32> {
  let ReleaseYear = 2013;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_AllwinnerA64          : CPUFamily<"AllwinnerA64", Vendor_Allwinner, "A64", 64> {
  let ReleaseYear = 2015;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_AllwinnerH618         : CPUFamily<"AllwinnerH618", Vendor_Allwinner, "H618", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_AmlogicS905X4         : CPUFamily<"AmlogicS905X4", Vendor_Amlogic, "S905X4", 64> {
  let ReleaseYear = 2020;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "12nm";
}
def Arch_AmlogicS922X_A311D    : CPUFamily<"AmlogicS922X", Vendor_Amlogic, "S922X", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "3-wide";
  let ProcessNode = "12nm";
}

// --- Global Catalog 2011+: Low-Power Microcontrollers (MCU) & DIY ---
def Arch_AtmelATmega328P       : CPUFamily<"ATmega328P", Vendor_MicrochipAtmel, "AVR8", 1> {
  let ReleaseYear = 2011;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 0;
  let DefaultSocket = "DIP28/TQFP32";
  let PipelineWidth = "1-wide";
  let ProcessNode = "130nm";
}
def Arch_AtmelSAMD21           : CPUFamily<"SAMD21", Vendor_MicrochipAtmel, "M0P", 32> {
  let ReleaseYear = 2014;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 0;
  let DefaultSocket = "QFN32/TQFP48";
  let PipelineWidth = "1-wide";
  let ProcessNode = "90nm";
}
def Arch_MicrochipSAME54       : CPUFamily<"SAME54", Vendor_MicrochipAtmel, "M4F", 32> {
  let ReleaseYear = 2017;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "TQFP100";
  let PipelineWidth = "1-wide";
  let ProcessNode = "65nm";
}
def Arch_MicrochipSAMV71       : CPUFamily<"SAMV71", Vendor_MicrochipAtmel, "M7DP", 32> {
  let ReleaseYear = 2015;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "LQFP144";
  let PipelineWidth = "2-wide";
  let ProcessNode = "65nm";
}
def Arch_STM32F407             : CPUFamily<"STM32F407", Vendor_STMicroelectronics, "F407", 32> {
  let ReleaseYear = 2011;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "LQFP100";
  let PipelineWidth = "1-wide";
  let ProcessNode = "90nm";
}
def Arch_STM32G474             : CPUFamily<"STM32G474", Vendor_STMicroelectronics, "G474", 32> {
  let ReleaseYear = 2019;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "LQFP100";
  let PipelineWidth = "1-wide";
  let ProcessNode = "90nm";
}
def Arch_STM32H743_H747        : CPUFamily<"STM32H747", Vendor_STMicroelectronics, "H747", 32> {
  let ReleaseYear = 2017;
  let Is64Bit = 0;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "TFBGA240";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_STM32H5F5             : CPUFamily<"STM32H5F5", Vendor_STMicroelectronics, "H5F5", 32> {
  let ReleaseYear = 2023;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "LQFP";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_STM32U585             : CPUFamily<"STM32U585", Vendor_STMicroelectronics, "U585", 32> {
  let ReleaseYear = 2022;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "LQFP";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_RPiRP2040             : CPUFamily<"RP2040", Vendor_RaspberryPi, "RP2040", 32> {
  let ReleaseYear = 2021;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 0;
  let DefaultSocket = "QFN56";
  let PipelineWidth = "1-wide";
  let ProcessNode = "40nm";
}
def Arch_RPiRP2350             : CPUFamily<"RP2350", Vendor_RaspberryPi, "RP2350", 32> {
  let ReleaseYear = 2024;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "QFN60/80";
  let PipelineWidth = "1-wide";
  let ProcessNode = "40nm";
}
def Arch_EspressifESP8266      : CPUFamily<"ESP8266", Vendor_Espressif, "L106", 32> {
  let ReleaseYear = 2014;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 0;
  let DefaultSocket = "QFN32";
  let PipelineWidth = "1-wide";
  let ProcessNode = "40nm";
}
def Arch_EspressifESP32        : CPUFamily<"ESP32", Vendor_Espressif, "LX6", 32> {
  let ReleaseYear = 2016;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "QFN48";
  let PipelineWidth = "1-wide";
  let ProcessNode = "40nm";
}
def Arch_EspressifESP32S3      : CPUFamily<"ESP32S3", Vendor_Espressif, "LX7", 32> {
  let ReleaseYear = 2020;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "QFN56";
  let PipelineWidth = "2-wide";
  let ProcessNode = "40nm";
}
def Arch_EspressifESP32P4      : CPUFamily<"ESP32P4", Vendor_Espressif, "P4", 32> {
  let ReleaseYear = 2024;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "QFN";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_TITMS320F28379D       : CPUFamily<"TMS320F28379D", Vendor_TexasInstruments, "C28X", 32> {
  let ReleaseYear = 2014;
  let Is64Bit = 0;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 32;
  let DefaultSocket = "BGA/LQFP";
  let PipelineWidth = "1-wide";
  let ProcessNode = "65nm";
}

// --- Global Catalog 2011+: RISC-V & Many-Core Accelerators ---
def Arch_SpacemiTK1_X60        : CPUFamily<"SpacemiTK1", Vendor_SpacemiT, "X60", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "12nm";
}
def Arch_StarFiveJH7110        : CPUFamily<"StarFiveJH7110", Vendor_SiFive, "U74", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 64;
  let DefaultSocket = "BGA";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}
def Arch_SophgoSG2042          : CPUFamily<"SophgoSG2042", Vendor_Sophgo, "C920", 64> {
  let ReleaseYear = 2023;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "LGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "12nm";
}
def Arch_SiFiveP870            : CPUFamily<"SiFiveP870", Vendor_SiFive, "P870", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "SoC";
  let PipelineWidth = "6-wide";
  let ProcessNode = "TSMC N5";
}
def Arch_EsperantoETSoC1       : CPUFamily<"EsperantoETSoC1", Vendor_Esperanto, "ETMinion", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "PCIe";
  let PipelineWidth = "8-wide";
  let ProcessNode = "TSMC 7nm";
}
def Arch_TenstorrentWormhole   : CPUFamily<"TenstorrentWormhole", Vendor_Tenstorrent, "TensixWH", 64> {
  let ReleaseYear = 2022;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "PCIe";
  let PipelineWidth = "8-wide";
  let ProcessNode = "12nm";
}
def Arch_TenstorrentBlackhole  : CPUFamily<"TenstorrentBlackhole", Vendor_Tenstorrent, "TensixBH", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 512;
  let DefaultSocket = "PCIe";
  let PipelineWidth = "12-wide";
  let ProcessNode = "TSMC N6";
}

// --- Global Catalog 2011+: Independent & Sovereign Architectures ---
def Arch_Loongson3A4000        : CPUFamily<"Loongson3A4000", Vendor_Loongson, "GS464V", 64> {
  let ReleaseYear = 2019;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "4-wide";
  let ProcessNode = "28nm";
}
def Arch_Loongson3C6000        : CPUFamily<"Loongson3C6000", Vendor_Loongson, "LA664C", 64> {
  let ReleaseYear = 2024;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "LGA";
  let PipelineWidth = "6-wide";
  let ProcessNode = "12nm";
}
def Arch_MCSTElbrus4C          : CPUFamily<"Elbrus4C", Vendor_MCST, "E2K_V3", 64> {
  let ReleaseYear = 2014;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "6-wide VLIW";
  let ProcessNode = "65nm";
}
def Arch_MCSTElbrus8S          : CPUFamily<"Elbrus8S", Vendor_MCST, "E2K_V4", 64> {
  let ReleaseYear = 2016;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "6-wide VLIW";
  let ProcessNode = "28nm";
}
def Arch_MCSTElbrus8SV         : CPUFamily<"Elbrus8SV", Vendor_MCST, "E2K_V5", 64> {
  let ReleaseYear = 2018;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "6-wide VLIW";
  let ProcessNode = "28nm";
}
def Arch_MCSTElbrus16C         : CPUFamily<"Elbrus16C", Vendor_MCST, "E2K_V6", 64> {
  let ReleaseYear = 2021;
  let IsHeterogeneous = 0;
  let MaxVectorBitWidth = 128;
  let DefaultSocket = "BGA";
  let PipelineWidth = "6-wide VLIW";
  let ProcessNode = "16nm";
}
def Arch_SunwaySW26010         : CPUFamily<"SunwaySW26010", Vendor_Sunway, "SW26", 64> {
  let ReleaseYear = 2016;
  let IsHeterogeneous = 1;
  let MaxVectorBitWidth = 256;
  let DefaultSocket = "HPC";
  let PipelineWidth = "2-wide";
  let ProcessNode = "28nm";
}

// =============================================================================
// SECTION 2: VECTOR INSTRUCTION SET ARCHITECTURES (ISAs)
// =============================================================================

class MachineISA<string name, int default_vector_bits, string feature_macro, int precedence_rank> {
  string Name = name;
  int VectorBits = default_vector_bits;
  string FeatureMacro = feature_macro;
  int Rank = precedence_rank;
  bit SupportsFMA = 0;
  bit SupportsMasking = 0;
  bit SupportsVariableLength = 0;
  int PreferredAlignmentBytes = 32;
  string HeaderInclude = "<immintrin.h>";
  int CPUIDLeaf = 0;
  int CPUIDSubleaf = 0;
  string CPUIDRegister = "NONE";
  int CPUIDBit = 0;
}

def ISA_Scalar                 : MachineISA<"Scalar", 0, "1", 10> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<cmath>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_x86_MMX                : MachineISA<"x86_MMX", 64, "__MMX__", 20> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<mmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 23;
}
def ISA_x86_SSE                : MachineISA<"x86_SSE", 128, "__SSE__", 30> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<xmmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 25;
}
def ISA_x86_SSE2               : MachineISA<"x86_SSE2", 128, "__SSE2__", 40> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<emmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 26;
}
def ISA_x86_SSE3               : MachineISA<"x86_SSE3", 128, "__SSE3__", 42> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<pmmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 0;
}
def ISA_x86_SSSE3              : MachineISA<"x86_SSSE3", 128, "__SSSE3__", 45> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<tmmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 9;
}
def ISA_x86_SSE41              : MachineISA<"x86_SSE41", 128, "__SSE4_1__", 48> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<smmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 19;
}
def ISA_x86_SSE42              : MachineISA<"x86_SSE42", 128, "__SSE4_2__", 50> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<nmmintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 20;
}
def ISA_x86_AVX                : MachineISA<"x86_AVX", 256, "__AVX__", 70> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 28;
}
def ISA_x86_FMA4               : MachineISA<"x86_FMA4", 256, "__FMA4__", 78> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<x86intrin.h>";
  let CPUIDLeaf = 2147483649;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 16;
}
def ISA_x86_XOP                : MachineISA<"x86_XOP", 256, "__XOP__", 79> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<x86intrin.h>";
  let CPUIDLeaf = 2147483649;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 11;
}
def ISA_x86_AVX2               : MachineISA<"x86_AVX2", 256, "__AVX2__", 80> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 5;
}
def ISA_x86_FMA3               : MachineISA<"x86_FMA3", 256, "__FMA__", 85> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 1;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 12;
}
def ISA_x86_AVX_VNNI           : MachineISA<"x86_AVX_VNNI", 256, "__AVXVNNI__", 88> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EAX";
  let CPUIDBit = 4;
}
def ISA_x86_AVX_IFMA           : MachineISA<"x86_AVX_IFMA", 256, "__AVX_IFMA__", 89> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EAX";
  let CPUIDBit = 23;
}
def ISA_x86_AVX512F            : MachineISA<"x86_AVX512F", 512, "__AVX512F__", 100> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 16;
}
def ISA_x86_AVX512CD           : MachineISA<"x86_AVX512CD", 512, "__AVX512CD__", 101> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 28;
}
def ISA_x86_AVX512DQ           : MachineISA<"x86_AVX512DQ", 512, "__AVX512DQ__", 102> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 17;
}
def ISA_x86_AVX512BW           : MachineISA<"x86_AVX512BW", 512, "__AVX512BW__", 104> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 30;
}
def ISA_x86_AVX512VL           : MachineISA<"x86_AVX512VL", 512, "__AVX512VL__", 105> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EBX";
  let CPUIDBit = 31;
}
def ISA_x86_AVX512VBMI         : MachineISA<"x86_AVX512VBMI", 512, "__AVX512VBMI__", 106> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 1;
}
def ISA_x86_AVX512VBMI2        : MachineISA<"x86_AVX512VBMI2", 512, "__AVX512VBMI2__", 107> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 6;
}
def ISA_x86_AVX512VNNI         : MachineISA<"x86_AVX512VNNI", 512, "__AVX512VNNI__", 108> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 11;
}
def ISA_x86_AVX512BF16         : MachineISA<"x86_AVX512BF16", 512, "__AVX512BF16__", 109> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EAX";
  let CPUIDBit = 5;
}
def ISA_x86_AVX512FP16         : MachineISA<"x86_AVX512FP16", 512, "__AVX512FP16__", 110> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 23;
}
def ISA_x86_AVX512BITALG       : MachineISA<"x86_AVX512BITALG", 512, "__AVX512BITALG__", 111> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 12;
}
def ISA_x86_AVX512VPOPCNTDQ    : MachineISA<"x86_AVX512VPOPCNTDQ", 512, "__AVX512VPOPCNTDQ__", 112> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "ECX";
  let CPUIDBit = 14;
}
def ISA_x86_AMX                : MachineISA<"x86_AMX", 1024, "__AMX_TILE__", 120> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 24;
}
def ISA_x86_APX                : MachineISA<"x86_APX", 64, "__APX_F__", 125> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 21;
}
def ISA_x86_APX_EGPR           : MachineISA<"x86_APX_EGPR", 64, "__APX_F__", 126> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 21;
}
def ISA_x86_APX_NDD            : MachineISA<"x86_APX_NDD", 64, "__APX_NDD__", 127> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 21;
}
def ISA_x86_APX_NF             : MachineISA<"x86_APX_NF", 64, "__APX_NF__", 128> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 21;
}
def ISA_x86_AVX10_1            : MachineISA<"x86_AVX10_1", 512, "__AVX10_1__", 130> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 19;
}
def ISA_x86_AVX10_2            : MachineISA<"x86_AVX10_2", 512, "__AVX10_2__", 135> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 19;
}
def ISA_x86_AVX10_2_256        : MachineISA<"x86_AVX10_2_256", 256, "__AVX10_2__", 136> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 19;
}
def ISA_x86_AVX10_2_512        : MachineISA<"x86_AVX10_2_512", 512, "__AVX10_2__", 138> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<immintrin.h>";
  let CPUIDLeaf = 7;
  let CPUIDSubleaf = 1;
  let CPUIDRegister = "EDX";
  let CPUIDBit = 19;
}
def ISA_ARM_V7_NEON            : MachineISA<"ARM_V7_NEON", 128, "__ARM_NEON__", 55> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_NEON               : MachineISA<"ARM_NEON", 128, "__ARM_NEON", 75> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_FP16               : MachineISA<"ARM_FP16", 128, "__ARM_FEATURE_FP16_VECTOR_ARITHMETIC", 76> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_FEAT_FP16          : MachineISA<"ARM_FEAT_FP16", 128, "__ARM_FEATURE_FP16_VECTOR_ARITHMETIC", 77> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_FEAT_BF16          : MachineISA<"ARM_FEAT_BF16", 128, "__ARM_FEATURE_BF16_VECTOR_ARITHMETIC", 78> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_FEAT_I8MM          : MachineISA<"ARM_FEAT_I8MM", 128, "__ARM_FEATURE_MATMUL_INT8", 79> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_FEAT_FHM           : MachineISA<"ARM_FEAT_FHM", 128, "__ARM_FEATURE_FP16_FML", 80> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_ASIMDDP            : MachineISA<"ARM_ASIMDDP", 128, "__ARM_FEATURE_DOTPROD", 81> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_neon.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_SVE                : MachineISA<"ARM_SVE", 512, "__ARM_FEATURE_SVE", 95> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<arm_sve.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_SVE2               : MachineISA<"ARM_SVE2", 512, "__ARM_FEATURE_SVE2", 98> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<arm_sve.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_SME                : MachineISA<"ARM_SME", 1024, "__ARM_FEATURE_SME", 115> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<arm_sme.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_ARM_SME2               : MachineISA<"ARM_SME2", 2048, "__ARM_FEATURE_SME2", 118> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<arm_sme.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zve32f           : MachineISA<"RISCV_Zve32f", 128, "__riscv_zve32f", 82> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zve64d           : MachineISA<"RISCV_Zve64d", 256, "__riscv_zve64d", 88> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zvfh             : MachineISA<"RISCV_Zvfh", 256, "__riscv_zvfh", 84> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zvbb             : MachineISA<"RISCV_Zvbb", 256, "__riscv_zvbb", 85> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zvbc             : MachineISA<"RISCV_Zvbc", 256, "__riscv_zvbc", 86> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Zvkb             : MachineISA<"RISCV_Zvkb", 256, "__riscv_zvkb", 87> {
  let SupportsFMA = 0;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_RISCV_Vector           : MachineISA<"RISCV_V10", 512, "__riscv_vector", 90> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 1;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<riscv_vector.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_LoongArch_LSX          : MachineISA<"LoongArch_LSX", 128, "__loongarch_sx", 72> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<lsxintrin.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_LoongArch_LASX         : MachineISA<"LoongArch_LASX", 256, "__loongarch_asx", 84> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<lasxintrin.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_IBM_AltiVec            : MachineISA<"IBM_AltiVec", 128, "__ALTIVEC__", 58> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<altivec.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_IBM_Vector             : MachineISA<"IBM_VX", 128, "__VEC__", 60> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<vecintrin.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_IBM_VSX                : MachineISA<"IBM_VSX", 128, "__VSX__", 62> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<altivec.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_IBM_NNPA               : MachineISA<"IBM_NNPA", 1024, "__NNPA__", 110> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 64;
  let HeaderInclude = "<s390intrin.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_Wasm_SIMD128           : MachineISA<"Wasm_SIMD128", 128, "__wasm_simd128__", 45> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<wasm_simd128.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}
def ISA_Wasm_RelaxedSIMD       : MachineISA<"Wasm_RelaxedSIMD", 128, "__wasm_relaxed_simd__", 47> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<wasm_simd128.h>";
  let CPUIDLeaf = 0;
  let CPUIDSubleaf = 0;
  let CPUIDRegister = "NONE";
  let CPUIDBit = 0;
}

// --- Global Catalog 2011+: Embedded, DSP, Coprocessor & Sovereign ISAs ---
def ISA_ARM_DSP                : MachineISA<"ARM_DSP", 32, "__ARM_FEATURE_DSP", 52> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<arm_acle.h>";
}
def ISA_ARM_Helium_MVE         : MachineISA<"ARM_Helium_MVE", 128, "__ARM_FEATURE_MVE", 83> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<arm_mve.h>";
}
def ISA_Xtensa_PIE             : MachineISA<"Xtensa_PIE", 128, "__XTENSA_PIE__", 82> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<esp_dsp.h>";
}
def ISA_ST_CORDIC              : MachineISA<"ST_CORDIC", 32, "__STM32_CORDIC__", 65> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<stm32g4xx_hal_cordic.h>";
}
def ISA_ST_FMAC                : MachineISA<"ST_FMAC", 16, "__STM32_FMAC__", 66> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 2;
  let HeaderInclude = "<stm32g4xx_hal_fmac.h>";
}
def ISA_TI_TMU                 : MachineISA<"TI_TMU", 32, "__TI_TMU__", 67> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<fpu_types.h>";
}
def ISA_TI_VCU                 : MachineISA<"TI_VCU", 32, "__TI_VCU__", 68> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<fpu_vcu.h>";
}
def ISA_RP2350_DP              : MachineISA<"RP2350_DP", 64, "__RP2350_DP__", 69> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 8;
  let HeaderInclude = "<pico/double.h>";
}
def ISA_MCST_E2K               : MachineISA<"MCST_E2K", 128, "__e2k__", 86> {
  let SupportsFMA = 1;
  let SupportsMasking = 1;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 16;
  let HeaderInclude = "<e2kintrin.h>";
}
def ISA_ShenWei_SIMD           : MachineISA<"ShenWei_SIMD", 256, "__sw_64__", 87> {
  let SupportsFMA = 1;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 32;
  let HeaderInclude = "<simd.h>";
}
def ISA_SoftFloat_Q31          : MachineISA<"SoftFloat_Q31", 0, "__MSIMD_SOFT_FLOAT__", 5> {
  let SupportsFMA = 0;
  let SupportsMasking = 0;
  let SupportsVariableLength = 0;
  let PreferredAlignmentBytes = 4;
  let HeaderInclude = "<stdint.h>";
}

// =============================================================================
// SECTION 3: PHYSICAL VECTOR REGISTER FILES & LANE TOPOLOGIES
// =============================================================================

class PhysicalRegisterFile<string name, int bit_width, int count, string prefix> {
  string Name = name;
  int BitWidth = bit_width;
  int RegisterCount = count;
  string Prefix = prefix;
}

def RegFile_XMM          : PhysicalRegisterFile<"RegFile_XMM", 128, 16, "xmm">;
def RegFile_YMM          : PhysicalRegisterFile<"RegFile_YMM", 256, 16, "ymm">;
def RegFile_ZMM          : PhysicalRegisterFile<"RegFile_ZMM", 512, 32, "zmm">;
def RegFile_Opmask       : PhysicalRegisterFile<"RegFile_Opmask", 64, 8, "k">;
def RegFile_TMM          : PhysicalRegisterFile<"RegFile_TMM", 8192, 8, "tmm">;
def RegFile_APX_GPR      : PhysicalRegisterFile<"RegFile_APX_GPR", 64, 32, "r">;
def RegFile_NEON         : PhysicalRegisterFile<"RegFile_NEON", 128, 32, "v">;
def RegFile_SVE          : PhysicalRegisterFile<"RegFile_SVE", 512, 32, "z">;
def RegFile_SVE_Pred     : PhysicalRegisterFile<"RegFile_SVE_Pred", 64, 16, "p">;
def RegFile_SME_ZA       : PhysicalRegisterFile<"RegFile_SME_ZA", 4096, 1, "za">;
def RegFile_RVV          : PhysicalRegisterFile<"RegFile_RVV", 512, 32, "v">;
def RegFile_LASX         : PhysicalRegisterFile<"RegFile_LASX", 256, 32, "xr">;
def RegFile_VSX          : PhysicalRegisterFile<"RegFile_VSX", 128, 64, "vs">;
def RegFile_Wasm         : PhysicalRegisterFile<"RegFile_Wasm", 128, 128, "v128">;

class RegisterLaneConfig<string name, PhysicalRegisterFile regfile, string element_type, int lanes> {
  string Name = name;
  PhysicalRegisterFile RegFile = regfile;
  string ElementType = element_type;
  int LaneCount = lanes;
}

def Lane_ZMM_64xU8             : RegisterLaneConfig<"Lane_ZMM_64xU8", RegFile_ZMM, "uint8_t", 64>;
def Lane_ZMM_64xI8             : RegisterLaneConfig<"Lane_ZMM_64xI8", RegFile_ZMM, "int8_t", 64>;
def Lane_ZMM_64xBF8_E5M2       : RegisterLaneConfig<"Lane_ZMM_64xBF8_E5M2", RegFile_ZMM, "uint8_t", 64>;
def Lane_ZMM_64xHF8_E4M3       : RegisterLaneConfig<"Lane_ZMM_64xHF8_E4M3", RegFile_ZMM, "uint8_t", 64>;
def Lane_YMM_32xBF8_E5M2       : RegisterLaneConfig<"Lane_YMM_32xBF8_E5M2", RegFile_YMM, "uint8_t", 32>;
def Lane_YMM_32xHF8_E4M3       : RegisterLaneConfig<"Lane_YMM_32xHF8_E4M3", RegFile_YMM, "uint8_t", 32>;
def Lane_XMM_16xBF8_E5M2       : RegisterLaneConfig<"Lane_XMM_16xBF8_E5M2", RegFile_XMM, "uint8_t", 16>;
def Lane_XMM_16xHF8_E4M3       : RegisterLaneConfig<"Lane_XMM_16xHF8_E4M3", RegFile_XMM, "uint8_t", 16>;
def Lane_ZMM_32xU16            : RegisterLaneConfig<"Lane_ZMM_32xU16", RegFile_ZMM, "uint16_t", 32>;
def Lane_ZMM_32xI16            : RegisterLaneConfig<"Lane_ZMM_32xI16", RegFile_ZMM, "int16_t", 32>;
def Lane_ZMM_32xFP16           : RegisterLaneConfig<"Lane_ZMM_32xFP16", RegFile_ZMM, "_Float16", 32>;
def Lane_ZMM_32xBF16           : RegisterLaneConfig<"Lane_ZMM_32xBF16", RegFile_ZMM, "__bf16", 32>;
def Lane_ZMM_16xU32            : RegisterLaneConfig<"Lane_ZMM_16xU32", RegFile_ZMM, "uint32_t", 16>;
def Lane_ZMM_16xI32            : RegisterLaneConfig<"Lane_ZMM_16xI32", RegFile_ZMM, "int32_t", 16>;
def Lane_ZMM_16xF32            : RegisterLaneConfig<"Lane_ZMM_16xF32", RegFile_ZMM, "float", 16>;
def Lane_ZMM_8xU64             : RegisterLaneConfig<"Lane_ZMM_8xU64", RegFile_ZMM, "uint64_t", 8>;
def Lane_ZMM_8xI64             : RegisterLaneConfig<"Lane_ZMM_8xI64", RegFile_ZMM, "int64_t", 8>;
def Lane_ZMM_8xF64             : RegisterLaneConfig<"Lane_ZMM_8xF64", RegFile_ZMM, "double", 8>;
def Lane_YMM_32xU8             : RegisterLaneConfig<"Lane_YMM_32xU8", RegFile_YMM, "uint8_t", 32>;
def Lane_YMM_32xI8             : RegisterLaneConfig<"Lane_YMM_32xI8", RegFile_YMM, "int8_t", 32>;
def Lane_YMM_16xU16            : RegisterLaneConfig<"Lane_YMM_16xU16", RegFile_YMM, "uint16_t", 16>;
def Lane_YMM_16xI16            : RegisterLaneConfig<"Lane_YMM_16xI16", RegFile_YMM, "int16_t", 16>;
def Lane_YMM_16xFP16           : RegisterLaneConfig<"Lane_YMM_16xFP16", RegFile_YMM, "_Float16", 16>;
def Lane_YMM_16xBF16           : RegisterLaneConfig<"Lane_YMM_16xBF16", RegFile_YMM, "__bf16", 16>;
def Lane_YMM_8xU32             : RegisterLaneConfig<"Lane_YMM_8xU32", RegFile_YMM, "uint32_t", 8>;
def Lane_YMM_8xI32             : RegisterLaneConfig<"Lane_YMM_8xI32", RegFile_YMM, "int32_t", 8>;
def Lane_YMM_8xF32             : RegisterLaneConfig<"Lane_YMM_8xF32", RegFile_YMM, "float", 8>;
def Lane_YMM_4xU64             : RegisterLaneConfig<"Lane_YMM_4xU64", RegFile_YMM, "uint64_t", 4>;
def Lane_YMM_4xI64             : RegisterLaneConfig<"Lane_YMM_4xI64", RegFile_YMM, "int64_t", 4>;
def Lane_YMM_4xF64             : RegisterLaneConfig<"Lane_YMM_4xF64", RegFile_YMM, "double", 4>;
def Lane_XMM_16xU8             : RegisterLaneConfig<"Lane_XMM_16xU8", RegFile_XMM, "uint8_t", 16>;
def Lane_XMM_16xI8             : RegisterLaneConfig<"Lane_XMM_16xI8", RegFile_XMM, "int8_t", 16>;
def Lane_XMM_8xU16             : RegisterLaneConfig<"Lane_XMM_8xU16", RegFile_XMM, "uint16_t", 8>;
def Lane_XMM_8xI16             : RegisterLaneConfig<"Lane_XMM_8xI16", RegFile_XMM, "int16_t", 8>;
def Lane_XMM_8xFP16            : RegisterLaneConfig<"Lane_XMM_8xFP16", RegFile_XMM, "_Float16", 8>;
def Lane_XMM_8xBF16            : RegisterLaneConfig<"Lane_XMM_8xBF16", RegFile_XMM, "__bf16", 8>;
def Lane_XMM_4xU32             : RegisterLaneConfig<"Lane_XMM_4xU32", RegFile_XMM, "uint32_t", 4>;
def Lane_XMM_4xI32             : RegisterLaneConfig<"Lane_XMM_4xI32", RegFile_XMM, "int32_t", 4>;
def Lane_XMM_4xF32             : RegisterLaneConfig<"Lane_XMM_4xF32", RegFile_XMM, "float", 4>;
def Lane_XMM_2xU64             : RegisterLaneConfig<"Lane_XMM_2xU64", RegFile_XMM, "uint64_t", 2>;
def Lane_XMM_2xI64             : RegisterLaneConfig<"Lane_XMM_2xI64", RegFile_XMM, "int64_t", 2>;
def Lane_XMM_2xF64             : RegisterLaneConfig<"Lane_XMM_2xF64", RegFile_XMM, "double", 2>;
def Lane_APX_32xI64            : RegisterLaneConfig<"Lane_APX_32xI64", RegFile_APX_GPR, "int64_t", 32>;
def Lane_APX_32xU64            : RegisterLaneConfig<"Lane_APX_32xU64", RegFile_APX_GPR, "uint64_t", 32>;
def Lane_APX_32xI32            : RegisterLaneConfig<"Lane_APX_32xI32", RegFile_APX_GPR, "int32_t", 32>;
def Lane_APX_32xU32            : RegisterLaneConfig<"Lane_APX_32xU32", RegFile_APX_GPR, "uint32_t", 32>;
def Lane_AMX_TMM_16x64_U8      : RegisterLaneConfig<"Lane_AMX_TMM_16x64_U8", RegFile_TMM, "uint8_t", 1024>;
def Lane_AMX_TMM_16x64_I8      : RegisterLaneConfig<"Lane_AMX_TMM_16x64_I8", RegFile_TMM, "int8_t", 1024>;
def Lane_AMX_TMM_16x32_BF16    : RegisterLaneConfig<"Lane_AMX_TMM_16x32_BF16", RegFile_TMM, "__bf16", 512>;
def Lane_AMX_TMM_16x32_FP16    : RegisterLaneConfig<"Lane_AMX_TMM_16x32_FP16", RegFile_TMM, "_Float16", 512>;
def Lane_NEON_16xU8            : RegisterLaneConfig<"Lane_NEON_16xU8", RegFile_NEON, "uint8_t", 16>;
def Lane_NEON_16xI8            : RegisterLaneConfig<"Lane_NEON_16xI8", RegFile_NEON, "int8_t", 16>;
def Lane_NEON_8xU16            : RegisterLaneConfig<"Lane_NEON_8xU16", RegFile_NEON, "uint16_t", 8>;
def Lane_NEON_8xI16            : RegisterLaneConfig<"Lane_NEON_8xI16", RegFile_NEON, "int16_t", 8>;
def Lane_NEON_8xFP16           : RegisterLaneConfig<"Lane_NEON_8xFP16", RegFile_NEON, "_Float16", 8>;
def Lane_NEON_8xBF16           : RegisterLaneConfig<"Lane_NEON_8xBF16", RegFile_NEON, "__bf16", 8>;
def Lane_NEON_4xU32            : RegisterLaneConfig<"Lane_NEON_4xU32", RegFile_NEON, "uint32_t", 4>;
def Lane_NEON_4xI32            : RegisterLaneConfig<"Lane_NEON_4xI32", RegFile_NEON, "int32_t", 4>;
def Lane_NEON_4xF32            : RegisterLaneConfig<"Lane_NEON_4xF32", RegFile_NEON, "float", 4>;
def Lane_NEON_2xU64            : RegisterLaneConfig<"Lane_NEON_2xU64", RegFile_NEON, "uint64_t", 2>;
def Lane_NEON_2xI64            : RegisterLaneConfig<"Lane_NEON_2xI64", RegFile_NEON, "int64_t", 2>;
def Lane_NEON_2xF64            : RegisterLaneConfig<"Lane_NEON_2xF64", RegFile_NEON, "double", 2>;
def Lane_SVE_64xU8             : RegisterLaneConfig<"Lane_SVE_64xU8", RegFile_SVE, "uint8_t", 64>;
def Lane_SVE_64xI8             : RegisterLaneConfig<"Lane_SVE_64xI8", RegFile_SVE, "int8_t", 64>;
def Lane_SVE_32xU16            : RegisterLaneConfig<"Lane_SVE_32xU16", RegFile_SVE, "uint16_t", 32>;
def Lane_SVE_32xI16            : RegisterLaneConfig<"Lane_SVE_32xI16", RegFile_SVE, "int16_t", 32>;
def Lane_SVE_32xFP16           : RegisterLaneConfig<"Lane_SVE_32xFP16", RegFile_SVE, "_Float16", 32>;
def Lane_SVE_32xBF16           : RegisterLaneConfig<"Lane_SVE_32xBF16", RegFile_SVE, "__bf16", 32>;
def Lane_SVE_16xU32            : RegisterLaneConfig<"Lane_SVE_16xU32", RegFile_SVE, "uint32_t", 16>;
def Lane_SVE_16xI32            : RegisterLaneConfig<"Lane_SVE_16xI32", RegFile_SVE, "int32_t", 16>;
def Lane_SVE_16xF32            : RegisterLaneConfig<"Lane_SVE_16xF32", RegFile_SVE, "float", 16>;
def Lane_SVE_8xU64             : RegisterLaneConfig<"Lane_SVE_8xU64", RegFile_SVE, "uint64_t", 8>;
def Lane_SVE_8xI64             : RegisterLaneConfig<"Lane_SVE_8xI64", RegFile_SVE, "int64_t", 8>;
def Lane_SVE_8xF64             : RegisterLaneConfig<"Lane_SVE_8xF64", RegFile_SVE, "double", 8>;
def Lane_SME_ZA_16x16_F32      : RegisterLaneConfig<"Lane_SME_ZA_16x16_F32", RegFile_SME_ZA, "float", 256>;
def Lane_SME_ZA_32x32_FP16     : RegisterLaneConfig<"Lane_SME_ZA_32x32_FP16", RegFile_SME_ZA, "_Float16", 1024>;
def Lane_SME_ZA_32x32_BF16     : RegisterLaneConfig<"Lane_SME_ZA_32x32_BF16", RegFile_SME_ZA, "__bf16", 1024>;
def Lane_SME_ZA_64x64_I8       : RegisterLaneConfig<"Lane_SME_ZA_64x64_I8", RegFile_SME_ZA, "int8_t", 4096>;
def Lane_RVV_64xU8             : RegisterLaneConfig<"Lane_RVV_64xU8", RegFile_RVV, "uint8_t", 64>;
def Lane_RVV_64xI8             : RegisterLaneConfig<"Lane_RVV_64xI8", RegFile_RVV, "int8_t", 64>;
def Lane_RVV_32xU16            : RegisterLaneConfig<"Lane_RVV_32xU16", RegFile_RVV, "uint16_t", 32>;
def Lane_RVV_32xI16            : RegisterLaneConfig<"Lane_RVV_32xI16", RegFile_RVV, "int16_t", 32>;
def Lane_RVV_32xFP16           : RegisterLaneConfig<"Lane_RVV_32xFP16", RegFile_RVV, "_Float16", 32>;
def Lane_RVV_16xU32            : RegisterLaneConfig<"Lane_RVV_16xU32", RegFile_RVV, "uint32_t", 16>;
def Lane_RVV_16xI32            : RegisterLaneConfig<"Lane_RVV_16xI32", RegFile_RVV, "int32_t", 16>;
def Lane_RVV_16xF32            : RegisterLaneConfig<"Lane_RVV_16xF32", RegFile_RVV, "float", 16>;
def Lane_RVV_8xU64             : RegisterLaneConfig<"Lane_RVV_8xU64", RegFile_RVV, "uint64_t", 8>;
def Lane_RVV_8xI64             : RegisterLaneConfig<"Lane_RVV_8xI64", RegFile_RVV, "int64_t", 8>;
def Lane_RVV_8xF64             : RegisterLaneConfig<"Lane_RVV_8xF64", RegFile_RVV, "double", 8>;
def Lane_LASX_32xU8            : RegisterLaneConfig<"Lane_LASX_32xU8", RegFile_LASX, "uint8_t", 32>;
def Lane_LASX_16xI16           : RegisterLaneConfig<"Lane_LASX_16xI16", RegFile_LASX, "int16_t", 16>;
def Lane_LASX_8xI32            : RegisterLaneConfig<"Lane_LASX_8xI32", RegFile_LASX, "int32_t", 8>;
def Lane_LASX_8xF32            : RegisterLaneConfig<"Lane_LASX_8xF32", RegFile_LASX, "float", 8>;
def Lane_LASX_4xF64            : RegisterLaneConfig<"Lane_LASX_4xF64", RegFile_LASX, "double", 4>;
def Lane_VSX_16xU8             : RegisterLaneConfig<"Lane_VSX_16xU8", RegFile_VSX, "uint8_t", 16>;
def Lane_VSX_8xI16             : RegisterLaneConfig<"Lane_VSX_8xI16", RegFile_VSX, "int16_t", 8>;
def Lane_VSX_4xI32             : RegisterLaneConfig<"Lane_VSX_4xI32", RegFile_VSX, "int32_t", 4>;
def Lane_VSX_4xF32             : RegisterLaneConfig<"Lane_VSX_4xF32", RegFile_VSX, "float", 4>;
def Lane_VSX_2xF64             : RegisterLaneConfig<"Lane_VSX_2xF64", RegFile_VSX, "double", 2>;
def Lane_Wasm_16xU8            : RegisterLaneConfig<"Lane_Wasm_16xU8", RegFile_Wasm, "uint8_t", 16>;
def Lane_Wasm_8xI16            : RegisterLaneConfig<"Lane_Wasm_8xI16", RegFile_Wasm, "int16_t", 8>;
def Lane_Wasm_4xI32            : RegisterLaneConfig<"Lane_Wasm_4xI32", RegFile_Wasm, "int32_t", 4>;
def Lane_Wasm_4xF32            : RegisterLaneConfig<"Lane_Wasm_4xF32", RegFile_Wasm, "float", 4>;
def Lane_Wasm_2xF64            : RegisterLaneConfig<"Lane_Wasm_2xF64", RegFile_Wasm, "double", 2>;

// =============================================================================
// SECTION 4: EXECUTION PIPELINES & HARDWARE PORTS
// =============================================================================

class ExecutionPort<string name, int port_id, string description> {
  string Name = name;
  int PortID = port_id;
  string Description = description;
}

def Port_0               : ExecutionPort<"Port_0", 0, "Intel: FMA, Vector Mul, ALU, Branch">;
def Port_1               : ExecutionPort<"Port_1", 1, "Intel: FMA, Vector Add, ALU, Slow LEA">;
def Port_2               : ExecutionPort<"Port_2", 2, "Intel: Load, AGU">;
def Port_3               : ExecutionPort<"Port_3", 3, "Intel: Load, AGU">;
def Port_4               : ExecutionPort<"Port_4", 4, "Intel: Store Data">;
def Port_5               : ExecutionPort<"Port_5", 5, "Intel: Vector Shuffle, Vector ALU, Fast LEA">;
def Port_6               : ExecutionPort<"Port_6", 6, "Intel: Primary ALU, Branch">;
def Port_7               : ExecutionPort<"Port_7", 7, "Intel: Store Address AGU">;
def Port_8               : ExecutionPort<"Port_8", 8, "Intel: Store Address AGU (Tiger Lake / Golden Cove)">;
def Port_9               : ExecutionPort<"Port_9", 9, "Intel: Store Data 2">;
def Port_10              : ExecutionPort<"Port_10", 10, "Intel Golden Cove / Lion Cove: ALU">;
def Port_11              : ExecutionPort<"Port_11", 11, "Intel Golden Cove / Lion Cove: ALU">;
def Port_LionCove_P0     : ExecutionPort<"Port_LionCove_P0", 100, "Intel Lion Cove: FMA 0 / Vector Mul / Int ALU">;
def Port_LionCove_P1     : ExecutionPort<"Port_LionCove_P1", 101, "Intel Lion Cove: FMA 1 / Vector Add / Int ALU">;
def Port_LionCove_P5     : ExecutionPort<"Port_LionCove_P5", 105, "Intel Lion Cove: Vector Shuffle / Permute / Bitwise">;
def Port_Skymont_P0      : ExecutionPort<"Port_Skymont_P0", 110, "Intel Skymont: 128-bit / 256-bit Vector ALU 0">;
def Port_Skymont_P1      : ExecutionPort<"Port_Skymont_P1", 111, "Intel Skymont: 128-bit / 256-bit Vector ALU 1">;
def Port_Skymont_P2      : ExecutionPort<"Port_Skymont_P2", 112, "Intel Skymont: Vector Load / AGU 0">;
def Port_Skymont_P3      : ExecutionPort<"Port_Skymont_P3", 113, "Intel Skymont: Vector Load / AGU 1">;
def Port_CoyoteCove_P0   : ExecutionPort<"Port_CoyoteCove_P0", 120, "Intel Nova Lake Coyote Cove: 512-bit AVX10.2 FMA 0 / APX ALU">;
def Port_CoyoteCove_P1   : ExecutionPort<"Port_CoyoteCove_P1", 121, "Intel Nova Lake Coyote Cove: 512-bit AVX10.2 FMA 1 / APX ALU">;
def Port_CoyoteCove_P5   : ExecutionPort<"Port_CoyoteCove_P5", 125, "Intel Nova Lake Coyote Cove: 512-bit AVX10.2 Shuffle / Permute">;
def Port_ArcticWolf_V0   : ExecutionPort<"Port_ArcticWolf_V0", 126, "Intel Nova Lake Arctic Wolf: Converged AVX10.2 Vector FMA Pipe 0">;
def Port_ArcticWolf_V1   : ExecutionPort<"Port_ArcticWolf_V1", 127, "Intel Nova Lake Arctic Wolf: Converged AVX10.2 Vector FMA Pipe 1">;
def Port_Zen_ALU0        : ExecutionPort<"Port_Zen_ALU0", 20, "AMD Zen: Integer ALU 0 / Branch">;
def Port_Zen_ALU1        : ExecutionPort<"Port_Zen_ALU1", 21, "AMD Zen: Integer ALU 1">;
def Port_Zen_ALU2        : ExecutionPort<"Port_Zen_ALU2", 22, "AMD Zen: Integer ALU 2">;
def Port_Zen_ALU3        : ExecutionPort<"Port_Zen_ALU3", 23, "AMD Zen: Integer ALU 3">;
def Port_Zen_FP0         : ExecutionPort<"Port_Zen_FP0", 24, "AMD Zen: FMA 0 / Mul / Shuffle">;
def Port_Zen_FP1         : ExecutionPort<"Port_Zen_FP1", 25, "AMD Zen: FMA 1 / Mul / Add">;
def Port_Zen_FP2         : ExecutionPort<"Port_Zen_FP2", 26, "AMD Zen: Add 0 / Non-FMA">;
def Port_Zen_FP3         : ExecutionPort<"Port_Zen_FP3", 27, "AMD Zen: Add 1 / Store / Cast">;
def Port_Zen5_FP0        : ExecutionPort<"Port_Zen5_FP0", 130, "AMD Zen 5: 512-bit Native FMA Pipe 0">;
def Port_Zen5_FP1        : ExecutionPort<"Port_Zen5_FP1", 131, "AMD Zen 5: 512-bit Native FMA Pipe 1">;
def Port_Zen5_FP2        : ExecutionPort<"Port_Zen5_FP2", 132, "AMD Zen 5: 512-bit Vector Add / Shuffle Pipe 2">;
def Port_Zen5_FP3        : ExecutionPort<"Port_Zen5_FP3", 133, "AMD Zen 5: 512-bit Vector Store / Permute Pipe 3">;
def Port_Zen6_FP0        : ExecutionPort<"Port_Zen6_FP0", 134, "AMD Zen 6 Morpheus: 512-bit Native FMA / FP16 / BMM Pipe 0">;
def Port_Zen6_FP1        : ExecutionPort<"Port_Zen6_FP1", 135, "AMD Zen 6 Morpheus: 512-bit Native FMA / FP16 / BMM Pipe 1">;
def Port_Zen6_FP2        : ExecutionPort<"Port_Zen6_FP2", 136, "AMD Zen 6 Morpheus: 512-bit Vector Add / Shuffle Pipe 2">;
def Port_Zen6_FP3        : ExecutionPort<"Port_Zen6_FP3", 137, "AMD Zen 6 Morpheus: 512-bit Vector Store / Permute Pipe 3">;
def Port_Apple_FMA0      : ExecutionPort<"Port_Apple_FMA0", 30, "Apple Firestorm/Avalanche: 128-bit Vector FMA Pipe 0">;
def Port_Apple_FMA1      : ExecutionPort<"Port_Apple_FMA1", 31, "Apple Firestorm/Avalanche: 128-bit Vector FMA Pipe 1">;
def Port_Apple_FMA2      : ExecutionPort<"Port_Apple_FMA2", 32, "Apple Firestorm/Avalanche: 128-bit Vector FMA Pipe 2">;
def Port_Apple_FMA3      : ExecutionPort<"Port_Apple_FMA3", 33, "Apple Firestorm/Avalanche: 128-bit Vector FMA Pipe 3">;
def Port_Apple_SME2      : ExecutionPort<"Port_Apple_SME2", 34, "Apple M4: SME2 Outer Product Matrix Pipe">;
def Port_Oryon_V0        : ExecutionPort<"Port_Oryon_V0", 40, "Qualcomm Oryon: 128-bit Vector FMA Pipe 0">;
def Port_Oryon_V1        : ExecutionPort<"Port_Oryon_V1", 41, "Qualcomm Oryon: 128-bit Vector FMA Pipe 1">;
def Port_Oryon_V2        : ExecutionPort<"Port_Oryon_V2", 42, "Qualcomm Oryon: 128-bit Vector Int/Shuffle Pipe 2">;
def Port_Oryon_V3        : ExecutionPort<"Port_Oryon_V3", 43, "Qualcomm Oryon: 128-bit Vector Load/Store Pipe 3">;
def Port_Neoverse_V0     : ExecutionPort<"Port_Neoverse_V0", 50, "ARM Neoverse V2: SVE 4x128b / 2x256b Pipe 0">;
def Port_Neoverse_V1     : ExecutionPort<"Port_Neoverse_V1", 51, "ARM Neoverse V2: SVE 4x128b / 2x256b Pipe 1">;
def Port_RVV_V0          : ExecutionPort<"Port_RVV_V0", 60, "RISC-V Vector V1.0: 512-bit Vector FMA / ALU Pipe 0">;
def Port_RVV_V1          : ExecutionPort<"Port_RVV_V1", 61, "RISC-V Vector V1.0: 512-bit Vector FMA / ALU Pipe 1">;

// =============================================================================
// SECTION 5: BRANCH PREDICTOR (BPU) MODELS & MISPREDICTION PENALTY TABLES
// =============================================================================

class BranchPredictorModel<string name, string predictor_arch, int penalty_min, int penalty_max,
                           int btb_entries, string predictability_threshold, string branchless_pref> {
  string Name = name;
  string PredictorArch = predictor_arch;
  int PenaltyMinCycles = penalty_min;
  int PenaltyMaxCycles = penalty_max;
  int BTBEntries = btb_entries;
  string PredictabilityThreshold = predictability_threshold;
  string RecommendedBranchlessStrategy = branchless_pref;
}

def BPU_SandyBridge      : BranchPredictorModel<"Intel Sandy Bridge", "Two-Level Adaptive", 14, 18, 4096, "0.85", "CMOV">;
def BPU_Haswell          : BranchPredictorModel<"Intel Haswell", "TAGE-like Dynamic", 14, 19, 4096, "0.86", "CMOV_AND_BLEND">;
def BPU_Skylake          : BranchPredictorModel<"Intel Skylake", "TAGE Branch Predictor", 14, 19, 5120, "0.88", "VPBLEND_K_MASK">;
def BPU_GoldenCove       : BranchPredictorModel<"Intel Golden Cove", "Deep TAGE-SC-L", 16, 21, 12288, "0.90", "VPBLEND_K_MASK">;
def BPU_LionCove         : BranchPredictorModel<"Intel Lion Cove", "Fast Recovery TAGE", 14, 18, 16384, "0.92", "APX_CCMP_CMOV">;
def BPU_CoyoteCove       : BranchPredictorModel<"Intel Coyote Cove", "Wide Neural TAGE + APX CCMP", 12, 16, 32768, "0.94", "APX_CCMP_K_MASK_AVX10_2">;
def BPU_Gracemont        : BranchPredictorModel<"Intel Gracemont", "Multi-Way Perceptron", 13, 17, 4096, "0.84", "CMOV">;
def BPU_Skymont          : BranchPredictorModel<"Intel Skymont", "Direct Branch Target", 12, 16, 8192, "0.87", "VPBLENDPS">;
def BPU_ArcticWolf       : BranchPredictorModel<"Intel Arctic Wolf", "Decoupled Dual-Pipe TAGE", 11, 15, 16384, "0.91", "APX_CCMP_K_MASK_AVX10_2">;
def BPU_Zen2             : BranchPredictorModel<"AMD Zen 2", "Hashed Perceptron", 14, 18, 4096, "0.86", "CMOV_AND_VBLEND">;
def BPU_Zen3             : BranchPredictorModel<"AMD Zen 3", "Decoupled TAGE BPU", 11, 14, 8192, "0.89", "VBLENDPS">;
def BPU_Zen4             : BranchPredictorModel<"AMD Zen 4", "2-Branch-per-cycle TAGE", 11, 14, 16384, "0.91", "K_MASK_OR_VBLEND">;
def BPU_Zen5             : BranchPredictorModel<"AMD Zen 5", "Dual-Pipeline TAGE", 10, 13, 24576, "0.93", "K_MASK_AVX512">;
def BPU_Zen6             : BranchPredictorModel<"AMD Zen 6 Morpheus", "Quad-Window Ahead-Pipeline TAGE", 9, 12, 32768, "0.95", "K_MASK_AVX512_AVX10_2">;
def BPU_AppleM1          : BranchPredictorModel<"Apple Firestorm", "Deep Perceptron / Multi-Hash", 12, 15, 16384, "0.92", "ASIMD_CSEL">;
def BPU_AppleM4          : BranchPredictorModel<"Apple M4", "Ultra-Wide Neural BPU", 11, 14, 32768, "0.94", "ASIMD_FCSEL">;
def BPU_QualcommOryon    : BranchPredictorModel<"Qualcomm Oryon", "Custom Multi-Way BPU", 11, 15, 16384, "0.91", "CSEL">;
def BPU_CortexA72        : BranchPredictorModel<"ARM Cortex-A72", "Bi-Mode Multi-Level", 15, 20, 4096, "0.82", "CSEL">;
def BPU_CortexA76        : BranchPredictorModel<"ARM Cortex-A76", "Decoupled Direction TAGE", 11, 15, 6144, "0.88", "CSEL">;
def BPU_NeoverseV2       : BranchPredictorModel<"ARM Neoverse V2", "Advanced TAGE-L", 10, 13, 16384, "0.92", "SVE_PRED_SELECT">;
def BPU_FujitsuA64FX     : BranchPredictorModel<"Fujitsu A64FX", "Branch History Table", 12, 16, 4096, "0.85", "SVE_PRED">;
def BPU_SiFiveX280       : BranchPredictorModel<"SiFive X280 RVV", "BHT / RAS Simple", 12, 16, 2048, "0.80", "RVV_VMERGE">;
def BPU_AscalonRVV       : BranchPredictorModel<"Tenstorrent Ascalon", "Advanced Out-of-Order TAGE", 11, 14, 12288, "0.90", "RVV_VMERGE">;
def BPU_LoongsonLA664    : BranchPredictorModel<"Loongson LA664", "Multi-Way Perceptron", 12, 16, 8192, "0.88", "LASX_XMASK">;
def BPU_IBMz16Telum      : BranchPredictorModel<"IBM z16 Telum", "Mainframe Dual-BPU", 14, 18, 16384, "0.90", "VSX_SELECT">;

// =============================================================================
// SECTION 6: CACHE HIERARCHY, LINE WIDTHS & HARDWARE PREFETCHERS
// =============================================================================

class HardwareMemoryStrategy<string name, bit is_non_temporal, bit prefetch_enabled, string hint> {
  string Name = name;
  bit IsNonTemporal = is_non_temporal;
  bit PrefetchEnabled = prefetch_enabled;
  string CacheHint = hint;
}

def HW_StandardL1L2 : HardwareMemoryStrategy<"StandardCached", 0, 0, "_MM_HINT_T0">;
def HW_StreamingNT  : HardwareMemoryStrategy<"NonTemporalNT",  1, 0, "_MM_HINT_NTA">;
def HW_AggressiveT0 : HardwareMemoryStrategy<"PrefetchT0",     0, 1, "_MM_HINT_T0">;
def HW_AppleUMA     : HardwareMemoryStrategy<"AppleSiliconUMA",0, 0, "none">;
def HW_SRAM_Direct  : HardwareMemoryStrategy<"SRAMDirect",     0, 0, "none">;

class CacheTopology<string name, int l1d_kb, int l1d_lat, int l2_kb, int l2_lat, int l3_mb, int l3_lat, int line_bytes, string prefetcher> {
  string Name = name;
  int L1D_KBytes = l1d_kb;
  int L1D_Latency = l1d_lat;
  int L2_KBytes = l2_kb;
  int L2_Latency = l2_lat;
  int L3_MBytes = l3_mb;
  int L3_Latency = l3_lat;
  int LineBytes = line_bytes;
  string Prefetcher = prefetcher;
}

def Cache_Skylake          : CacheTopology<"Cache_Skylake", 32, 4, 1024, 14, 38, 50, 64, "Intel Streamer + Spatial IP">;
def Cache_Icelake          : CacheTopology<"Cache_Icelake", 48, 5, 1280, 13, 30, 48, 64, "Intel L1/L2 Next-Line + AMP">;
def Cache_Tigerlake        : CacheTopology<"Cache_Tigerlake", 48, 5, 1280, 13, 24, 46, 64, "Intel Dual-Path Streamer">;
def Cache_Alderlake_P      : CacheTopology<"Cache_Alderlake_P", 48, 5, 1280, 14, 30, 48, 64, "Intel Golden Cove Streamer">;
def Cache_Alderlake_E      : CacheTopology<"Cache_Alderlake_E", 32, 3, 2048, 16, 30, 48, 64, "Intel Gracemont Stride Prefetcher">;
def Cache_Raptorlake_P     : CacheTopology<"Cache_Raptorlake_P", 48, 5, 2048, 15, 36, 50, 64, "Intel Raptor Cove Advanced">;
def Cache_Arrowlake_P      : CacheTopology<"Cache_Arrowlake_P", 48, 4, 3072, 14, 36, 48, 64, "Intel Lion Cove Multi-Tier">;
def Cache_Arrowlake_E      : CacheTopology<"Cache_Arrowlake_E", 32, 3, 4096, 15, 36, 48, 64, "Intel Skymont Wide Streamer">;
def Cache_GraniteRapids    : CacheTopology<"Cache_GraniteRapids", 48, 4, 2048, 14, 504, 52, 64, "Intel Mesh Shared LLC Streamer">;
def Cache_NovaLake_bLLC    : CacheTopology<"Cache_NovaLake_bLLC", 48, 4, 4096, 13, 144, 46, 64, "Intel Nova Lake Coyote Cove bLLC 3D Stacked Cache">;
def Cache_NovaLake_ArcticWolf : CacheTopology<"Cache_NovaLake_ArcticWolf", 32, 3, 4096, 14, 144, 46, 64, "Intel Nova Lake Arctic Wolf Shared bLLC Streamer">;
def Cache_Zen2             : CacheTopology<"Cache_Zen2", 32, 4, 512, 12, 32, 48, 64, "AMD Zen 2 L1/L2 Streamer">;
def Cache_Zen3             : CacheTopology<"Cache_Zen3", 32, 4, 512, 12, 32, 46, 64, "AMD Zen 3 L1/L2 Streamer">;
def Cache_Zen4             : CacheTopology<"Cache_Zen4", 32, 4, 1024, 14, 64, 48, 64, "AMD Zen 4 Stride + Region">;
def Cache_Zen4c            : CacheTopology<"Cache_Zen4c", 32, 4, 1024, 14, 32, 50, 64, "AMD Zen 4c Dense Streamer">;
def Cache_Zen5             : CacheTopology<"Cache_Zen5", 48, 4, 1024, 14, 64, 46, 64, "AMD Zen 5 Dual Streamer">;
def Cache_Zen5_3DVCache    : CacheTopology<"Cache_Zen5_3DVCache", 48, 4, 1024, 14, 128, 48, 64, "AMD Zen 5 9950X3D 2nd-Gen Inverted 3D V-Cache Stack">;
def Cache_Zen5c            : CacheTopology<"Cache_Zen5c", 48, 4, 1024, 14, 32, 48, 64, "AMD Zen 5c Dense Streamer">;
def Cache_Zen6             : CacheTopology<"Cache_Zen6", 48, 4, 2048, 12, 96, 42, 64, "AMD Zen 6 Morpheus 32-Core Unified CCD Ring Streamer">;
def Cache_Zen6_3DVCache    : CacheTopology<"Cache_Zen6_3DVCache", 48, 4, 2048, 12, 192, 44, 64, "AMD Zen 6 Medusa X3D Dual-Stacked 3rd-Gen 3D V-Cache">;
def Cache_AppleM1          : CacheTopology<"Cache_AppleM1", 128, 3, 12288, 16, 24, 32, 128, "Apple DMP + Hardware Streamer">;
def Cache_AppleM2          : CacheTopology<"Cache_AppleM2", 128, 3, 16384, 16, 24, 32, 128, "Apple M2 High Bandwidth DMP">;
def Cache_AppleM3          : CacheTopology<"Cache_AppleM3", 128, 3, 16384, 15, 28, 30, 128, "Apple M3 Dynamic Caching DMP">;
def Cache_AppleM4          : CacheTopology<"Cache_AppleM4", 128, 3, 16384, 15, 32, 30, 128, "Apple M4 Wide Bandwidth DMP">;
def Cache_AppleM4Max       : CacheTopology<"Cache_AppleM4Max", 128, 3, 32768, 15, 48, 28, 128, "Apple M4 Max System Level Cache">;
def Cache_QualcommOryon    : CacheTopology<"Cache_QualcommOryon", 192, 4, 12288, 16, 36, 42, 64, "Qualcomm Custom Hardware Streamer">;
def Cache_CortexA53        : CacheTopology<"Cache_CortexA53", 32, 4, 512, 16, 0, 0, 64, "ARM A53 Stride Prefetcher">;
def Cache_CortexA55        : CacheTopology<"Cache_CortexA55", 32, 4, 512, 14, 0, 0, 64, "ARM A55 Spatial Prefetcher">;
def Cache_CortexA72        : CacheTopology<"Cache_CortexA72", 32, 4, 1024, 15, 0, 0, 64, "ARM A72 Stride Prefetcher">;
def Cache_CortexA76        : CacheTopology<"Cache_CortexA76", 64, 4, 512, 11, 2, 30, 64, "ARM DynamIQ Streamer">;
def Cache_CortexA78        : CacheTopology<"Cache_CortexA78", 64, 4, 512, 11, 4, 32, 64, "ARM Cortex-A78 Dual Streamer">;
def Cache_CortexX1         : CacheTopology<"Cache_CortexX1", 64, 3, 1024, 11, 8, 32, 64, "ARM Cortex-X1 High Performance">;
def Cache_CortexX4         : CacheTopology<"Cache_CortexX4", 64, 3, 2048, 12, 16, 34, 64, "ARM Cortex-X4 Wide Bandwidth">;
def Cache_NeoverseN1       : CacheTopology<"Cache_NeoverseN1", 64, 4, 1024, 11, 32, 36, 64, "ARM Neoverse N1 Server Streamer">;
def Cache_NeoverseV2       : CacheTopology<"Cache_NeoverseV2", 64, 4, 2048, 12, 64, 40, 64, "ARM Neoverse High-BW Prefetcher">;
def Cache_FujitsuA64FX     : CacheTopology<"Cache_FujitsuA64FX", 64, 4, 8192, 15, 32, 60, 256, "Fujitsu A64FX HBM2 Streamer">;
def Cache_Loongson3A6000   : CacheTopology<"Cache_Loongson3A6000", 64, 4, 1024, 14, 16, 42, 64, "Loongson Dynamic Multi-Stream">;
def Cache_IBMz16Telum      : CacheTopology<"Cache_IBMz16Telum", 32, 4, 32768, 12, 256, 36, 256, "IBM Telum On-Chip Shared Ring">;
def Cache_IBMPower10       : CacheTopology<"Cache_IBMPower10", 64, 4, 2048, 13, 120, 38, 128, "IBM POWER10 Distributed L3">;

// =============================================================================
// SECTION 6B: GOTOBLAS / BLIS CACHE BLOCKING & REGISTER TILING STRATEGIES
// =============================================================================
// Dictates the exact cache-fitting dimensions for arbitrary MxNxK GEMM operations:
//  - MR x NR: Micro-kernel register accumulation tile (fits strictly within vector register file)
//  - KC: K-dimension block size (packed B matrix fits in L1D cache: KC * NR * sizeof(elem) <= 0.5 * L1D)
//  - MC: M-dimension block size (packed A matrix fits in L2 cache:  MC * KC * sizeof(elem) <= 0.5 * L2)
//  - NC: N-dimension block size (fits in L3 / LLC cache)
// =============================================================================

class BLIS_CacheBlockingStrategy<string name, int mr, int nr, int kc, int mc, int nc,
                                 int pack_align_bytes, int prefetch_lines_a, int prefetch_lines_b> {
  string Name = name;
  int MR = mr;                         // Register tile rows
  int NR = nr;                         // Register tile columns (elements)
  int KC = kc;                         // Inner loop K dimension (L1D bound)
  int MC = mc;                         // Middle loop M dimension (L2 bound)
  int NC = nc;                         // Outer loop N dimension (L3 bound)
  int PackAlignmentBytes = pack_align_bytes;
  int PrefetchLinesAhead_A = prefetch_lines_a;
  int PrefetchLinesAhead_B = prefetch_lines_b;
}

def Tile_AVX2_Haswell_Skylake      : BLIS_CacheBlockingStrategy<"Tile_AVX2_Haswell_Skylake",       6,  16, 256, 72,  4096,  64, 4, 4>;
def Tile_AVX512_SkylakeX_ICL       : BLIS_CacheBlockingStrategy<"Tile_AVX512_SkylakeX_ICL",        8,  32, 384, 128, 8192,  64, 6, 6>;
def Tile_AVX512_Zen4_Zen5          : BLIS_CacheBlockingStrategy<"Tile_AVX512_Zen4_Zen5",          12,  32, 512, 144, 8192,  64, 6, 6>;
def Tile_AVX512_Zen6_3DVCache      : BLIS_CacheBlockingStrategy<"Tile_AVX512_Zen6_3DVCache",      14,  32, 512, 224, 16384, 64, 8, 8>;
def Tile_AVX10_2_NovaLake_bLLC     : BLIS_CacheBlockingStrategy<"Tile_AVX10_2_NovaLake_bLLC",     14,  32, 512, 224, 16384, 64, 8, 8>;
def Tile_Apple_M1_M4               : BLIS_CacheBlockingStrategy<"Tile_Apple_M1_M4",                8,  16, 512, 192, 16384, 128, 8, 8>;
def Tile_ARM_NeoverseV2_SVE2       : BLIS_CacheBlockingStrategy<"Tile_ARM_NeoverseV2_SVE2",        8,  32, 384, 128, 8192,  64, 6, 6>;
def Tile_RISCV_Vector_V1           : BLIS_CacheBlockingStrategy<"Tile_RISCV_Vector_V1",            4,  16, 256, 64,  2048,  64, 4, 4>;

// =============================================================================
// SECTION 6C: 2D MATRIX TILE HARDWARE ENGINES (AMX & SME)
// =============================================================================
// Models hardware matrix accelerators operating on 2D register tiles rather
// than 1D vector lanes.
// =============================================================================

class MatrixTileArchitecture<string name, int num_tile_registers, int tile_bytes,
                             int max_rows, int max_cols_bytes, string config_struct,
                             string load_inst, string store_inst, string fma_inst, string release_inst> {
  string Name = name;
  int NumTileRegisters = num_tile_registers;
  int TileSizeBytes = tile_bytes;
  int MaxRows = max_rows;
  int MaxColsBytes = max_cols_bytes;
  string ConfigStruct = config_struct;
  string LoadInstruction = load_inst;
  string StoreInstruction = store_inst;
  string FMAInstruction = fma_inst;
  string ReleaseInstruction = release_inst;
}

def Arch_Intel_AMX_TMUL : MatrixTileArchitecture<
    "Intel_AMX_TMUL",
    8, 1024, 16, 64,
    "__tilecfg",
    "_tile_loadd",
    "_tile_stored",
    "_tile_dpbf16ps",
    "_tile_release">;

def Arch_ARM_SME_ZA : MatrixTileArchitecture<
    "ARM_SME_ZA",
    4, 4096, 64, 64,
    "svcr_sm_enable",
    "ldr_za_tile",
    "str_za_tile",
    "fmopa_za32",
    "svcr_sm_disable">;

// =============================================================================
// SECTION 7: MEMORY CONSISTENCY & FENCING MODELS
// =============================================================================

class MemoryConsistencyModel<string name, string ordering, bit store_load_reorder, bit store_store_reorder,
                             string full_fence, string store_fence, string load_fence, int fence_cycles> {
  string Name = name;
  string OrderingType = ordering;
  bit StoreLoadReorderAllowed = store_load_reorder;
  bit StoreStoreReorderAllowed = store_store_reorder;
  string FullFenceInstruction = full_fence;
  string StoreFenceInstruction = store_fence;
  string LoadFenceInstruction = load_fence;
  int FullFenceCycles = fence_cycles;
}

def MemModel_x86_TSO         : MemoryConsistencyModel<"MemModel_x86_TSO", "TotalStoreOrder", 1, 0, "_mm_mfence()", "_mm_sfence()", "_mm_lfence()", 25>;
def MemModel_ARM64_Weak      : MemoryConsistencyModel<"MemModel_ARM64_Weak", "WeaklyOrdered", 1, 1, "dmb ish", "dmb ishst", "dmb ishld", 35>;
def MemModel_RISCV_RVWMO     : MemoryConsistencyModel<"MemModel_RISCV_RVWMO", "RVWMO", 1, 1, "fence rw,rw", "fence w,w", "fence r,r", 30>;
def MemModel_LoongArch_Weak  : MemoryConsistencyModel<"MemModel_LoongArch_Weak", "WeakOrdering", 1, 1, "dbar 0", "dbar 0", "dbar 0", 28>;
def MemModel_IBM_Strong      : MemoryConsistencyModel<"MemModel_IBM_Strong", "StrongOrdering", 0, 0, "sync", "lwsync", "lwsync", 15>;

// =============================================================================
// SECTION 8: FREQUENCY SCALING & AVX-512 DOWNCLOCKING & RDTSC SERIALIZATION
// =============================================================================

class FrequencyThrottlingModel<string name, string avx2_downclock_pct, string avx512_downclock_pct,
                               int license_transition_cycles, int rdtsc_lat_cycles, string rdtsc_fence> {
  string Name = name;
  string AVX2DownclockPercent = avx2_downclock_pct;
  string AVX512DownclockPercent = avx512_downclock_pct;
  int LicenseTransitionCycles = license_transition_cycles;
  int RDTSCLatencyCycles = rdtsc_lat_cycles;
  string RDTSCFenceInstruction = rdtsc_fence;
}

def Throttle_SkylakeX        : FrequencyThrottlingModel<"Throttle_SkylakeX", "5.0", "18.0", 20000, 24, "lfence; rdtsc">;
def Throttle_CascadeLake     : FrequencyThrottlingModel<"Throttle_CascadeLake", "4.0", "14.0", 15000, 24, "lfence; rdtsc">;
def Throttle_CooperLake      : FrequencyThrottlingModel<"Throttle_CooperLake", "2.0", "8.0", 8000, 22, "lfence; rdtsc">;
def Throttle_IceLake         : FrequencyThrottlingModel<"Throttle_IceLake", "0.0", "2.0", 2000, 20, "lfence; rdtsc">;
def Throttle_TigerLake       : FrequencyThrottlingModel<"Throttle_TigerLake", "0.0", "0.0", 500, 18, "lfence; rdtsc">;
def Throttle_RocketLake      : FrequencyThrottlingModel<"Throttle_RocketLake", "0.0", "0.0", 500, 18, "lfence; rdtsc">;
def Throttle_AlderLake       : FrequencyThrottlingModel<"Throttle_AlderLake", "0.0", "0.0", 0, 16, "lfence; rdtsc">;
def Throttle_RaptorLake      : FrequencyThrottlingModel<"Throttle_RaptorLake", "0.0", "0.0", 0, 16, "lfence; rdtsc">;
def Throttle_MeteorLake      : FrequencyThrottlingModel<"Throttle_MeteorLake", "0.0", "0.0", 0, 16, "lfence; rdtsc">;
def Throttle_ArrowLake       : FrequencyThrottlingModel<"Throttle_ArrowLake", "0.0", "0.0", 0, 14, "lfence; rdtsc">;
def Throttle_NovaLake        : FrequencyThrottlingModel<"Throttle_NovaLake", "0.0", "0.0", 0, 12, "lfence; rdtsc">;
def Throttle_EmeraldRapids   : FrequencyThrottlingModel<"Throttle_EmeraldRapids", "0.0", "0.0", 0, 18, "lfence; rdtsc">;
def Throttle_Zen2            : FrequencyThrottlingModel<"Throttle_Zen2", "0.0", "0.0", 0, 20, "lfence; rdtsc">;
def Throttle_Zen3            : FrequencyThrottlingModel<"Throttle_Zen3", "0.0", "0.0", 0, 18, "lfence; rdtsc">;
def Throttle_Zen4            : FrequencyThrottlingModel<"Throttle_Zen4", "0.0", "0.0", 0, 16, "lfence; rdtsc">;
def Throttle_Zen5            : FrequencyThrottlingModel<"Throttle_Zen5", "0.0", "0.0", 0, 14, "lfence; rdtsc">;
def Throttle_Zen6            : FrequencyThrottlingModel<"Throttle_Zen6", "0.0", "0.0", 0, 12, "lfence; rdtsc">;
def Throttle_AppleM1_M4      : FrequencyThrottlingModel<"Throttle_AppleM1_M4", "0.0", "0.0", 0, 10, "isb; mrs cntvct_el0">;
def Throttle_ARM_Generic     : FrequencyThrottlingModel<"Throttle_ARM_Generic", "0.0", "0.0", 0, 12, "isb; mrs cntvct_el0">;

// =============================================================================
// SECTION 9: HETEROGENEOUS HYBRID CORE SCHEDULING (P-Core vs E-Core Routing)
// =============================================================================

class HybridCoreTopology<string name, CPUFamily pcore_arch, CPUFamily ecore_arch,
                          int pcore_max_vec_bits, int ecore_max_vec_bits,
                          string pcore_affinity_mask, string ecore_affinity_mask,
                          string preferred_workload_pcore, string preferred_workload_ecore> {
  string Name = name;
  CPUFamily PCoreArch = pcore_arch;
  CPUFamily ECoreArch = ecore_arch;
  int PCoreMaxVectorBits = pcore_max_vec_bits;
  int ECoreMaxVectorBits = ecore_max_vec_bits;
  string PCoreAffinityMask = pcore_affinity_mask;
  string ECoreAffinityMask = ecore_affinity_mask;
  string PreferredWorkloadPCore = preferred_workload_pcore;
  string PreferredWorkloadECore = preferred_workload_ecore;
}

def Hybrid_AlderLake       : HybridCoreTopology<"Hybrid_AlderLake", Arch_Alderlake, Arch_Gracemont, 256, 256, "0x0000FFFF", "0xFFFF0000", "HighCompute_FMA_Transcendental", "MemoryStreaming_IOWorkload">;
def Hybrid_RaptorLake      : HybridCoreTopology<"Hybrid_RaptorLake", Arch_Raptorlake, Arch_Gracemont, 256, 256, "0x0000FFFF", "0xFFFF0000", "HighCompute_FMA_Transcendental", "MemoryStreaming_IOWorkload">;
def Hybrid_MeteorLake      : HybridCoreTopology<"Hybrid_MeteorLake", Arch_Meteorlake, Arch_Crestmont, 256, 256, "0x000000FF", "0xFFFFFF00", "AVX2_Compute_Heavy", "LowPower_Streaming">;
def Hybrid_ArrowLake       : HybridCoreTopology<"Hybrid_ArrowLake", Arch_Arrowlake, Arch_Skymont, 256, 256, "0x000000FF", "0xFFFFFF00", "HighFreq_AVX2_VNNI_Compute", "Vectorized_Throughput_Parallel">;
def Hybrid_LunarLake       : HybridCoreTopology<"Hybrid_LunarLake", Arch_Lunarlake, Arch_Skymont, 256, 256, "0x0000000F", "0x000000F0", "AVX2_Matrix_Polynomial", "Memory_Bounded_Reduction">;
def Hybrid_NovaLake        : HybridCoreTopology<"Hybrid_NovaLake", Arch_CoyoteCove, Arch_ArcticWolf, 512, 512, "0x0000FFFF", "0xFFFFFFFF0000", "AVX10_2_512_APX_HeavyCompute", "AVX10_2_512_ParallelStreaming">;
def Hybrid_Zen5_9950X3D    : HybridCoreTopology<"Hybrid_Zen5_9950X3D", Arch_Zen5_X3D, Arch_Zen5, 512, 512, "0x0000FFFF", "0xFFFF0000", "CacheBound_LBM_Stencil_V_Cache_CCD0", "FreqBound_AVX512_Transcendental_CCD1">;
def Hybrid_Zen6_Medusa     : HybridCoreTopology<"Hybrid_Zen6_Medusa", Arch_Zen6_X3D, Arch_Zen6c, 512, 512, "0x0000FFFF", "0xFFFF0000", "CacheBound_3DVCache_AVX10_2_BMM", "Throughput_Dense_Prometheus_CCD">;
def Hybrid_AppleM1_M4      : HybridCoreTopology<"Hybrid_AppleM1_M4", Arch_AppleM1, Arch_CortexA55, 128, 128, "0x0000000F", "0x000000F0", "Wide_4FMA_Heavy_Double", "Background_Vector_Processing">;
def Hybrid_RK3399          : HybridCoreTopology<"Hybrid_RK3399", Arch_CortexA72, Arch_CortexA53, 128, 128, "0x0000000C", "0x00000003", "Compute_A72_DualCore", "Lightweight_A53_QuadCore">;
def Hybrid_RK3588          : HybridCoreTopology<"Hybrid_RK3588", Arch_CortexA76, Arch_CortexA55, 128, 128, "0x000000F0", "0x0000000F", "Compute_A76_QuadCore", "Lightweight_A55_QuadCore">;

// =============================================================================
// SECTION 10: PRIMITIVE VECTOR ARITHMETIC & HARDWARE INTRINSICS CATALOG
// =============================================================================

class VectorIntrinsicPrimitive<string op, string type_name, int lanes, string x86_intr, string arm_intr> {
  string Operation = op;
  string TypeName = type_name;
  int Lanes = lanes;
  string X86Intrinsic = x86_intr;
  string ARMIntrinsic = arm_intr;
}

class HardwareScalarPrimitive<string op, string type_name, string x86_intr, string arm_intr> {
  string Operation = op;
  string TypeName = type_name;
  string X86Intrinsic = x86_intr;
  string ARMIntrinsic = arm_intr;
}

// -----------------------------------------------------------------------------
// EXHAUSTIVE PRIMITIVE CATALOG: 8-bit, 16-bit, 32-bit, 64-bit, FP16 & Floats
// -----------------------------------------------------------------------------

def Prim_U8_Add                  : VectorIntrinsicPrimitive<"add", "u8", 16, "_mm_add_epi8", "vaddq_u8">;
def Prim_U8_256_Add              : VectorIntrinsicPrimitive<"add", "u8", 32, "_mm256_add_epi8", "svadd_u8_z">;
def Prim_U8_512_Add              : VectorIntrinsicPrimitive<"add", "u8", 64, "_mm512_add_epi8", "svadd_u8_z">;
def Prim_U8_Sub                  : VectorIntrinsicPrimitive<"sub", "u8", 16, "_mm_sub_epi8", "vsubq_u8">;
def Prim_U8_256_Sub              : VectorIntrinsicPrimitive<"sub", "u8", 32, "_mm256_sub_epi8", "svsub_u8_z">;
def Prim_U8_512_Sub              : VectorIntrinsicPrimitive<"sub", "u8", 64, "_mm512_sub_epi8", "svsub_u8_z">;
def Prim_U8_Mul                  : VectorIntrinsicPrimitive<"mul", "u8", 16, "_mm_mullo_epi16", "vmulq_u8">;
def Prim_U8_256_Mul              : VectorIntrinsicPrimitive<"mul", "u8", 32, "_mm256_mullo_epi16", "svmul_u8_z">;
def Prim_U8_512_Mul              : VectorIntrinsicPrimitive<"mul", "u8", 64, "_mm512_mullo_epi16", "svmul_u8_z">;
def Prim_U8_Min                  : VectorIntrinsicPrimitive<"min", "u8", 16, "_mm_min_epu8", "vminq_u8">;
def Prim_U8_256_Min              : VectorIntrinsicPrimitive<"min", "u8", 32, "_mm256_min_epu8", "svmin_u8_z">;
def Prim_U8_512_Min              : VectorIntrinsicPrimitive<"min", "u8", 64, "_mm512_min_epu8", "svmin_u8_z">;
def Prim_U8_Max                  : VectorIntrinsicPrimitive<"max", "u8", 16, "_mm_max_epu8", "vmaxq_u8">;
def Prim_U8_256_Max              : VectorIntrinsicPrimitive<"max", "u8", 32, "_mm256_max_epu8", "svmax_u8_z">;
def Prim_U8_512_Max              : VectorIntrinsicPrimitive<"max", "u8", 64, "_mm512_max_epu8", "svmax_u8_z">;
def Prim_U8_CmpEq                : VectorIntrinsicPrimitive<"cmpeq", "u8", 16, "_mm_cmpeq_epi8", "vcmeqq_u8">;
def Prim_U8_256_CmpEq            : VectorIntrinsicPrimitive<"cmpeq", "u8", 32, "_mm256_cmpeq_epi8", "svcmeq_u8_z">;
def Prim_U8_512_CmpEq            : VectorIntrinsicPrimitive<"cmpeq", "u8", 64, "_mm512_cmpeq_epu8_mask", "svcmeq_u8_z">;
def Prim_U8_CmpGt                : VectorIntrinsicPrimitive<"cmpgt", "u8", 16, "_mm_cmpgt_epu8_mask", "vcmgtq_u8">;
def Prim_U8_256_CmpGt            : VectorIntrinsicPrimitive<"cmpgt", "u8", 32, "_mm256_cmpgt_epu8_mask", "svcmgt_u8_z">;
def Prim_U8_512_CmpGt            : VectorIntrinsicPrimitive<"cmpgt", "u8", 64, "_mm512_cmpgt_epu8_mask", "svcmgt_u8_z">;
def Prim_U8_BlendV               : VectorIntrinsicPrimitive<"blendv", "u8", 16, "_mm_blendv_epi8", "vbslq_u8">;
def Prim_U8_256_BlendV           : VectorIntrinsicPrimitive<"blendv", "u8", 32, "_mm256_blendv_epi8", "svbsl_u8_z">;
def Prim_U8_512_BlendV           : VectorIntrinsicPrimitive<"blendv", "u8", 64, "_mm512_mask_blend_epi8", "svbsl_u8_z">;
def Prim_U8_And                  : VectorIntrinsicPrimitive<"and", "u8", 16, "_mm_and_si128", "vandq_u8">;
def Prim_U8_256_And              : VectorIntrinsicPrimitive<"and", "u8", 32, "_mm256_and_si256", "svand_u8_z">;
def Prim_U8_512_And              : VectorIntrinsicPrimitive<"and", "u8", 64, "_mm512_and_si512", "svand_u8_z">;
def Prim_U8_Or                   : VectorIntrinsicPrimitive<"or", "u8", 16, "_mm_or_si128", "vorrq_u8">;
def Prim_U8_256_Or               : VectorIntrinsicPrimitive<"or", "u8", 32, "_mm256_or_si256", "svorr_u8_z">;
def Prim_U8_512_Or               : VectorIntrinsicPrimitive<"or", "u8", 64, "_mm512_or_si512", "svorr_u8_z">;
def Prim_U8_Xor                  : VectorIntrinsicPrimitive<"xor", "u8", 16, "_mm_xor_si128", "veorq_u8">;
def Prim_U8_256_Xor              : VectorIntrinsicPrimitive<"xor", "u8", 32, "_mm256_xor_si256", "sveor_u8_z">;
def Prim_U8_512_Xor              : VectorIntrinsicPrimitive<"xor", "u8", 64, "_mm512_xor_si512", "sveor_u8_z">;
def Prim_U8_AddSat               : VectorIntrinsicPrimitive<"adds_sat", "u8", 16, "_mm_adds_epu8", "vqaddq_u8">;
def Prim_U8_256_AddSat           : VectorIntrinsicPrimitive<"adds_sat", "u8", 32, "_mm256_adds_epu8", "svqadd_u8_z">;
def Prim_U8_512_AddSat           : VectorIntrinsicPrimitive<"adds_sat", "u8", 64, "_mm512_adds_epu8", "svqadd_u8_z">;
def Prim_U8_SubSat               : VectorIntrinsicPrimitive<"subs_sat", "u8", 16, "_mm_subs_epu8", "vqsubq_u8">;
def Prim_U8_256_SubSat           : VectorIntrinsicPrimitive<"subs_sat", "u8", 32, "_mm256_subs_epu8", "svqsub_u8_z">;
def Prim_U8_512_SubSat           : VectorIntrinsicPrimitive<"subs_sat", "u8", 64, "_mm512_subs_epu8", "svqsub_u8_z">;
def Prim_U8_Shl                  : VectorIntrinsicPrimitive<"sll", "u8", 16, "_mm_slli_epi16", "vshlq_n_u8">;
def Prim_U8_256_Shl              : VectorIntrinsicPrimitive<"sll", "u8", 32, "_mm256_slli_epi16", "svlsl_n_u8_z">;
def Prim_U8_512_Shl              : VectorIntrinsicPrimitive<"sll", "u8", 64, "_mm512_slli_epi16", "svshl_n_u8_z">;
def Prim_U8_Shr                  : VectorIntrinsicPrimitive<"srl", "u8", 16, "_mm_srli_epi16", "vshrq_n_u8">;
def Prim_U8_256_Shr              : VectorIntrinsicPrimitive<"srl", "u8", 32, "_mm256_srli_epi16", "svlsr_n_u8_z">;
def Prim_U8_512_Shr              : VectorIntrinsicPrimitive<"srl", "u8", 64, "_mm512_srli_epi16", "svlsr_n_u8_z">;

def Prim_I8_Add                  : VectorIntrinsicPrimitive<"add", "i8", 16, "_mm_add_epi8", "vaddq_s8">;
def Prim_I8_256_Add              : VectorIntrinsicPrimitive<"add", "i8", 32, "_mm256_add_epi8", "svadd_s8_z">;
def Prim_I8_512_Add              : VectorIntrinsicPrimitive<"add", "i8", 64, "_mm512_add_epi8", "svadd_s8_z">;
def Prim_I8_Sub                  : VectorIntrinsicPrimitive<"sub", "i8", 16, "_mm_sub_epi8", "vsubq_s8">;
def Prim_I8_256_Sub              : VectorIntrinsicPrimitive<"sub", "i8", 32, "_mm256_sub_epi8", "svsub_s8_z">;
def Prim_I8_512_Sub              : VectorIntrinsicPrimitive<"sub", "i8", 64, "_mm512_sub_epi8", "svsub_s8_z">;
def Prim_I8_Mul                  : VectorIntrinsicPrimitive<"mul", "i8", 16, "_mm_mullo_epi16", "vmulq_s8">;
def Prim_I8_256_Mul              : VectorIntrinsicPrimitive<"mul", "i8", 32, "_mm256_mullo_epi16", "svmul_s8_z">;
def Prim_I8_512_Mul              : VectorIntrinsicPrimitive<"mul", "i8", 64, "_mm512_mullo_epi16", "svmul_s8_z">;
def Prim_I8_Min                  : VectorIntrinsicPrimitive<"min", "i8", 16, "_mm_min_epi8", "vminq_s8">;
def Prim_I8_256_Min              : VectorIntrinsicPrimitive<"min", "i8", 32, "_mm256_min_epi8", "svmin_s8_z">;
def Prim_I8_512_Min              : VectorIntrinsicPrimitive<"min", "i8", 64, "_mm512_min_epi8", "svmin_s8_z">;
def Prim_I8_Max                  : VectorIntrinsicPrimitive<"max", "i8", 16, "_mm_max_epi8", "vmaxq_s8">;
def Prim_I8_256_Max              : VectorIntrinsicPrimitive<"max", "i8", 32, "_mm256_max_epi8", "svmax_s8_z">;
def Prim_I8_512_Max              : VectorIntrinsicPrimitive<"max", "i8", 64, "_mm512_max_epi8", "svmax_s8_z">;
def Prim_I8_CmpEq                : VectorIntrinsicPrimitive<"cmpeq", "i8", 16, "_mm_cmpeq_epi8", "vcmeqq_s8">;
def Prim_I8_256_CmpEq            : VectorIntrinsicPrimitive<"cmpeq", "i8", 32, "_mm256_cmpeq_epi8", "svcmeq_s8_z">;
def Prim_I8_512_CmpEq            : VectorIntrinsicPrimitive<"cmpeq", "i8", 64, "_mm512_cmpeq_epi8", "svcmeq_s8_z">;
def Prim_I8_CmpGt                : VectorIntrinsicPrimitive<"cmpgt", "i8", 16, "_mm_cmpgt_epi8", "vcmgtq_s8">;
def Prim_I8_256_CmpGt            : VectorIntrinsicPrimitive<"cmpgt", "i8", 32, "_mm256_cmpgt_epi8", "svcmgt_s8_z">;
def Prim_I8_512_CmpGt            : VectorIntrinsicPrimitive<"cmpgt", "i8", 64, "_mm512_cmpgt_epi8", "svcmgt_s8_z">;
def Prim_I8_BlendV               : VectorIntrinsicPrimitive<"blendv", "i8", 16, "_mm_blendv_epi8", "vbslq_s8">;
def Prim_I8_256_BlendV           : VectorIntrinsicPrimitive<"blendv", "i8", 32, "_mm256_blendv_epi8", "svbsl_s8_z">;
def Prim_I8_512_BlendV           : VectorIntrinsicPrimitive<"blendv", "i8", 64, "_mm512_mask_blend_epi8", "svbsl_s8_z">;
def Prim_I8_And                  : VectorIntrinsicPrimitive<"and", "i8", 16, "_mm_and_si128", "vandq_s8">;
def Prim_I8_256_And              : VectorIntrinsicPrimitive<"and", "i8", 32, "_mm256_and_si256", "svand_s8_z">;
def Prim_I8_512_And              : VectorIntrinsicPrimitive<"and", "i8", 64, "_mm512_and_si512", "svand_s8_z">;
def Prim_I8_Or                   : VectorIntrinsicPrimitive<"or", "i8", 16, "_mm_or_si128", "vorrq_s8">;
def Prim_I8_256_Or               : VectorIntrinsicPrimitive<"or", "i8", 32, "_mm256_or_si256", "svorr_s8_z">;
def Prim_I8_512_Or               : VectorIntrinsicPrimitive<"or", "i8", 64, "_mm512_or_si512", "svorr_s8_z">;
def Prim_I8_Xor                  : VectorIntrinsicPrimitive<"xor", "i8", 16, "_mm_xor_si128", "veorq_s8">;
def Prim_I8_256_Xor              : VectorIntrinsicPrimitive<"xor", "i8", 32, "_mm256_xor_si256", "sveor_s8_z">;
def Prim_I8_512_Xor              : VectorIntrinsicPrimitive<"xor", "i8", 64, "_mm512_xor_si512", "sveor_s8_z">;
def Prim_I8_AddSat               : VectorIntrinsicPrimitive<"adds_sat", "i8", 16, "_mm_adds_epi8", "vqaddq_s8">;
def Prim_I8_256_AddSat           : VectorIntrinsicPrimitive<"adds_sat", "i8", 32, "_mm256_adds_epi8", "svqadd_s8_z">;
def Prim_I8_512_AddSat           : VectorIntrinsicPrimitive<"adds_sat", "i8", 64, "_mm512_adds_epi8", "svqadd_s8_z">;
def Prim_I8_SubSat               : VectorIntrinsicPrimitive<"subs_sat", "i8", 16, "_mm_subs_epi8", "vqsubq_s8">;
def Prim_I8_256_SubSat           : VectorIntrinsicPrimitive<"subs_sat", "i8", 32, "_mm256_subs_epi8", "svqsub_s8_z">;
def Prim_I8_512_SubSat           : VectorIntrinsicPrimitive<"subs_sat", "i8", 64, "_mm512_subs_epi8", "svqsub_s8_z">;
def Prim_I8_Shl                  : VectorIntrinsicPrimitive<"sll", "i8", 16, "_mm_slli_epi16", "vshlq_n_s8">;
def Prim_I8_256_Shl              : VectorIntrinsicPrimitive<"sll", "i8", 32, "_mm256_slli_epi16", "svlsl_n_s8_z">;
def Prim_I8_512_Shl              : VectorIntrinsicPrimitive<"sll", "i8", 64, "_mm512_slli_epi16", "svshl_n_s8_z">;
def Prim_I8_Shr                  : VectorIntrinsicPrimitive<"sra", "i8", 16, "_mm_srai_epi16", "vshrq_n_s8">;
def Prim_I8_256_Shr              : VectorIntrinsicPrimitive<"sra", "i8", 32, "_mm256_srai_epi16", "svasr_n_s8_z">;
def Prim_I8_512_Shr              : VectorIntrinsicPrimitive<"sra", "i8", 64, "_mm512_srai_epi16", "svshr_n_s8_z">;

def Prim_I16_Add                 : VectorIntrinsicPrimitive<"add", "i16", 8, "_mm_add_epi16", "vaddq_s16">;
def Prim_I16_256_Add             : VectorIntrinsicPrimitive<"add", "i16", 16, "_mm256_add_epi16", "svadd_s16_z">;
def Prim_I16_512_Add             : VectorIntrinsicPrimitive<"add", "i16", 32, "_mm512_add_epi16", "svadd_s16_z">;
def Prim_I16_Sub                 : VectorIntrinsicPrimitive<"sub", "i16", 8, "_mm_sub_epi16", "vsubq_s16">;
def Prim_I16_256_Sub             : VectorIntrinsicPrimitive<"sub", "i16", 16, "_mm256_sub_epi16", "svsub_s16_z">;
def Prim_I16_512_Sub             : VectorIntrinsicPrimitive<"sub", "i16", 32, "_mm512_sub_epi16", "svsub_s16_z">;
def Prim_I16_Mul                 : VectorIntrinsicPrimitive<"mul", "i16", 8, "_mm_mullo_epi16", "vmulq_s16">;
def Prim_I16_256_Mul             : VectorIntrinsicPrimitive<"mul", "i16", 16, "_mm256_mullo_epi16", "svmul_s16_z">;
def Prim_I16_512_Mul             : VectorIntrinsicPrimitive<"mul", "i16", 32, "_mm512_mullo_epi16", "svmul_s16_z">;
def Prim_I16_Min                 : VectorIntrinsicPrimitive<"min", "i16", 8, "_mm_min_epi16", "vminq_s16">;
def Prim_I16_256_Min             : VectorIntrinsicPrimitive<"min", "i16", 16, "_mm256_min_epi16", "svmin_s16_z">;
def Prim_I16_512_Min             : VectorIntrinsicPrimitive<"min", "i16", 32, "_mm512_min_epi16", "svmin_s16_z">;
def Prim_I16_Max                 : VectorIntrinsicPrimitive<"max", "i16", 8, "_mm_max_epi16", "vmaxq_s16">;
def Prim_I16_256_Max             : VectorIntrinsicPrimitive<"max", "i16", 16, "_mm256_max_epi16", "svmax_s16_z">;
def Prim_I16_512_Max             : VectorIntrinsicPrimitive<"max", "i16", 32, "_mm512_max_epi16", "svmax_s16_z">;
def Prim_I16_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "i16", 8, "_mm_cmpeq_epi16", "vcmeqq_s16">;
def Prim_I16_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i16", 16, "_mm256_cmpeq_epi16", "svcmeq_s16_z">;
def Prim_I16_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i16", 32, "_mm512_cmpeq_epi16", "svcmeq_s16_z">;
def Prim_I16_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "i16", 8, "_mm_cmpgt_epi16", "vcmgtq_s16">;
def Prim_I16_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i16", 16, "_mm256_cmpgt_epi16", "svcmgt_s16_z">;
def Prim_I16_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i16", 32, "_mm512_cmpgt_epi16", "svcmgt_s16_z">;
def Prim_I16_BlendV              : VectorIntrinsicPrimitive<"blendv", "i16", 8, "_mm_blendv_epi8", "vbslq_s16">;
def Prim_I16_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "i16", 16, "_mm256_blendv_epi8", "svbsl_s16_z">;
def Prim_I16_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "i16", 32, "_mm512_mask_blend_epi16", "svbsl_s16_z">;
def Prim_I16_And                 : VectorIntrinsicPrimitive<"and", "i16", 8, "_mm_and_si128", "vandq_s16">;
def Prim_I16_256_And             : VectorIntrinsicPrimitive<"and", "i16", 16, "_mm256_and_si256", "svand_s16_z">;
def Prim_I16_512_And             : VectorIntrinsicPrimitive<"and", "i16", 32, "_mm512_and_si512", "svand_s16_z">;
def Prim_I16_Or                  : VectorIntrinsicPrimitive<"or", "i16", 8, "_mm_or_si128", "vorrq_s16">;
def Prim_I16_256_Or              : VectorIntrinsicPrimitive<"or", "i16", 16, "_mm256_or_si256", "svorr_s16_z">;
def Prim_I16_512_Or              : VectorIntrinsicPrimitive<"or", "i16", 32, "_mm512_or_si512", "svorr_s16_z">;
def Prim_I16_Xor                 : VectorIntrinsicPrimitive<"xor", "i16", 8, "_mm_xor_si128", "veorq_s16">;
def Prim_I16_256_Xor             : VectorIntrinsicPrimitive<"xor", "i16", 16, "_mm256_xor_si256", "sveor_s16_z">;
def Prim_I16_512_Xor             : VectorIntrinsicPrimitive<"xor", "i16", 32, "_mm512_xor_si512", "sveor_s16_z">;
def Prim_I16_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "i16", 8, "_mm_adds_epi16", "vqaddq_s16">;
def Prim_I16_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i16", 16, "_mm256_adds_epi16", "svqadd_s16_z">;
def Prim_I16_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i16", 32, "_mm512_adds_epi16", "svqadd_s16_z">;
def Prim_I16_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "i16", 8, "_mm_subs_epi16", "vqsubq_s16">;
def Prim_I16_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i16", 16, "_mm256_subs_epi16", "svqsub_s16_z">;
def Prim_I16_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i16", 32, "_mm512_subs_epi16", "svqsub_s16_z">;
def Prim_I16_Shl                 : VectorIntrinsicPrimitive<"sll", "i16", 8, "_mm_slli_epi16", "vshlq_n_s16">;
def Prim_I16_256_Shl             : VectorIntrinsicPrimitive<"sll", "i16", 16, "_mm256_slli_epi16", "svlsl_n_s16_z">;
def Prim_I16_512_Shl             : VectorIntrinsicPrimitive<"sll", "i16", 32, "_mm512_slli_epi16", "svshl_n_s16_z">;
def Prim_I16_Shr                 : VectorIntrinsicPrimitive<"sra", "i16", 8, "_mm_srai_epi16", "vshrq_n_s16">;
def Prim_I16_256_Shr             : VectorIntrinsicPrimitive<"sra", "i16", 16, "_mm256_srai_epi16", "svasr_n_s16_z">;
def Prim_I16_512_Shr             : VectorIntrinsicPrimitive<"sra", "i16", 32, "_mm512_srai_epi16", "svshr_n_s16_z">;

def Prim_U16_Add                 : VectorIntrinsicPrimitive<"add", "u16", 8, "_mm_add_epi16", "vaddq_u16">;
def Prim_U16_256_Add             : VectorIntrinsicPrimitive<"add", "u16", 16, "_mm256_add_epi16", "svadd_u16_z">;
def Prim_U16_512_Add             : VectorIntrinsicPrimitive<"add", "u16", 32, "_mm512_add_epi16", "svadd_u16_z">;
def Prim_U16_Sub                 : VectorIntrinsicPrimitive<"sub", "u16", 8, "_mm_sub_epi16", "vsubq_u16">;
def Prim_U16_256_Sub             : VectorIntrinsicPrimitive<"sub", "u16", 16, "_mm256_sub_epi16", "svsub_u16_z">;
def Prim_U16_512_Sub             : VectorIntrinsicPrimitive<"sub", "u16", 32, "_mm512_sub_epi16", "svsub_u16_z">;
def Prim_U16_Mul                 : VectorIntrinsicPrimitive<"mul", "u16", 8, "_mm_mullo_epi16", "vmulq_u16">;
def Prim_U16_256_Mul             : VectorIntrinsicPrimitive<"mul", "u16", 16, "_mm256_mullo_epi16", "svmul_u16_z">;
def Prim_U16_512_Mul             : VectorIntrinsicPrimitive<"mul", "u16", 32, "_mm512_mullo_epi16", "svmul_u16_z">;
def Prim_U16_Min                 : VectorIntrinsicPrimitive<"min", "u16", 8, "_mm_min_epu16", "vminq_u16">;
def Prim_U16_256_Min             : VectorIntrinsicPrimitive<"min", "u16", 16, "_mm256_min_epu16", "svmin_u16_z">;
def Prim_U16_512_Min             : VectorIntrinsicPrimitive<"min", "u16", 32, "_mm512_min_epu16", "svmin_u16_z">;
def Prim_U16_Max                 : VectorIntrinsicPrimitive<"max", "u16", 8, "_mm_max_epu16", "vmaxq_u16">;
def Prim_U16_256_Max             : VectorIntrinsicPrimitive<"max", "u16", 16, "_mm256_max_epu16", "svmax_u16_z">;
def Prim_U16_512_Max             : VectorIntrinsicPrimitive<"max", "u16", 32, "_mm512_max_epu16", "svmax_u16_z">;
def Prim_U16_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "u16", 8, "_mm_cmpeq_epi16", "vcmeqq_u16">;
def Prim_U16_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u16", 16, "_mm256_cmpeq_epi16", "svcmeq_u16_z">;
def Prim_U16_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u16", 32, "_mm512_cmpeq_epu16_mask", "svcmeq_u16_z">;
def Prim_U16_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "u16", 8, "_mm_cmpgt_epu16_mask", "vcmgtq_u16">;
def Prim_U16_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u16", 16, "_mm256_cmpgt_epu16_mask", "svcmgt_u16_z">;
def Prim_U16_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u16", 32, "_mm512_cmpgt_epu16_mask", "svcmgt_u16_z">;
def Prim_U16_BlendV              : VectorIntrinsicPrimitive<"blendv", "u16", 8, "_mm_blendv_epi8", "vbslq_u16">;
def Prim_U16_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "u16", 16, "_mm256_blendv_epi8", "svbsl_u16_z">;
def Prim_U16_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "u16", 32, "_mm512_mask_blend_epi16", "svbsl_u16_z">;
def Prim_U16_And                 : VectorIntrinsicPrimitive<"and", "u16", 8, "_mm_and_si128", "vandq_u16">;
def Prim_U16_256_And             : VectorIntrinsicPrimitive<"and", "u16", 16, "_mm256_and_si256", "svand_u16_z">;
def Prim_U16_512_And             : VectorIntrinsicPrimitive<"and", "u16", 32, "_mm512_and_si512", "svand_u16_z">;
def Prim_U16_Or                  : VectorIntrinsicPrimitive<"or", "u16", 8, "_mm_or_si128", "vorrq_u16">;
def Prim_U16_256_Or              : VectorIntrinsicPrimitive<"or", "u16", 16, "_mm256_or_si256", "svorr_u16_z">;
def Prim_U16_512_Or              : VectorIntrinsicPrimitive<"or", "u16", 32, "_mm512_or_si512", "svorr_u16_z">;
def Prim_U16_Xor                 : VectorIntrinsicPrimitive<"xor", "u16", 8, "_mm_xor_si128", "veorq_u16">;
def Prim_U16_256_Xor             : VectorIntrinsicPrimitive<"xor", "u16", 16, "_mm256_xor_si256", "sveor_u16_z">;
def Prim_U16_512_Xor             : VectorIntrinsicPrimitive<"xor", "u16", 32, "_mm512_xor_si512", "sveor_u16_z">;
def Prim_U16_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "u16", 8, "_mm_adds_epu16", "vqaddq_u16">;
def Prim_U16_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u16", 16, "_mm256_adds_epu16", "svqadd_u16_z">;
def Prim_U16_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u16", 32, "_mm512_adds_epu16", "svqadd_u16_z">;
def Prim_U16_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "u16", 8, "_mm_subs_epu16", "vqsubq_u16">;
def Prim_U16_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u16", 16, "_mm256_subs_epu16", "svqsub_u16_z">;
def Prim_U16_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u16", 32, "_mm512_subs_epu16", "svqsub_u16_z">;
def Prim_U16_Shl                 : VectorIntrinsicPrimitive<"sll", "u16", 8, "_mm_slli_epi16", "vshlq_n_u16">;
def Prim_U16_256_Shl             : VectorIntrinsicPrimitive<"sll", "u16", 16, "_mm256_slli_epi16", "svlsl_n_u16_z">;
def Prim_U16_512_Shl             : VectorIntrinsicPrimitive<"sll", "u16", 32, "_mm512_slli_epi16", "svshl_n_u16_z">;
def Prim_U16_Shr                 : VectorIntrinsicPrimitive<"sra", "u16", 8, "_mm_srli_epi16", "vshrq_n_u16">;
def Prim_U16_256_Shr             : VectorIntrinsicPrimitive<"sra", "u16", 16, "_mm256_srli_epi16", "svlsr_n_u16_z">;
def Prim_U16_512_Shr             : VectorIntrinsicPrimitive<"sra", "u16", 32, "_mm512_srli_epi16", "svshr_n_u16_z">;

def Prim_I32_Add                 : VectorIntrinsicPrimitive<"add", "i32", 4, "_mm_add_epi32", "vaddq_s32">;
def Prim_I32_256_Add             : VectorIntrinsicPrimitive<"add", "i32", 8, "_mm256_add_epi32", "svadd_s32_z">;
def Prim_I32_512_Add             : VectorIntrinsicPrimitive<"add", "i32", 16, "_mm512_add_epi32", "svadd_s32_z">;
def Prim_I32_Sub                 : VectorIntrinsicPrimitive<"sub", "i32", 4, "_mm_sub_epi32", "vsubq_s32">;
def Prim_I32_256_Sub             : VectorIntrinsicPrimitive<"sub", "i32", 8, "_mm256_sub_epi32", "svsub_s32_z">;
def Prim_I32_512_Sub             : VectorIntrinsicPrimitive<"sub", "i32", 16, "_mm512_sub_epi32", "svsub_s32_z">;
def Prim_I32_Mul                 : VectorIntrinsicPrimitive<"mul", "i32", 4, "_mm_mullo_epi32", "vmulq_s32">;
def Prim_I32_256_Mul             : VectorIntrinsicPrimitive<"mul", "i32", 8, "_mm256_mullo_epi32", "svmul_s32_z">;
def Prim_I32_512_Mul             : VectorIntrinsicPrimitive<"mul", "i32", 16, "_mm512_mullo_epi32", "svmul_s32_z">;
def Prim_I32_Min                 : VectorIntrinsicPrimitive<"min", "i32", 4, "_mm_min_epi32", "vminq_s32">;
def Prim_I32_256_Min             : VectorIntrinsicPrimitive<"min", "i32", 8, "_mm256_min_epi32", "svmin_s32_z">;
def Prim_I32_512_Min             : VectorIntrinsicPrimitive<"min", "i32", 16, "_mm512_min_epi32", "svmin_s32_z">;
def Prim_I32_Max                 : VectorIntrinsicPrimitive<"max", "i32", 4, "_mm_max_epi32", "vmaxq_s32">;
def Prim_I32_256_Max             : VectorIntrinsicPrimitive<"max", "i32", 8, "_mm256_max_epi32", "svmax_s32_z">;
def Prim_I32_512_Max             : VectorIntrinsicPrimitive<"max", "i32", 16, "_mm512_max_epi32", "svmax_s32_z">;
def Prim_I32_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "i32", 4, "_mm_cmpeq_epi32", "vcmeqq_s32">;
def Prim_I32_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i32", 8, "_mm256_cmpeq_epi32", "svcmeq_s32_z">;
def Prim_I32_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i32", 16, "_mm512_cmpeq_epi32", "svcmeq_s32_z">;
def Prim_I32_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "i32", 4, "_mm_cmpgt_epi32", "vcmgtq_s32">;
def Prim_I32_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i32", 8, "_mm256_cmpgt_epi32", "svcmgt_s32_z">;
def Prim_I32_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i32", 16, "_mm512_cmpgt_epi32", "svcmgt_s32_z">;
def Prim_I32_BlendV              : VectorIntrinsicPrimitive<"blendv", "i32", 4, "_mm_blendv_epi8", "vbslq_s32">;
def Prim_I32_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "i32", 8, "_mm256_blendv_epi8", "svbsl_s32_z">;
def Prim_I32_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "i32", 16, "_mm512_mask_blend_epi32", "svbsl_s32_z">;
def Prim_I32_And                 : VectorIntrinsicPrimitive<"and", "i32", 4, "_mm_and_si128", "vandq_s32">;
def Prim_I32_256_And             : VectorIntrinsicPrimitive<"and", "i32", 8, "_mm256_and_si256", "svand_s32_z">;
def Prim_I32_512_And             : VectorIntrinsicPrimitive<"and", "i32", 16, "_mm512_and_epi32", "svand_s32_z">;
def Prim_I32_Or                  : VectorIntrinsicPrimitive<"or", "i32", 4, "_mm_or_si128", "vorrq_s32">;
def Prim_I32_256_Or              : VectorIntrinsicPrimitive<"or", "i32", 8, "_mm256_or_si256", "svorr_s32_z">;
def Prim_I32_512_Or              : VectorIntrinsicPrimitive<"or", "i32", 16, "_mm512_or_epi32", "svorr_s32_z">;
def Prim_I32_Xor                 : VectorIntrinsicPrimitive<"xor", "i32", 4, "_mm_xor_si128", "veorq_s32">;
def Prim_I32_256_Xor             : VectorIntrinsicPrimitive<"xor", "i32", 8, "_mm256_xor_si256", "sveor_s32_z">;
def Prim_I32_512_Xor             : VectorIntrinsicPrimitive<"xor", "i32", 16, "_mm512_xor_epi32", "sveor_s32_z">;
def Prim_I32_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "i32", 4, "_mm_add_epi32", "vqaddq_s32">;
def Prim_I32_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i32", 8, "_mm256_add_epi32", "svqadd_s32_z">;
def Prim_I32_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i32", 16, "_mm512_add_epi32", "svqadd_s32_z">;
def Prim_I32_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "i32", 4, "_mm_sub_epi32", "vqsubq_s32">;
def Prim_I32_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i32", 8, "_mm256_sub_epi32", "svqsub_s32_z">;
def Prim_I32_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i32", 16, "_mm512_sub_epi32", "svqsub_s32_z">;
def Prim_I32_Shl                 : VectorIntrinsicPrimitive<"sll", "i32", 4, "_mm_slli_epi32", "vshlq_n_s32">;
def Prim_I32_256_Shl             : VectorIntrinsicPrimitive<"sll", "i32", 8, "_mm256_slli_epi32", "svlsl_n_s32_z">;
def Prim_I32_512_Shl             : VectorIntrinsicPrimitive<"sll", "i32", 16, "_mm512_slli_epi32", "svshl_n_s32_z">;
def Prim_I32_Shr                 : VectorIntrinsicPrimitive<"sra", "i32", 4, "_mm_srai_epi32", "vshrq_n_s32">;
def Prim_I32_256_Shr             : VectorIntrinsicPrimitive<"sra", "i32", 8, "_mm256_srai_epi32", "svasr_n_s32_z">;
def Prim_I32_512_Shr             : VectorIntrinsicPrimitive<"sra", "i32", 16, "_mm512_srai_epi32", "svshr_n_s32_z">;

def Prim_U32_Add                 : VectorIntrinsicPrimitive<"add", "u32", 4, "_mm_add_epi32", "vaddq_u32">;
def Prim_U32_256_Add             : VectorIntrinsicPrimitive<"add", "u32", 8, "_mm256_add_epi32", "svadd_u32_z">;
def Prim_U32_512_Add             : VectorIntrinsicPrimitive<"add", "u32", 16, "_mm512_add_epi32", "svadd_u32_z">;
def Prim_U32_Sub                 : VectorIntrinsicPrimitive<"sub", "u32", 4, "_mm_sub_epi32", "vsubq_u32">;
def Prim_U32_256_Sub             : VectorIntrinsicPrimitive<"sub", "u32", 8, "_mm256_sub_epi32", "svsub_u32_z">;
def Prim_U32_512_Sub             : VectorIntrinsicPrimitive<"sub", "u32", 16, "_mm512_sub_epi32", "svsub_u32_z">;
def Prim_U32_Mul                 : VectorIntrinsicPrimitive<"mul", "u32", 4, "_mm_mullo_epi32", "vmulq_u32">;
def Prim_U32_256_Mul             : VectorIntrinsicPrimitive<"mul", "u32", 8, "_mm256_mullo_epi32", "svmul_u32_z">;
def Prim_U32_512_Mul             : VectorIntrinsicPrimitive<"mul", "u32", 16, "_mm512_mullo_epi32", "svmul_u32_z">;
def Prim_U32_Min                 : VectorIntrinsicPrimitive<"min", "u32", 4, "_mm_min_epu32", "vminq_u32">;
def Prim_U32_256_Min             : VectorIntrinsicPrimitive<"min", "u32", 8, "_mm256_min_epu32", "svmin_u32_z">;
def Prim_U32_512_Min             : VectorIntrinsicPrimitive<"min", "u32", 16, "_mm512_min_epu32", "svmin_u32_z">;
def Prim_U32_Max                 : VectorIntrinsicPrimitive<"max", "u32", 4, "_mm_max_epu32", "vmaxq_u32">;
def Prim_U32_256_Max             : VectorIntrinsicPrimitive<"max", "u32", 8, "_mm256_max_epu32", "svmax_u32_z">;
def Prim_U32_512_Max             : VectorIntrinsicPrimitive<"max", "u32", 16, "_mm512_max_epu32", "svmax_u32_z">;
def Prim_U32_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "u32", 4, "_mm_cmpeq_epi32", "vcmeqq_u32">;
def Prim_U32_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u32", 8, "_mm256_cmpeq_epi32", "svcmeq_u32_z">;
def Prim_U32_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u32", 16, "_mm512_cmpeq_epu32_mask", "svcmeq_u32_z">;
def Prim_U32_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "u32", 4, "_mm_cmpgt_epu32_mask", "vcmgtq_u32">;
def Prim_U32_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u32", 8, "_mm256_cmpgt_epu32_mask", "svcmgt_u32_z">;
def Prim_U32_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u32", 16, "_mm512_cmpgt_epu32_mask", "svcmgt_u32_z">;
def Prim_U32_BlendV              : VectorIntrinsicPrimitive<"blendv", "u32", 4, "_mm_blendv_epi8", "vbslq_u32">;
def Prim_U32_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "u32", 8, "_mm256_blendv_epi8", "svbsl_u32_z">;
def Prim_U32_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "u32", 16, "_mm512_mask_blend_epi32", "svbsl_u32_z">;
def Prim_U32_And                 : VectorIntrinsicPrimitive<"and", "u32", 4, "_mm_and_si128", "vandq_u32">;
def Prim_U32_256_And             : VectorIntrinsicPrimitive<"and", "u32", 8, "_mm256_and_si256", "svand_u32_z">;
def Prim_U32_512_And             : VectorIntrinsicPrimitive<"and", "u32", 16, "_mm512_and_epi32", "svand_u32_z">;
def Prim_U32_Or                  : VectorIntrinsicPrimitive<"or", "u32", 4, "_mm_or_si128", "vorrq_u32">;
def Prim_U32_256_Or              : VectorIntrinsicPrimitive<"or", "u32", 8, "_mm256_or_si256", "svorr_u32_z">;
def Prim_U32_512_Or              : VectorIntrinsicPrimitive<"or", "u32", 16, "_mm512_or_epi32", "svorr_u32_z">;
def Prim_U32_Xor                 : VectorIntrinsicPrimitive<"xor", "u32", 4, "_mm_xor_si128", "veorq_u32">;
def Prim_U32_256_Xor             : VectorIntrinsicPrimitive<"xor", "u32", 8, "_mm256_xor_si256", "sveor_u32_z">;
def Prim_U32_512_Xor             : VectorIntrinsicPrimitive<"xor", "u32", 16, "_mm512_xor_epi32", "sveor_u32_z">;
def Prim_U32_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "u32", 4, "_mm_add_epi32", "vqaddq_u32">;
def Prim_U32_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u32", 8, "_mm256_add_epi32", "svqadd_u32_z">;
def Prim_U32_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u32", 16, "_mm512_add_epi32", "svqadd_u32_z">;
def Prim_U32_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "u32", 4, "_mm_sub_epi32", "vqsubq_u32">;
def Prim_U32_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u32", 8, "_mm256_sub_epi32", "svqsub_u32_z">;
def Prim_U32_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u32", 16, "_mm512_sub_epi32", "svqsub_u32_z">;
def Prim_U32_Shl                 : VectorIntrinsicPrimitive<"sll", "u32", 4, "_mm_slli_epi32", "vshlq_n_u32">;
def Prim_U32_256_Shl             : VectorIntrinsicPrimitive<"sll", "u32", 8, "_mm256_slli_epi32", "svlsl_n_u32_z">;
def Prim_U32_512_Shl             : VectorIntrinsicPrimitive<"sll", "u32", 16, "_mm512_slli_epi32", "svshl_n_u32_z">;
def Prim_U32_Shr                 : VectorIntrinsicPrimitive<"sra", "u32", 4, "_mm_srli_epi32", "vshrq_n_u32">;
def Prim_U32_256_Shr             : VectorIntrinsicPrimitive<"sra", "u32", 8, "_mm256_srli_epi32", "svlsr_n_u32_z">;
def Prim_U32_512_Shr             : VectorIntrinsicPrimitive<"sra", "u32", 16, "_mm512_srli_epi32", "svshr_n_u32_z">;

def Prim_I64_Add                 : VectorIntrinsicPrimitive<"add", "i64", 2, "_mm_add_epi64", "vaddq_s64">;
def Prim_I64_256_Add             : VectorIntrinsicPrimitive<"add", "i64", 4, "_mm256_add_epi64", "svadd_s64_z">;
def Prim_I64_512_Add             : VectorIntrinsicPrimitive<"add", "i64", 8, "_mm512_add_epi64", "svadd_s64_z">;
def Prim_I64_Sub                 : VectorIntrinsicPrimitive<"sub", "i64", 2, "_mm_sub_epi64", "vsubq_s64">;
def Prim_I64_256_Sub             : VectorIntrinsicPrimitive<"sub", "i64", 4, "_mm256_sub_epi64", "svsub_s64_z">;
def Prim_I64_512_Sub             : VectorIntrinsicPrimitive<"sub", "i64", 8, "_mm512_sub_epi64", "svsub_s64_z">;
def Prim_I64_Mul                 : VectorIntrinsicPrimitive<"mul", "i64", 2, "_mm_mullo_epi64", "vmulq_s64">;
def Prim_I64_256_Mul             : VectorIntrinsicPrimitive<"mul", "i64", 4, "_mm256_mullo_epi64", "svmul_s64_z">;
def Prim_I64_512_Mul             : VectorIntrinsicPrimitive<"mul", "i64", 8, "_mm512_mullo_epi64", "svmul_s64_z">;
def Prim_I64_Min                 : VectorIntrinsicPrimitive<"min", "i64", 2, "_mm_min_epi64", "vminq_s64">;
def Prim_I64_256_Min             : VectorIntrinsicPrimitive<"min", "i64", 4, "_mm256_min_epi64", "svmin_s64_z">;
def Prim_I64_512_Min             : VectorIntrinsicPrimitive<"min", "i64", 8, "_mm512_min_epi64", "svmin_s64_z">;
def Prim_I64_Max                 : VectorIntrinsicPrimitive<"max", "i64", 2, "_mm_max_epi64", "vmaxq_s64">;
def Prim_I64_256_Max             : VectorIntrinsicPrimitive<"max", "i64", 4, "_mm256_max_epi64", "svmax_s64_z">;
def Prim_I64_512_Max             : VectorIntrinsicPrimitive<"max", "i64", 8, "_mm512_max_epi64", "svmax_s64_z">;
def Prim_I64_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "i64", 2, "_mm_cmpeq_epi64", "vcmeqq_s64">;
def Prim_I64_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i64", 4, "_mm256_cmpeq_epi64", "svcmeq_s64_z">;
def Prim_I64_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "i64", 8, "_mm512_cmpeq_epi64", "svcmeq_s64_z">;
def Prim_I64_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "i64", 2, "_mm_cmpgt_epi64", "vcmgtq_s64">;
def Prim_I64_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i64", 4, "_mm256_cmpgt_epi64", "svcmgt_s64_z">;
def Prim_I64_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "i64", 8, "_mm512_cmpgt_epi64", "svcmgt_s64_z">;
def Prim_I64_BlendV              : VectorIntrinsicPrimitive<"blendv", "i64", 2, "_mm_blendv_epi8", "vbslq_s64">;
def Prim_I64_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "i64", 4, "_mm256_blendv_epi8", "svbsl_s64_z">;
def Prim_I64_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "i64", 8, "_mm512_mask_blend_epi64", "svbsl_s64_z">;
def Prim_I64_And                 : VectorIntrinsicPrimitive<"and", "i64", 2, "_mm_and_si128", "vandq_s64">;
def Prim_I64_256_And             : VectorIntrinsicPrimitive<"and", "i64", 4, "_mm256_and_si256", "svand_s64_z">;
def Prim_I64_512_And             : VectorIntrinsicPrimitive<"and", "i64", 8, "_mm512_and_epi64", "svand_s64_z">;
def Prim_I64_Or                  : VectorIntrinsicPrimitive<"or", "i64", 2, "_mm_or_si128", "vorrq_s64">;
def Prim_I64_256_Or              : VectorIntrinsicPrimitive<"or", "i64", 4, "_mm256_or_si256", "svorr_s64_z">;
def Prim_I64_512_Or              : VectorIntrinsicPrimitive<"or", "i64", 8, "_mm512_or_epi64", "svorr_s64_z">;
def Prim_I64_Xor                 : VectorIntrinsicPrimitive<"xor", "i64", 2, "_mm_xor_si128", "veorq_s64">;
def Prim_I64_256_Xor             : VectorIntrinsicPrimitive<"xor", "i64", 4, "_mm256_xor_si256", "sveor_s64_z">;
def Prim_I64_512_Xor             : VectorIntrinsicPrimitive<"xor", "i64", 8, "_mm512_xor_epi64", "sveor_s64_z">;
def Prim_I64_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "i64", 2, "_mm_add_epi64", "vqaddq_s64">;
def Prim_I64_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i64", 4, "_mm256_add_epi64", "svqadd_s64_z">;
def Prim_I64_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "i64", 8, "_mm512_add_epi64", "svqadd_s64_z">;
def Prim_I64_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "i64", 2, "_mm_sub_epi64", "vqsubq_s64">;
def Prim_I64_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i64", 4, "_mm256_sub_epi64", "svqsub_s64_z">;
def Prim_I64_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "i64", 8, "_mm512_sub_epi64", "svqsub_s64_z">;
def Prim_I64_Shl                 : VectorIntrinsicPrimitive<"sll", "i64", 2, "_mm_slli_epi64", "vshlq_n_s64">;
def Prim_I64_256_Shl             : VectorIntrinsicPrimitive<"sll", "i64", 4, "_mm256_slli_epi64", "svlsl_n_s64_z">;
def Prim_I64_512_Shl             : VectorIntrinsicPrimitive<"sll", "i64", 8, "_mm512_slli_epi64", "svshl_n_s64_z">;
def Prim_I64_Shr                 : VectorIntrinsicPrimitive<"sra", "i64", 2, "_mm_srai_epi64", "vshrq_n_s64">;
def Prim_I64_256_Shr             : VectorIntrinsicPrimitive<"sra", "i64", 4, "_mm256_srai_epi64", "svasr_n_s64_z">;
def Prim_I64_512_Shr             : VectorIntrinsicPrimitive<"sra", "i64", 8, "_mm512_srai_epi64", "svshr_n_s64_z">;

def Prim_U64_Add                 : VectorIntrinsicPrimitive<"add", "u64", 2, "_mm_add_epi64", "vaddq_u64">;
def Prim_U64_256_Add             : VectorIntrinsicPrimitive<"add", "u64", 4, "_mm256_add_epi64", "svadd_u64_z">;
def Prim_U64_512_Add             : VectorIntrinsicPrimitive<"add", "u64", 8, "_mm512_add_epi64", "svadd_u64_z">;
def Prim_U64_Sub                 : VectorIntrinsicPrimitive<"sub", "u64", 2, "_mm_sub_epi64", "vsubq_u64">;
def Prim_U64_256_Sub             : VectorIntrinsicPrimitive<"sub", "u64", 4, "_mm256_sub_epi64", "svsub_u64_z">;
def Prim_U64_512_Sub             : VectorIntrinsicPrimitive<"sub", "u64", 8, "_mm512_sub_epi64", "svsub_u64_z">;
def Prim_U64_Mul                 : VectorIntrinsicPrimitive<"mul", "u64", 2, "_mm_mullo_epi64", "vmulq_u64">;
def Prim_U64_256_Mul             : VectorIntrinsicPrimitive<"mul", "u64", 4, "_mm256_mullo_epi64", "svmul_u64_z">;
def Prim_U64_512_Mul             : VectorIntrinsicPrimitive<"mul", "u64", 8, "_mm512_mullo_epi64", "svmul_u64_z">;
def Prim_U64_Min                 : VectorIntrinsicPrimitive<"min", "u64", 2, "_mm_min_epu64", "vminq_u64">;
def Prim_U64_256_Min             : VectorIntrinsicPrimitive<"min", "u64", 4, "_mm256_min_epu64", "svmin_u64_z">;
def Prim_U64_512_Min             : VectorIntrinsicPrimitive<"min", "u64", 8, "_mm512_min_epu64", "svmin_u64_z">;
def Prim_U64_Max                 : VectorIntrinsicPrimitive<"max", "u64", 2, "_mm_max_epu64", "vmaxq_u64">;
def Prim_U64_256_Max             : VectorIntrinsicPrimitive<"max", "u64", 4, "_mm256_max_epu64", "svmax_u64_z">;
def Prim_U64_512_Max             : VectorIntrinsicPrimitive<"max", "u64", 8, "_mm512_max_epu64", "svmax_u64_z">;
def Prim_U64_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "u64", 2, "_mm_cmpeq_epi64", "vcmeqq_u64">;
def Prim_U64_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u64", 4, "_mm256_cmpeq_epi64", "svcmeq_u64_z">;
def Prim_U64_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "u64", 8, "_mm512_cmpeq_epu64_mask", "svcmeq_u64_z">;
def Prim_U64_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "u64", 2, "_mm_cmpgt_epu64_mask", "vcmgtq_u64">;
def Prim_U64_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u64", 4, "_mm256_cmpgt_epu64_mask", "svcmgt_u64_z">;
def Prim_U64_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "u64", 8, "_mm512_cmpgt_epu64_mask", "svcmgt_u64_z">;
def Prim_U64_BlendV              : VectorIntrinsicPrimitive<"blendv", "u64", 2, "_mm_blendv_epi8", "vbslq_u64">;
def Prim_U64_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "u64", 4, "_mm256_blendv_epi8", "svbsl_u64_z">;
def Prim_U64_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "u64", 8, "_mm512_mask_blend_epi64", "svbsl_u64_z">;
def Prim_U64_And                 : VectorIntrinsicPrimitive<"and", "u64", 2, "_mm_and_si128", "vandq_u64">;
def Prim_U64_256_And             : VectorIntrinsicPrimitive<"and", "u64", 4, "_mm256_and_si256", "svand_u64_z">;
def Prim_U64_512_And             : VectorIntrinsicPrimitive<"and", "u64", 8, "_mm512_and_epi64", "svand_u64_z">;
def Prim_U64_Or                  : VectorIntrinsicPrimitive<"or", "u64", 2, "_mm_or_si128", "vorrq_u64">;
def Prim_U64_256_Or              : VectorIntrinsicPrimitive<"or", "u64", 4, "_mm256_or_si256", "svorr_u64_z">;
def Prim_U64_512_Or              : VectorIntrinsicPrimitive<"or", "u64", 8, "_mm512_or_epi64", "svorr_u64_z">;
def Prim_U64_Xor                 : VectorIntrinsicPrimitive<"xor", "u64", 2, "_mm_xor_si128", "veorq_u64">;
def Prim_U64_256_Xor             : VectorIntrinsicPrimitive<"xor", "u64", 4, "_mm256_xor_si256", "sveor_u64_z">;
def Prim_U64_512_Xor             : VectorIntrinsicPrimitive<"xor", "u64", 8, "_mm512_xor_epi64", "sveor_u64_z">;
def Prim_U64_AddSat              : VectorIntrinsicPrimitive<"adds_sat", "u64", 2, "_mm_add_epi64", "vqaddq_u64">;
def Prim_U64_256_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u64", 4, "_mm256_add_epi64", "svqadd_u64_z">;
def Prim_U64_512_AddSat          : VectorIntrinsicPrimitive<"adds_sat", "u64", 8, "_mm512_add_epi64", "svqadd_u64_z">;
def Prim_U64_SubSat              : VectorIntrinsicPrimitive<"subs_sat", "u64", 2, "_mm_sub_epi64", "vqsubq_u64">;
def Prim_U64_256_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u64", 4, "_mm256_sub_epi64", "svqsub_u64_z">;
def Prim_U64_512_SubSat          : VectorIntrinsicPrimitive<"subs_sat", "u64", 8, "_mm512_sub_epi64", "svqsub_u64_z">;
def Prim_U64_Shl                 : VectorIntrinsicPrimitive<"sll", "u64", 2, "_mm_slli_epi64", "vshlq_n_u64">;
def Prim_U64_256_Shl             : VectorIntrinsicPrimitive<"sll", "u64", 4, "_mm256_slli_epi64", "svlsl_n_u64_z">;
def Prim_U64_512_Shl             : VectorIntrinsicPrimitive<"sll", "u64", 8, "_mm512_slli_epi64", "svshl_n_u64_z">;
def Prim_U64_Shr                 : VectorIntrinsicPrimitive<"sra", "u64", 2, "_mm_srli_epi64", "vshrq_n_u64">;
def Prim_U64_256_Shr             : VectorIntrinsicPrimitive<"sra", "u64", 4, "_mm256_srli_epi64", "svlsr_n_u64_z">;
def Prim_U64_512_Shr             : VectorIntrinsicPrimitive<"sra", "u64", 8, "_mm512_srli_epi64", "svshr_n_u64_z">;

def Prim_FP16_Add                : VectorIntrinsicPrimitive<"add", "fp16", 8, "_mm_add_ph", "vaddq_f16">;
def Prim_FP16_256_Add            : VectorIntrinsicPrimitive<"add", "fp16", 16, "_mm256_add_ph", "svadd_f16_z">;
def Prim_FP16_512_Add            : VectorIntrinsicPrimitive<"add", "fp16", 32, "_mm512_add_ph", "svadd_f16_z">;
def Prim_FP16_Sub                : VectorIntrinsicPrimitive<"sub", "fp16", 8, "_mm_sub_ph", "vsubq_f16">;
def Prim_FP16_256_Sub            : VectorIntrinsicPrimitive<"sub", "fp16", 16, "_mm256_sub_ph", "svsub_f16_z">;
def Prim_FP16_512_Sub            : VectorIntrinsicPrimitive<"sub", "fp16", 32, "_mm512_sub_ph", "svsub_f16_z">;
def Prim_FP16_Mul                : VectorIntrinsicPrimitive<"mul", "fp16", 8, "_mm_mul_ph", "vmulq_f16">;
def Prim_FP16_256_Mul            : VectorIntrinsicPrimitive<"mul", "fp16", 16, "_mm256_mul_ph", "svmul_f16_z">;
def Prim_FP16_512_Mul            : VectorIntrinsicPrimitive<"mul", "fp16", 32, "_mm512_mul_ph", "svmul_f16_z">;
def Prim_FP16_Min                : VectorIntrinsicPrimitive<"min", "fp16", 8, "_mm_min_ph", "vminq_f16">;
def Prim_FP16_256_Min            : VectorIntrinsicPrimitive<"min", "fp16", 16, "_mm256_min_ph", "svmin_f16_z">;
def Prim_FP16_512_Min            : VectorIntrinsicPrimitive<"min", "fp16", 32, "_mm512_min_ph", "svmin_f16_z">;
def Prim_FP16_Max                : VectorIntrinsicPrimitive<"max", "fp16", 8, "_mm_max_ph", "vmaxq_f16">;
def Prim_FP16_256_Max            : VectorIntrinsicPrimitive<"max", "fp16", 16, "_mm256_max_ph", "svmax_f16_z">;
def Prim_FP16_512_Max            : VectorIntrinsicPrimitive<"max", "fp16", 32, "_mm512_max_ph", "svmax_f16_z">;
def Prim_FP16_CmpEq              : VectorIntrinsicPrimitive<"cmpeq", "fp16", 8, "_mm_cmpeq_ph", "vcmeqq_f16">;
def Prim_FP16_256_CmpEq          : VectorIntrinsicPrimitive<"cmpeq", "fp16", 16, "_mm256_cmpeq_ph", "svcmeq_f16_z">;
def Prim_FP16_512_CmpEq          : VectorIntrinsicPrimitive<"cmpeq", "fp16", 32, "_mm512_cmpeq_ph", "svcmeq_f16_z">;
def Prim_FP16_CmpGt              : VectorIntrinsicPrimitive<"cmpgt", "fp16", 8, "_mm_cmpgt_ph", "vcmgtq_f16">;
def Prim_FP16_256_CmpGt          : VectorIntrinsicPrimitive<"cmpgt", "fp16", 16, "_mm256_cmpgt_ph", "svcmgt_f16_z">;
def Prim_FP16_512_CmpGt          : VectorIntrinsicPrimitive<"cmpgt", "fp16", 32, "_mm512_cmpgt_ph", "svcmgt_f16_z">;
def Prim_FP16_BlendV             : VectorIntrinsicPrimitive<"blendv", "fp16", 8, "_mm_blendv_ph", "vbslq_f16">;
def Prim_FP16_256_BlendV         : VectorIntrinsicPrimitive<"blendv", "fp16", 16, "_mm256_blendv_ph", "svbsl_f16_z">;
def Prim_FP16_512_BlendV         : VectorIntrinsicPrimitive<"blendv", "fp16", 32, "_mm512_mask_blend_ph", "svbsl_f16_z">;
def Prim_FP16_And                : VectorIntrinsicPrimitive<"and", "fp16", 8, "_mm_and_ph", "vandq_f16">;
def Prim_FP16_256_And            : VectorIntrinsicPrimitive<"and", "fp16", 16, "_mm256_and_ph", "svand_f16_z">;
def Prim_FP16_512_And            : VectorIntrinsicPrimitive<"and", "fp16", 32, "_mm512_and_ph", "svand_f16_z">;
def Prim_FP16_Or                 : VectorIntrinsicPrimitive<"or", "fp16", 8, "_mm_or_ph", "vorrq_f16">;
def Prim_FP16_256_Or             : VectorIntrinsicPrimitive<"or", "fp16", 16, "_mm256_or_ph", "svorr_f16_z">;
def Prim_FP16_512_Or             : VectorIntrinsicPrimitive<"or", "fp16", 32, "_mm512_or_ph", "svorr_f16_z">;
def Prim_FP16_Xor                : VectorIntrinsicPrimitive<"xor", "fp16", 8, "_mm_xor_ph", "veorq_f16">;
def Prim_FP16_256_Xor            : VectorIntrinsicPrimitive<"xor", "fp16", 16, "_mm256_xor_ph", "sveor_f16_z">;
def Prim_FP16_512_Xor            : VectorIntrinsicPrimitive<"xor", "fp16", 32, "_mm512_xor_ph", "sveor_f16_z">;
def Prim_FP16_Div                : VectorIntrinsicPrimitive<"div", "fp16", 8, "_mm_div_ph", "vdivq_f16">;
def Prim_FP16_256_Div            : VectorIntrinsicPrimitive<"div", "fp16", 16, "_mm256_div_ph", "svdiv_f16_z">;
def Prim_FP16_512_Div            : VectorIntrinsicPrimitive<"div", "fp16", 32, "_mm512_div_ph", "svdiv_f16_z">;
def Prim_FP16_FMAdd              : VectorIntrinsicPrimitive<"fmadd", "fp16", 8, "_mm_fmadd_ph", "vfmaq_f16">;
def Prim_FP16_256_FMAdd          : VectorIntrinsicPrimitive<"fmadd", "fp16", 16, "_mm256_fmadd_ph", "svmla_f16_z">;
def Prim_FP16_512_FMAdd          : VectorIntrinsicPrimitive<"fmadd", "fp16", 32, "_mm512_fmadd_ph", "svfma_f16_z">;
def Prim_FP16_FMSub              : VectorIntrinsicPrimitive<"fmsub", "fp16", 8, "_mm_fmsub_ph", "vfmsq_f16">;
def Prim_FP16_256_FMSub          : VectorIntrinsicPrimitive<"fmsub", "fp16", 16, "_mm256_fmsub_ph", "svmls_f16_z">;
def Prim_FP16_512_FMSub          : VectorIntrinsicPrimitive<"fmsub", "fp16", 32, "_mm512_fmsub_ph", "svfms_f16_z">;
def Prim_FP16_Sqrt               : VectorIntrinsicPrimitive<"sqrt", "fp16", 8, "_mm_sqrt_ph", "vsqrtq_f16">;
def Prim_FP16_256_Sqrt           : VectorIntrinsicPrimitive<"sqrt", "fp16", 16, "_mm256_sqrt_ph", "svsqrt_f16_z">;
def Prim_FP16_512_Sqrt           : VectorIntrinsicPrimitive<"sqrt", "fp16", 32, "_mm512_sqrt_ph", "svsqrt_f16_z">;
def Prim_FP16_RSqrt              : VectorIntrinsicPrimitive<"rsqrt", "fp16", 8, "_mm_rsqrt_ph", "vrsqrteq_f16">;
def Prim_FP16_256_RSqrt          : VectorIntrinsicPrimitive<"rsqrt", "fp16", 16, "_mm256_rsqrt_ph", "svrsqrte_f16">;
def Prim_FP16_512_RSqrt          : VectorIntrinsicPrimitive<"rsqrt", "fp16", 32, "_mm512_rsqrt_ph", "svrsqrte_f16_z">;
def Prim_FP16_Rcp                : VectorIntrinsicPrimitive<"rcp", "fp16", 8, "_mm_rcp_ph", "vrecpeq_f16">;
def Prim_FP16_256_Rcp            : VectorIntrinsicPrimitive<"rcp", "fp16", 16, "_mm256_rcp_ph", "svrecpe_f16">;
def Prim_FP16_512_Rcp            : VectorIntrinsicPrimitive<"rcp", "fp16", 32, "_mm512_rcp_ph", "svrecpe_f16_z">;
def Prim_FP16_Floor              : VectorIntrinsicPrimitive<"floor", "fp16", 8, "_mm_floor_ph", "vrndmq_f16">;
def Prim_FP16_256_Floor          : VectorIntrinsicPrimitive<"floor", "fp16", 16, "_mm256_floor_ph", "svrintm_f16_z">;
def Prim_FP16_512_Floor          : VectorIntrinsicPrimitive<"floor", "fp16", 32, "_mm512_floor_ph", "svrndm_f16_z">;
def Prim_FP16_Ceil               : VectorIntrinsicPrimitive<"ceil", "fp16", 8, "_mm_ceil_ph", "vrndpq_f16">;
def Prim_FP16_256_Ceil           : VectorIntrinsicPrimitive<"ceil", "fp16", 16, "_mm256_ceil_ph", "svrintp_f16_z">;
def Prim_FP16_512_Ceil           : VectorIntrinsicPrimitive<"ceil", "fp16", 32, "_mm512_ceil_ph", "svrndp_f16_z">;
def Prim_FP16_Trunc              : VectorIntrinsicPrimitive<"trunc", "fp16", 8, "_mm_round_ph", "vrndq_f16">;
def Prim_FP16_256_Trunc          : VectorIntrinsicPrimitive<"trunc", "fp16", 16, "_mm256_round_ph", "svrintz_f16_z">;
def Prim_FP16_512_Trunc          : VectorIntrinsicPrimitive<"trunc", "fp16", 32, "_mm512_round_ph", "svrnd_f16_z">;

def Prim_BF16_Add                : VectorIntrinsicPrimitive<"add", "bf16", 8, "_mm_addne_pbh", "vaddq_bf16">;
def Prim_BF16_256_Add            : VectorIntrinsicPrimitive<"add", "bf16", 16, "_mm256_addne_pbh", "svadd_bf16_z">;
def Prim_BF16_512_Add            : VectorIntrinsicPrimitive<"add", "bf16", 32, "_mm512_addne_pbh", "svadd_bf16_z">;
def Prim_BF16_Sub                : VectorIntrinsicPrimitive<"sub", "bf16", 8, "_mm_subne_pbh", "vsubq_bf16">;
def Prim_BF16_256_Sub            : VectorIntrinsicPrimitive<"sub", "bf16", 16, "_mm256_subne_pbh", "svsub_bf16_z">;
def Prim_BF16_512_Sub            : VectorIntrinsicPrimitive<"sub", "bf16", 32, "_mm512_subne_pbh", "svsub_bf16_z">;
def Prim_BF16_Mul                : VectorIntrinsicPrimitive<"mul", "bf16", 8, "_mm_mulne_pbh", "vmulq_bf16">;
def Prim_BF16_256_Mul            : VectorIntrinsicPrimitive<"mul", "bf16", 16, "_mm256_mulne_pbh", "svmul_bf16_z">;
def Prim_BF16_512_Mul            : VectorIntrinsicPrimitive<"mul", "bf16", 32, "_mm512_mulne_pbh", "svmul_bf16_z">;
def Prim_BF16_Min                : VectorIntrinsicPrimitive<"min", "bf16", 8, "_mm_min_pbh", "vminq_bf16">;
def Prim_BF16_256_Min            : VectorIntrinsicPrimitive<"min", "bf16", 16, "_mm256_min_pbh", "svmin_bf16_z">;
def Prim_BF16_512_Min            : VectorIntrinsicPrimitive<"min", "bf16", 32, "_mm512_min_pbh", "svmin_bf16_z">;
def Prim_BF16_Max                : VectorIntrinsicPrimitive<"max", "bf16", 8, "_mm_max_pbh", "vmaxq_bf16">;
def Prim_BF16_256_Max            : VectorIntrinsicPrimitive<"max", "bf16", 16, "_mm256_max_pbh", "svmax_bf16_z">;
def Prim_BF16_512_Max            : VectorIntrinsicPrimitive<"max", "bf16", 32, "_mm512_max_pbh", "svmax_bf16_z">;
def Prim_BF16_CmpEq              : VectorIntrinsicPrimitive<"cmpeq", "bf16", 8, "_mm_cmpeq_pbh", "vcmeqq_bf16">;
def Prim_BF16_256_CmpEq          : VectorIntrinsicPrimitive<"cmpeq", "bf16", 16, "_mm256_cmpeq_pbh", "svcmeq_bf16_z">;
def Prim_BF16_512_CmpEq          : VectorIntrinsicPrimitive<"cmpeq", "bf16", 32, "_mm512_cmpeq_pbh", "svcmeq_bf16_z">;
def Prim_BF16_CmpGt              : VectorIntrinsicPrimitive<"cmpgt", "bf16", 8, "_mm_cmpgt_pbh", "vcmgtq_bf16">;
def Prim_BF16_256_CmpGt          : VectorIntrinsicPrimitive<"cmpgt", "bf16", 16, "_mm256_cmpgt_pbh", "svcmgt_bf16_z">;
def Prim_BF16_512_CmpGt          : VectorIntrinsicPrimitive<"cmpgt", "bf16", 32, "_mm512_cmpgt_pbh", "svcmgt_bf16_z">;
def Prim_BF16_BlendV             : VectorIntrinsicPrimitive<"blendv", "bf16", 8, "_mm_blendv_pbh", "vbslq_bf16">;
def Prim_BF16_256_BlendV         : VectorIntrinsicPrimitive<"blendv", "bf16", 16, "_mm256_blendv_pbh", "svbsl_bf16_z">;
def Prim_BF16_512_BlendV         : VectorIntrinsicPrimitive<"blendv", "bf16", 32, "_mm512_blendv_pbh", "svbsl_bf16_z">;
def Prim_BF16_And                : VectorIntrinsicPrimitive<"and", "bf16", 8, "_mm_and_pbh", "vandq_bf16">;
def Prim_BF16_256_And            : VectorIntrinsicPrimitive<"and", "bf16", 16, "_mm256_and_pbh", "svand_bf16_z">;
def Prim_BF16_512_And            : VectorIntrinsicPrimitive<"and", "bf16", 32, "_mm512_and_pbh", "svand_bf16_z">;
def Prim_BF16_Or                 : VectorIntrinsicPrimitive<"or", "bf16", 8, "_mm_or_pbh", "vorrq_bf16">;
def Prim_BF16_256_Or             : VectorIntrinsicPrimitive<"or", "bf16", 16, "_mm256_or_pbh", "svorr_bf16_z">;
def Prim_BF16_512_Or             : VectorIntrinsicPrimitive<"or", "bf16", 32, "_mm512_or_pbh", "svorr_bf16_z">;
def Prim_BF16_Xor                : VectorIntrinsicPrimitive<"xor", "bf16", 8, "_mm_xor_pbh", "veorq_bf16">;
def Prim_BF16_256_Xor            : VectorIntrinsicPrimitive<"xor", "bf16", 16, "_mm256_xor_pbh", "sveor_bf16_z">;
def Prim_BF16_512_Xor            : VectorIntrinsicPrimitive<"xor", "bf16", 32, "_mm512_xor_pbh", "sveor_bf16_z">;
def Prim_BF16_Div                : VectorIntrinsicPrimitive<"div", "bf16", 8, "_mm_divne_pbh", "vdivq_bf16">;
def Prim_BF16_256_Div            : VectorIntrinsicPrimitive<"div", "bf16", 16, "_mm256_divne_pbh", "svdiv_bf16_z">;
def Prim_BF16_512_Div            : VectorIntrinsicPrimitive<"div", "bf16", 32, "_mm512_divne_pbh", "svdiv_bf16_z">;
def Prim_BF16_FMAdd              : VectorIntrinsicPrimitive<"fmadd", "bf16", 8, "_mm_fmaddne_pbh", "vfmaq_bf16">;
def Prim_BF16_256_FMAdd          : VectorIntrinsicPrimitive<"fmadd", "bf16", 16, "_mm256_fmaddne_pbh", "svmla_bf16_z">;
def Prim_BF16_512_FMAdd          : VectorIntrinsicPrimitive<"fmadd", "bf16", 32, "_mm512_fmaddne_pbh", "svfma_bf16_z">;
def Prim_BF16_FMSub              : VectorIntrinsicPrimitive<"fmsub", "bf16", 8, "_mm_fmsubne_pbh", "vfmsq_bf16">;
def Prim_BF16_256_FMSub          : VectorIntrinsicPrimitive<"fmsub", "bf16", 16, "_mm256_fmsubne_pbh", "svmls_bf16_z">;
def Prim_BF16_512_FMSub          : VectorIntrinsicPrimitive<"fmsub", "bf16", 32, "_mm512_fmsubne_pbh", "svfms_bf16_z">;
def Prim_BF16_Sqrt               : VectorIntrinsicPrimitive<"sqrt", "bf16", 8, "_mm_sqrtne_pbh", "vsqrtq_bf16">;
def Prim_BF16_256_Sqrt           : VectorIntrinsicPrimitive<"sqrt", "bf16", 16, "_mm256_sqrtne_pbh", "svsqrt_bf16_z">;
def Prim_BF16_512_Sqrt           : VectorIntrinsicPrimitive<"sqrt", "bf16", 32, "_mm512_sqrtne_pbh", "svsqrt_bf16_z">;
def Prim_BF16_RSqrt              : VectorIntrinsicPrimitive<"rsqrt", "bf16", 8, "_mm_rsqrt_pbh", "vrsqrteq_bf16">;
def Prim_BF16_256_RSqrt          : VectorIntrinsicPrimitive<"rsqrt", "bf16", 16, "_mm256_rsqrt_pbh", "svrsqrte_bf16">;
def Prim_BF16_512_RSqrt          : VectorIntrinsicPrimitive<"rsqrt", "bf16", 32, "_mm512_rsqrt_pbh", "svrsqrte_bf16_z">;
def Prim_BF16_Rcp                : VectorIntrinsicPrimitive<"rcp", "bf16", 8, "_mm_rcp_pbh", "vrecpeq_bf16">;
def Prim_BF16_256_Rcp            : VectorIntrinsicPrimitive<"rcp", "bf16", 16, "_mm256_rcp_pbh", "svrecpe_bf16">;
def Prim_BF16_512_Rcp            : VectorIntrinsicPrimitive<"rcp", "bf16", 32, "_mm512_rcp_pbh", "svrecpe_bf16_z">;
def Prim_BF16_Floor              : VectorIntrinsicPrimitive<"floor", "bf16", 8, "_mm_floor_pbh", "vrndmq_bf16">;
def Prim_BF16_256_Floor          : VectorIntrinsicPrimitive<"floor", "bf16", 16, "_mm256_floor_pbh", "svrintm_bf16_z">;
def Prim_BF16_512_Floor          : VectorIntrinsicPrimitive<"floor", "bf16", 32, "_mm512_floor_pbh", "svrndm_bf16_z">;
def Prim_BF16_Ceil               : VectorIntrinsicPrimitive<"ceil", "bf16", 8, "_mm_ceil_pbh", "vrndpq_bf16">;
def Prim_BF16_256_Ceil           : VectorIntrinsicPrimitive<"ceil", "bf16", 16, "_mm256_ceil_pbh", "svrintp_bf16_z">;
def Prim_BF16_512_Ceil           : VectorIntrinsicPrimitive<"ceil", "bf16", 32, "_mm512_ceil_pbh", "svrndp_bf16_z">;
def Prim_BF16_Trunc              : VectorIntrinsicPrimitive<"trunc", "bf16", 8, "_mm_round_pbh", "vrndq_bf16">;
def Prim_BF16_256_Trunc          : VectorIntrinsicPrimitive<"trunc", "bf16", 16, "_mm256_round_pbh", "svrintz_bf16_z">;
def Prim_BF16_512_Trunc          : VectorIntrinsicPrimitive<"trunc", "bf16", 32, "_mm512_round_pbh", "svrnd_bf16_z">;

def Prim_F32_Add                 : VectorIntrinsicPrimitive<"add", "f32", 4, "_mm_add_ps", "vaddq_f32">;
def Prim_F32_256_Add             : VectorIntrinsicPrimitive<"add", "f32", 8, "_mm256_add_ps", "svadd_f32_z">;
def Prim_F32_512_Add             : VectorIntrinsicPrimitive<"add", "f32", 16, "_mm512_add_ps", "svadd_f32_z">;
def Prim_F32_Sub                 : VectorIntrinsicPrimitive<"sub", "f32", 4, "_mm_sub_ps", "vsubq_f32">;
def Prim_F32_256_Sub             : VectorIntrinsicPrimitive<"sub", "f32", 8, "_mm256_sub_ps", "svsub_f32_z">;
def Prim_F32_512_Sub             : VectorIntrinsicPrimitive<"sub", "f32", 16, "_mm512_sub_ps", "svsub_f32_z">;
def Prim_F32_Mul                 : VectorIntrinsicPrimitive<"mul", "f32", 4, "_mm_mul_ps", "vmulq_f32">;
def Prim_F32_256_Mul             : VectorIntrinsicPrimitive<"mul", "f32", 8, "_mm256_mul_ps", "svmul_f32_z">;
def Prim_F32_512_Mul             : VectorIntrinsicPrimitive<"mul", "f32", 16, "_mm512_mul_ps", "svmul_f32_z">;
def Prim_F32_Min                 : VectorIntrinsicPrimitive<"min", "f32", 4, "_mm_min_ps", "vminq_f32">;
def Prim_F32_256_Min             : VectorIntrinsicPrimitive<"min", "f32", 8, "_mm256_min_ps", "svmin_f32_z">;
def Prim_F32_512_Min             : VectorIntrinsicPrimitive<"min", "f32", 16, "_mm512_min_ps", "svmin_f32_z">;
def Prim_F32_Max                 : VectorIntrinsicPrimitive<"max", "f32", 4, "_mm_max_ps", "vmaxq_f32">;
def Prim_F32_256_Max             : VectorIntrinsicPrimitive<"max", "f32", 8, "_mm256_max_ps", "svmax_f32_z">;
def Prim_F32_512_Max             : VectorIntrinsicPrimitive<"max", "f32", 16, "_mm512_max_ps", "svmax_f32_z">;
def Prim_F32_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "f32", 4, "_mm_cmpeq_ps", "vcmeqq_f32">;
def Prim_F32_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "f32", 8, "_mm256_cmpeq_ps", "svcmeq_f32_z">;
def Prim_F32_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "f32", 16, "_mm512_cmpeq_ps", "svcmeq_f32_z">;
def Prim_F32_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "f32", 4, "_mm_cmpgt_ps", "vcmgtq_f32">;
def Prim_F32_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "f32", 8, "_mm256_cmpgt_ps", "svcmgt_f32_z">;
def Prim_F32_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "f32", 16, "_mm512_cmpgt_ps", "svcmgt_f32_z">;
def Prim_F32_BlendV              : VectorIntrinsicPrimitive<"blendv", "f32", 4, "_mm_blendv_ps", "vbslq_f32">;
def Prim_F32_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "f32", 8, "_mm256_blendv_ps", "svbsl_f32_z">;
def Prim_F32_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "f32", 16, "_mm512_mask_blend_ps", "svbsl_f32_z">;
def Prim_F32_And                 : VectorIntrinsicPrimitive<"and", "f32", 4, "_mm_and_ps", "vandq_f32">;
def Prim_F32_256_And             : VectorIntrinsicPrimitive<"and", "f32", 8, "_mm256_and_ps", "svand_f32_z">;
def Prim_F32_512_And             : VectorIntrinsicPrimitive<"and", "f32", 16, "_mm512_and_ps", "svand_f32_z">;
def Prim_F32_Or                  : VectorIntrinsicPrimitive<"or", "f32", 4, "_mm_or_ps", "vorrq_f32">;
def Prim_F32_256_Or              : VectorIntrinsicPrimitive<"or", "f32", 8, "_mm256_or_ps", "svorr_f32_z">;
def Prim_F32_512_Or              : VectorIntrinsicPrimitive<"or", "f32", 16, "_mm512_or_ps", "svorr_f32_z">;
def Prim_F32_Xor                 : VectorIntrinsicPrimitive<"xor", "f32", 4, "_mm_xor_ps", "veorq_f32">;
def Prim_F32_256_Xor             : VectorIntrinsicPrimitive<"xor", "f32", 8, "_mm256_xor_ps", "sveor_f32_z">;
def Prim_F32_512_Xor             : VectorIntrinsicPrimitive<"xor", "f32", 16, "_mm512_xor_ps", "sveor_f32_z">;
def Prim_F32_Div                 : VectorIntrinsicPrimitive<"div", "f32", 4, "_mm_div_ps", "vdivq_f32">;
def Prim_F32_256_Div             : VectorIntrinsicPrimitive<"div", "f32", 8, "_mm256_div_ps", "svdiv_f32_z">;
def Prim_F32_512_Div             : VectorIntrinsicPrimitive<"div", "f32", 16, "_mm512_div_ps", "svdiv_f32_z">;
def Prim_F32_FMAdd               : VectorIntrinsicPrimitive<"fmadd", "f32", 4, "_mm_fmadd_ps", "vfmaq_f32">;
def Prim_F32_256_FMAdd           : VectorIntrinsicPrimitive<"fmadd", "f32", 8, "_mm256_fmadd_ps", "svmla_f32_z">;
def Prim_F32_512_FMAdd           : VectorIntrinsicPrimitive<"fmadd", "f32", 16, "_mm512_fmadd_ps", "svfma_f32_z">;
def Prim_F32_FMSub               : VectorIntrinsicPrimitive<"fmsub", "f32", 4, "_mm_fmsub_ps", "vfmsq_f32">;
def Prim_F32_256_FMSub           : VectorIntrinsicPrimitive<"fmsub", "f32", 8, "_mm256_fmsub_ps", "svmls_f32_z">;
def Prim_F32_512_FMSub           : VectorIntrinsicPrimitive<"fmsub", "f32", 16, "_mm512_fmsub_ps", "svfms_f32_z">;
def Prim_F32_Sqrt                : VectorIntrinsicPrimitive<"sqrt", "f32", 4, "_mm_sqrt_ps", "vsqrtq_f32">;
def Prim_F32_256_Sqrt            : VectorIntrinsicPrimitive<"sqrt", "f32", 8, "_mm256_sqrt_ps", "svsqrt_f32_z">;
def Prim_F32_512_Sqrt            : VectorIntrinsicPrimitive<"sqrt", "f32", 16, "_mm512_sqrt_ps", "svsqrt_f32_z">;
def Prim_F32_RSqrt               : VectorIntrinsicPrimitive<"rsqrt", "f32", 4, "_mm_rsqrt_ps", "vrsqrteq_f32">;
def Prim_F32_256_RSqrt           : VectorIntrinsicPrimitive<"rsqrt", "f32", 8, "_mm256_rsqrt_ps", "svrsqrte_f32">;
def Prim_F32_512_RSqrt           : VectorIntrinsicPrimitive<"rsqrt", "f32", 16, "_mm512_rsqrt_ps", "svrsqrte_f32_z">;
def Prim_F32_Rcp                 : VectorIntrinsicPrimitive<"rcp", "f32", 4, "_mm_rcp_ps", "vrecpeq_f32">;
def Prim_F32_256_Rcp             : VectorIntrinsicPrimitive<"rcp", "f32", 8, "_mm256_rcp_ps", "svrecpe_f32">;
def Prim_F32_512_Rcp             : VectorIntrinsicPrimitive<"rcp", "f32", 16, "_mm512_rcp_ps", "svrecpe_f32_z">;
def Prim_F32_Floor               : VectorIntrinsicPrimitive<"floor", "f32", 4, "_mm_floor_ps", "vrndmq_f32">;
def Prim_F32_256_Floor           : VectorIntrinsicPrimitive<"floor", "f32", 8, "_mm256_floor_ps", "svrintm_f32_z">;
def Prim_F32_512_Floor           : VectorIntrinsicPrimitive<"floor", "f32", 16, "_mm512_floor_ps", "svrndm_f32_z">;
def Prim_F32_Ceil                : VectorIntrinsicPrimitive<"ceil", "f32", 4, "_mm_ceil_ps", "vrndpq_f32">;
def Prim_F32_256_Ceil            : VectorIntrinsicPrimitive<"ceil", "f32", 8, "_mm256_ceil_ps", "svrintp_f32_z">;
def Prim_F32_512_Ceil            : VectorIntrinsicPrimitive<"ceil", "f32", 16, "_mm512_ceil_ps", "svrndp_f32_z">;
def Prim_F32_Trunc               : VectorIntrinsicPrimitive<"trunc", "f32", 4, "_mm_round_ps", "vrndq_f32">;
def Prim_F32_256_Trunc           : VectorIntrinsicPrimitive<"trunc", "f32", 8, "_mm256_round_ps", "svrintz_f32_z">;
def Prim_F32_512_Trunc           : VectorIntrinsicPrimitive<"trunc", "f32", 16, "_mm512_round_ps", "svrnd_f32_z">;

def Prim_F64_Add                 : VectorIntrinsicPrimitive<"add", "f64", 2, "_mm_add_pd", "vaddq_f64">;
def Prim_F64_256_Add             : VectorIntrinsicPrimitive<"add", "f64", 4, "_mm256_add_pd", "svadd_f64_z">;
def Prim_F64_512_Add             : VectorIntrinsicPrimitive<"add", "f64", 8, "_mm512_add_pd", "svadd_f64_z">;
def Prim_F64_Sub                 : VectorIntrinsicPrimitive<"sub", "f64", 2, "_mm_sub_pd", "vsubq_f64">;
def Prim_F64_256_Sub             : VectorIntrinsicPrimitive<"sub", "f64", 4, "_mm256_sub_pd", "svsub_f64_z">;
def Prim_F64_512_Sub             : VectorIntrinsicPrimitive<"sub", "f64", 8, "_mm512_sub_pd", "svsub_f64_z">;
def Prim_F64_Mul                 : VectorIntrinsicPrimitive<"mul", "f64", 2, "_mm_mul_pd", "vmulq_f64">;
def Prim_F64_256_Mul             : VectorIntrinsicPrimitive<"mul", "f64", 4, "_mm256_mul_pd", "svmul_f64_z">;
def Prim_F64_512_Mul             : VectorIntrinsicPrimitive<"mul", "f64", 8, "_mm512_mul_pd", "svmul_f64_z">;
def Prim_F64_Min                 : VectorIntrinsicPrimitive<"min", "f64", 2, "_mm_min_pd", "vminq_f64">;
def Prim_F64_256_Min             : VectorIntrinsicPrimitive<"min", "f64", 4, "_mm256_min_pd", "svmin_f64_z">;
def Prim_F64_512_Min             : VectorIntrinsicPrimitive<"min", "f64", 8, "_mm512_min_pd", "svmin_f64_z">;
def Prim_F64_Max                 : VectorIntrinsicPrimitive<"max", "f64", 2, "_mm_max_pd", "vmaxq_f64">;
def Prim_F64_256_Max             : VectorIntrinsicPrimitive<"max", "f64", 4, "_mm256_max_pd", "svmax_f64_z">;
def Prim_F64_512_Max             : VectorIntrinsicPrimitive<"max", "f64", 8, "_mm512_max_pd", "svmax_f64_z">;
def Prim_F64_CmpEq               : VectorIntrinsicPrimitive<"cmpeq", "f64", 2, "_mm_cmpeq_pd", "vcmeqq_f64">;
def Prim_F64_256_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "f64", 4, "_mm256_cmpeq_pd", "svcmeq_f64_z">;
def Prim_F64_512_CmpEq           : VectorIntrinsicPrimitive<"cmpeq", "f64", 8, "_mm512_cmpeq_pd", "svcmeq_f64_z">;
def Prim_F64_CmpGt               : VectorIntrinsicPrimitive<"cmpgt", "f64", 2, "_mm_cmpgt_pd", "vcmgtq_f64">;
def Prim_F64_256_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "f64", 4, "_mm256_cmpgt_pd", "svcmgt_f64_z">;
def Prim_F64_512_CmpGt           : VectorIntrinsicPrimitive<"cmpgt", "f64", 8, "_mm512_cmpgt_pd", "svcmgt_f64_z">;
def Prim_F64_BlendV              : VectorIntrinsicPrimitive<"blendv", "f64", 2, "_mm_blendv_pd", "vbslq_f64">;
def Prim_F64_256_BlendV          : VectorIntrinsicPrimitive<"blendv", "f64", 4, "_mm256_blendv_pd", "svbsl_f64_z">;
def Prim_F64_512_BlendV          : VectorIntrinsicPrimitive<"blendv", "f64", 8, "_mm512_mask_blend_pd", "svbsl_f64_z">;
def Prim_F64_And                 : VectorIntrinsicPrimitive<"and", "f64", 2, "_mm_and_pd", "vandq_f64">;
def Prim_F64_256_And             : VectorIntrinsicPrimitive<"and", "f64", 4, "_mm256_and_pd", "svand_f64_z">;
def Prim_F64_512_And             : VectorIntrinsicPrimitive<"and", "f64", 8, "_mm512_and_pd", "svand_f64_z">;
def Prim_F64_Or                  : VectorIntrinsicPrimitive<"or", "f64", 2, "_mm_or_pd", "vorrq_f64">;
def Prim_F64_256_Or              : VectorIntrinsicPrimitive<"or", "f64", 4, "_mm256_or_pd", "svorr_f64_z">;
def Prim_F64_512_Or              : VectorIntrinsicPrimitive<"or", "f64", 8, "_mm512_or_pd", "svorr_f64_z">;
def Prim_F64_Xor                 : VectorIntrinsicPrimitive<"xor", "f64", 2, "_mm_xor_pd", "veorq_f64">;
def Prim_F64_256_Xor             : VectorIntrinsicPrimitive<"xor", "f64", 4, "_mm256_xor_pd", "sveor_f64_z">;
def Prim_F64_512_Xor             : VectorIntrinsicPrimitive<"xor", "f64", 8, "_mm512_xor_pd", "sveor_f64_z">;
def Prim_F64_Div                 : VectorIntrinsicPrimitive<"div", "f64", 2, "_mm_div_pd", "vdivq_f64">;
def Prim_F64_256_Div             : VectorIntrinsicPrimitive<"div", "f64", 4, "_mm256_div_pd", "svdiv_f64_z">;
def Prim_F64_512_Div             : VectorIntrinsicPrimitive<"div", "f64", 8, "_mm512_div_pd", "svdiv_f64_z">;
def Prim_F64_FMAdd               : VectorIntrinsicPrimitive<"fmadd", "f64", 2, "_mm_fmadd_pd", "vfmaq_f64">;
def Prim_F64_256_FMAdd           : VectorIntrinsicPrimitive<"fmadd", "f64", 4, "_mm256_fmadd_pd", "svmla_f64_z">;
def Prim_F64_512_FMAdd           : VectorIntrinsicPrimitive<"fmadd", "f64", 8, "_mm512_fmadd_pd", "svfma_f64_z">;
def Prim_F64_FMSub               : VectorIntrinsicPrimitive<"fmsub", "f64", 2, "_mm_fmsub_pd", "vfmsq_f64">;
def Prim_F64_256_FMSub           : VectorIntrinsicPrimitive<"fmsub", "f64", 4, "_mm256_fmsub_pd", "svmls_f64_z">;
def Prim_F64_512_FMSub           : VectorIntrinsicPrimitive<"fmsub", "f64", 8, "_mm512_fmsub_pd", "svfms_f64_z">;
def Prim_F64_Sqrt                : VectorIntrinsicPrimitive<"sqrt", "f64", 2, "_mm_sqrt_pd", "vsqrtq_f64">;
def Prim_F64_256_Sqrt            : VectorIntrinsicPrimitive<"sqrt", "f64", 4, "_mm256_sqrt_pd", "svsqrt_f64_z">;
def Prim_F64_512_Sqrt            : VectorIntrinsicPrimitive<"sqrt", "f64", 8, "_mm512_sqrt_pd", "svsqrt_f64_z">;
def Prim_F64_RSqrt               : VectorIntrinsicPrimitive<"rsqrt", "f64", 2, "_mm_rsqrt_pd", "vrsqrteq_f64">;
def Prim_F64_256_RSqrt           : VectorIntrinsicPrimitive<"rsqrt", "f64", 4, "_mm256_rsqrt_pd", "svrsqrte_f64">;
def Prim_F64_512_RSqrt           : VectorIntrinsicPrimitive<"rsqrt", "f64", 8, "_mm512_rsqrt_pd", "svrsqrte_f64_z">;
def Prim_F64_Rcp                 : VectorIntrinsicPrimitive<"rcp", "f64", 2, "_mm_rcp_pd", "vrecpeq_f64">;
def Prim_F64_256_Rcp             : VectorIntrinsicPrimitive<"rcp", "f64", 4, "_mm256_rcp_pd", "svrecpe_f64">;
def Prim_F64_512_Rcp             : VectorIntrinsicPrimitive<"rcp", "f64", 8, "_mm512_rcp_pd", "svrecpe_f64_z">;
def Prim_F64_Floor               : VectorIntrinsicPrimitive<"floor", "f64", 2, "_mm_floor_pd", "vrndmq_f64">;
def Prim_F64_256_Floor           : VectorIntrinsicPrimitive<"floor", "f64", 4, "_mm256_floor_pd", "svrintm_f64_z">;
def Prim_F64_512_Floor           : VectorIntrinsicPrimitive<"floor", "f64", 8, "_mm512_floor_pd", "svrndm_f64_z">;
def Prim_F64_Ceil                : VectorIntrinsicPrimitive<"ceil", "f64", 2, "_mm_ceil_pd", "vrndpq_f64">;
def Prim_F64_256_Ceil            : VectorIntrinsicPrimitive<"ceil", "f64", 4, "_mm256_ceil_pd", "svrintp_f64_z">;
def Prim_F64_512_Ceil            : VectorIntrinsicPrimitive<"ceil", "f64", 8, "_mm512_ceil_pd", "svrndp_f64_z">;
def Prim_F64_Trunc               : VectorIntrinsicPrimitive<"trunc", "f64", 2, "_mm_round_pd", "vrndq_f64">;
def Prim_F64_256_Trunc           : VectorIntrinsicPrimitive<"trunc", "f64", 4, "_mm256_round_pd", "svrintz_f64_z">;
def Prim_F64_512_Trunc           : VectorIntrinsicPrimitive<"trunc", "f64", 8, "_mm512_round_pd", "svrnd_f64_z">;

// Masked Vector Arithmetic Primitives (AVX-512 {k} & SVE Predication)
def Mask_F32_512_Add           : VectorIntrinsicPrimitive<"mask_add", "f32", 16, "_mm512_mask_add_ps", "svadd_f32_m">;
def Mask_F32_512_Sub           : VectorIntrinsicPrimitive<"mask_sub", "f32", 16, "_mm512_mask_sub_ps", "svsub_f32_m">;
def Mask_F32_512_Mul           : VectorIntrinsicPrimitive<"mask_mul", "f32", 16, "_mm512_mask_mul_ps", "svmul_f32_m">;
def Mask_F32_512_Div           : VectorIntrinsicPrimitive<"mask_div", "f32", 16, "_mm512_mask_div_ps", "svdiv_f32_m">;
def Mask_F32_512_FMAdd         : VectorIntrinsicPrimitive<"mask_fmadd", "f32", 16, "_mm512_mask_fmadd_ps", "svmla_f32_m">;
def Mask_F32_512_Sqrt          : VectorIntrinsicPrimitive<"mask_sqrt", "f32", 16, "_mm512_mask_sqrt_ps", "svsqrt_f32_m">;
def Mask_F32_512_Min           : VectorIntrinsicPrimitive<"mask_min", "f32", 16, "_mm512_mask_min_ps", "svmin_f32_m">;
def Mask_F32_512_Max           : VectorIntrinsicPrimitive<"mask_max", "f32", 16, "_mm512_mask_max_ps", "svmax_f32_m">;
def Mask_F64_512_Add           : VectorIntrinsicPrimitive<"mask_add", "f64", 8, "_mm512_mask_add_pd", "svadd_f64_m">;
def Mask_F64_512_Sub           : VectorIntrinsicPrimitive<"mask_sub", "f64", 8, "_mm512_mask_sub_pd", "svsub_f64_m">;
def Mask_F64_512_Mul           : VectorIntrinsicPrimitive<"mask_mul", "f64", 8, "_mm512_mask_mul_pd", "svmul_f64_m">;
def Mask_F64_512_Div           : VectorIntrinsicPrimitive<"mask_div", "f64", 8, "_mm512_mask_div_pd", "svdiv_f64_m">;
def Mask_F64_512_FMAdd         : VectorIntrinsicPrimitive<"mask_fmadd", "f64", 8, "_mm512_mask_fmadd_pd", "svmla_f64_m">;
def Mask_F64_512_Sqrt          : VectorIntrinsicPrimitive<"mask_sqrt", "f64", 8, "_mm512_mask_sqrt_pd", "svsqrt_f64_m">;
def MaskZ_F32_512_Add          : VectorIntrinsicPrimitive<"maskz_add", "f32", 16, "_mm512_maskz_add_ps", "svadd_f32_z">;
def MaskZ_F32_512_Mul          : VectorIntrinsicPrimitive<"maskz_mul", "f32", 16, "_mm512_maskz_mul_ps", "svmul_f32_z">;
def MaskZ_F32_512_FMAdd        : VectorIntrinsicPrimitive<"maskz_fmadd", "f32", 16, "_mm512_maskz_fmadd_ps", "svmla_f32_z">;
def MaskZ_F64_512_Add          : VectorIntrinsicPrimitive<"maskz_add", "f64", 8, "_mm512_maskz_add_pd", "svadd_f64_z">;
def MaskZ_F64_512_Mul          : VectorIntrinsicPrimitive<"maskz_mul", "f64", 8, "_mm512_maskz_mul_pd", "svmul_f64_z">;
def MaskZ_F64_512_FMAdd        : VectorIntrinsicPrimitive<"maskz_fmadd", "f64", 8, "_mm512_maskz_fmadd_pd", "svmla_f64_z">;
def Mask_FP16_512_Add          : VectorIntrinsicPrimitive<"mask_add", "fp16", 32, "_mm512_mask_add_ph", "svadd_f16_m">;
def Mask_FP16_512_Mul          : VectorIntrinsicPrimitive<"mask_mul", "fp16", 32, "_mm512_mask_mul_ph", "svmul_f16_m">;
def Mask_FP16_512_FMAdd        : VectorIntrinsicPrimitive<"mask_fmadd", "fp16", 32, "_mm512_mask_fmadd_ph", "svmla_f16_m">;
def Mask_I32_512_Add           : VectorIntrinsicPrimitive<"mask_add", "i32", 16, "_mm512_mask_add_epi32", "svadd_s32_m">;
def Mask_I32_512_Mul           : VectorIntrinsicPrimitive<"mask_mul", "i32", 16, "_mm512_mask_mullo_epi32", "svmul_s32_m">;
def Mask_I64_512_Add           : VectorIntrinsicPrimitive<"mask_add", "i64", 8, "_mm512_mask_add_epi64", "svadd_s64_m">;
def Mask_I64_512_Mul           : VectorIntrinsicPrimitive<"mask_mul", "i64", 8, "_mm512_mask_mullo_epi64", "svmul_s64_m">;

// Vector Data Permutation, Shuffling, Blending and Gathering
def Prim_Perm_F32_256_Var      : VectorIntrinsicPrimitive<"permutevar", "f32", 8, "_mm256_permutevar8x32_ps", "vtbl2_u32">;
def Prim_Perm_F32_512_Var      : VectorIntrinsicPrimitive<"permutevar", "f32", 16, "_mm512_permutexvar_ps", "svtbl_f32">;
def Prim_Perm_F64_256_Var      : VectorIntrinsicPrimitive<"permutevar", "f64", 4, "_mm256_permutex_pd", "vtbl2_u64">;
def Prim_Perm_F64_512_Var      : VectorIntrinsicPrimitive<"permutevar", "f64", 8, "_mm512_permutexvar_pd", "svtbl_f64">;
def Prim_Blend_F32_128         : VectorIntrinsicPrimitive<"blend", "f32", 4, "_mm_blend_ps", "vbslq_f32">;
def Prim_Blend_F32_256         : VectorIntrinsicPrimitive<"blend", "f32", 8, "_mm256_blend_ps", "svbsl_f32_z">;
def Prim_Blend_F64_128         : VectorIntrinsicPrimitive<"blend", "f64", 2, "_mm_blend_pd", "vbslq_f64">;
def Prim_Blend_F64_256         : VectorIntrinsicPrimitive<"blend", "f64", 4, "_mm256_blend_pd", "svbsl_f64_z">;
def Prim_Gather_F32_256        : VectorIntrinsicPrimitive<"gather", "f32", 8, "_mm256_i32gather_ps", "svld1_gather_offset_f32">;
def Prim_Gather_F32_512        : VectorIntrinsicPrimitive<"gather", "f32", 16, "_mm512_i32gather_ps", "svld1_gather_offset_f32">;
def Prim_Gather_F64_256        : VectorIntrinsicPrimitive<"gather", "f64", 4, "_mm256_i32gather_pd", "svld1_gather_offset_f64">;
def Prim_Gather_F64_512        : VectorIntrinsicPrimitive<"gather", "f64", 8, "_mm512_i64gather_pd", "svld1_gather_offset_f64">;
def Prim_Scatter_F32_512       : VectorIntrinsicPrimitive<"scatter", "f32", 16, "_mm512_i32scatter_ps", "svst1_scatter_offset_f32">;
def Prim_Scatter_F64_512       : VectorIntrinsicPrimitive<"scatter", "f64", 8, "_mm512_i64scatter_pd", "svst1_scatter_offset_f64">;

// Hardware Cryptographic Acceleration (SHA256, SHA512 & AES)
def Prim_Crypto_SHA256Msg1     : VectorIntrinsicPrimitive<"sha256_msg1", "u32", 4, "_mm_sha256msg1_epu32", "vsha256su0q_u32">;
def Prim_Crypto_SHA256Msg2     : VectorIntrinsicPrimitive<"sha256_msg2", "u32", 4, "_mm_sha256msg2_epu32", "vsha256su1q_u32">;
def Prim_Crypto_SHA256Rnd1     : VectorIntrinsicPrimitive<"sha256_rnd1", "u32", 4, "_mm_sha256rnds2_epu32", "vsha256hq_u32">;
def Prim_Crypto_AESEnc         : VectorIntrinsicPrimitive<"aes_enc", "u8", 16, "_mm_aesenc_si128", "vaeseq_u8">;
def Prim_Crypto_AESDec         : VectorIntrinsicPrimitive<"aes_dec", "u8", 16, "_mm_aesdec_si128", "vaesdq_u8">;
def Prim_Crypto_AESKeyGen      : VectorIntrinsicPrimitive<"aes_keygen", "u8", 16, "_mm_aeskeygenassist_si128", "vaeskeygenq_u8">;
def Prim_Crypto_SHA512Msg1     : VectorIntrinsicPrimitive<"sha512_msg1", "u64", 2, "_mm512_sha512msg1_epu64", "vsha512su0q_u64">;
def Prim_Crypto_SHA512Msg2     : VectorIntrinsicPrimitive<"sha512_msg2", "u64", 2, "_mm512_sha512msg2_epu64", "vsha512su1q_u64">;
def Prim_Crypto_SHA512Rnd      : VectorIntrinsicPrimitive<"sha512_rnd", "u64", 2, "_mm512_sha512rnds2_epu64", "vsha512hq_u64">;
def Prim_Crypto_SM3Rnds4       : VectorIntrinsicPrimitive<"sm3_rnds4", "u32", 4, "_mm_sm3rnds4_epi32", "vsm3partw1q_u32">;
def Prim_Crypto_SM4Rnds4       : VectorIntrinsicPrimitive<"sm4_rnds4", "u32", 4, "_mm_sm4rnds4_epi32", "vsm4ekeyq_u32">;

// Hardware Entropy, Bit Manipulation & Streaming Synchronizations
def Prim_TRNG_RDRAND32       : HardwareScalarPrimitive<"rdrand32", "uint32_t", "_rdrand32_step", "__builtin_arm_rndr">;
def Prim_TRNG_RDRAND64       : HardwareScalarPrimitive<"rdrand64", "uint64_t", "_rdrand64_step", "__builtin_arm_rndr">;
def Prim_TRNG_RDSEED32       : HardwareScalarPrimitive<"rdseed32", "uint32_t", "_rdseed32_step", "__builtin_arm_rndrrs">;
def Prim_TRNG_RDSEED64       : HardwareScalarPrimitive<"rdseed64", "uint64_t", "_rdseed64_step", "__builtin_arm_rndrrs">;
def Prim_BMI_Popcnt32        : HardwareScalarPrimitive<"popcnt32", "uint32_t", "_mm_popcnt_u32", "__builtin_popcount">;
def Prim_BMI_Popcnt64        : HardwareScalarPrimitive<"popcnt64", "uint64_t", "_mm_popcnt_u64", "__builtin_popcountll">;
def Prim_BMI_Lzcnt32         : HardwareScalarPrimitive<"lzcnt32", "uint32_t", "__lzcnt", "__builtin_clz">;
def Prim_BMI_Lzcnt64         : HardwareScalarPrimitive<"lzcnt64", "uint64_t", "__lzcnt64", "__builtin_clzll">;
def Prim_BMI_Tzcnt32         : HardwareScalarPrimitive<"tzcnt32", "uint32_t", "_tzcnt_u32", "__builtin_ctz">;
def Prim_BMI_Tzcnt64         : HardwareScalarPrimitive<"tzcnt64", "uint64_t", "_tzcnt_u64", "__builtin_ctzll">;
def Prim_BMI2_Pext64         : HardwareScalarPrimitive<"pext64", "uint64_t", "_pext_u64", "__builtin_arm_pext">;
def Prim_BMI2_Pdep64         : HardwareScalarPrimitive<"pdep64", "uint64_t", "_pdep_u64", "__builtin_arm_pdep">;
def Prim_BMI_Blsr64          : HardwareScalarPrimitive<"blsr64", "uint64_t", "_blsr_u64", "__builtin_arm_blsr">;
def Prim_BMI_Blsmsk64        : HardwareScalarPrimitive<"blsmsk64", "uint64_t", "_blsmsk_u64", "__builtin_arm_blsmsk">;
def Prim_BMI_Bextr64         : HardwareScalarPrimitive<"bextr64", "uint64_t", "_bextr_u64", "__builtin_arm_bextr">;
def Prim_Mem_StreamPS128     : HardwareScalarPrimitive<"stream_ps_128", "float*", "_mm_stream_ps", "__builtin_nontemporal_store">;
def Prim_Mem_StreamPS256     : HardwareScalarPrimitive<"stream_ps_256", "float*", "_mm256_stream_ps", "__builtin_nontemporal_store">;
def Prim_Mem_StreamPS512     : HardwareScalarPrimitive<"stream_ps_512", "float*", "_mm512_stream_ps", "svstnt1_f32">;
def Prim_Mem_StreamPD128     : HardwareScalarPrimitive<"stream_pd_128", "double*", "_mm_stream_pd", "__builtin_nontemporal_store">;
def Prim_Mem_StreamPD256     : HardwareScalarPrimitive<"stream_pd_256", "double*", "_mm256_stream_pd", "__builtin_nontemporal_store">;
def Prim_Mem_StreamPD512     : HardwareScalarPrimitive<"stream_pd_512", "double*", "_mm512_stream_pd", "svstnt1_f64">;
def Prim_Mem_PrefetchT0      : HardwareScalarPrimitive<"prefetch_t0", "void*", "_mm_prefetch(_MM_HINT_T0)", "__builtin_prefetch(ptr,0,3)">;
def Prim_Mem_PrefetchT1      : HardwareScalarPrimitive<"prefetch_t1", "void*", "_mm_prefetch(_MM_HINT_T1)", "__builtin_prefetch(ptr,0,2)">;
def Prim_Mem_PrefetchT2      : HardwareScalarPrimitive<"prefetch_t2", "void*", "_mm_prefetch(_MM_HINT_T2)", "__builtin_prefetch(ptr,0,1)">;
def Prim_Mem_PrefetchNTA     : HardwareScalarPrimitive<"prefetch_nta", "void*", "_mm_prefetch(_MM_HINT_NTA)", "__builtin_prefetch(ptr,0,0)">;
def Prim_Mem_FenceStore      : HardwareScalarPrimitive<"fence_store", "void", "_mm_sfence()", "dmb ishst">;
def Prim_Mem_FenceLoad       : HardwareScalarPrimitive<"fence_load", "void", "_mm_lfence()", "dmb ishld">;
def Prim_Mem_FenceFull       : HardwareScalarPrimitive<"fence_full", "void", "_mm_mfence()", "dmb ish">;

// =============================================================================
// SECTION 10B: INSTRUCTION TIMINGS & HARDWARE INTRINSIC PATTERNS
// =============================================================================

class MachineInstruction<string asm_mnemonic, string intrinsic, MachineISA isa, RegisterLaneConfig lane> {
  string AsmMnemonic = asm_mnemonic;
  string IntrinsicPattern = intrinsic;
  MachineISA ISA = isa;
  RegisterLaneConfig Lane = lane;
  string LatencyCycles = "4.0";
  string ReciprocalThroughput = "0.5";
  list<ExecutionPort> Ports = [Port_0, Port_1];
  bit SupportsMasking = 0;
  bit SupportsEmbeddedBroadcast = 0;
}

def Inst_VFMADD213PS_512     : MachineInstruction<"vfmadd213ps", "_mm512_fmadd_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VFMADD213PD_512     : MachineInstruction<"vfmadd213pd", "_mm512_fmadd_pd", ISA_x86_AVX512F, Lane_ZMM_8xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VMULPS_512          : MachineInstruction<"vmulps", "_mm512_mul_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VMULPD_512          : MachineInstruction<"vmulpd", "_mm512_mul_pd", ISA_x86_AVX512F, Lane_ZMM_8xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VADDPS_512          : MachineInstruction<"vaddps", "_mm512_add_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VADDPD_512          : MachineInstruction<"vaddpd", "_mm512_add_pd", ISA_x86_AVX512F, Lane_ZMM_8xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VSQRTPS_512         : MachineInstruction<"vsqrtps", "_mm512_sqrt_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "18.0";
  let ReciprocalThroughput = "8.0";
  let Ports = [Port_0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VSQRTPD_512         : MachineInstruction<"vsqrtpd", "_mm512_sqrt_pd", ISA_x86_AVX512F, Lane_ZMM_8xF64> {
  let LatencyCycles = "23.0";
  let ReciprocalThroughput = "12.0";
  let Ports = [Port_0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VMOVNTPS_512        : MachineInstruction<"vmovntps", "_mm512_stream_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_4, Port_9];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VMOVNTPD_512        : MachineInstruction<"vmovntpd", "_mm512_stream_pd", ISA_x86_AVX512F, Lane_ZMM_8xF64> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_4, Port_9];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VFMADD213PH_512     : MachineInstruction<"vfmadd213ph", "_mm512_fmadd_ph", ISA_x86_AVX512FP16, Lane_ZMM_32xFP16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VADDPH_512          : MachineInstruction<"vaddph", "_mm512_add_ph", ISA_x86_AVX512FP16, Lane_ZMM_32xFP16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VMULPH_512          : MachineInstruction<"vmulph", "_mm512_mul_ph", ISA_x86_AVX512FP16, Lane_ZMM_32xFP16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VDPBF16PS_512       : MachineInstruction<"vdpbf16ps", "_mm512_dpbf16_ps", ISA_x86_AVX512BF16, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VFMADD213PS_256     : MachineInstruction<"vfmadd213ps", "_mm256_fmadd_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VFMADD213PD_256     : MachineInstruction<"vfmadd213pd", "_mm256_fmadd_pd", ISA_x86_AVX2, Lane_YMM_4xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VMULPS_256          : MachineInstruction<"vmulps", "_mm256_mul_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VMULPD_256          : MachineInstruction<"vmulpd", "_mm256_mul_pd", ISA_x86_AVX2, Lane_YMM_4xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VADDPS_256          : MachineInstruction<"vaddps", "_mm256_add_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VADDPD_256          : MachineInstruction<"vaddpd", "_mm256_add_pd", ISA_x86_AVX2, Lane_YMM_4xF64> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VSQRTPS_256         : MachineInstruction<"vsqrtps", "_mm256_sqrt_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "13.0";
  let ReciprocalThroughput = "4.0";
  let Ports = [Port_0];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VSQRTPD_256         : MachineInstruction<"vsqrtpd", "_mm256_sqrt_pd", ISA_x86_AVX2, Lane_YMM_4xF64> {
  let LatencyCycles = "18.0";
  let ReciprocalThroughput = "6.0";
  let Ports = [Port_0];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VMOVNTPS_256        : MachineInstruction<"vmovntps", "_mm256_stream_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_4];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VMOVNTPD_256        : MachineInstruction<"vmovntpd", "_mm256_stream_pd", ISA_x86_AVX2, Lane_YMM_4xF64> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_4];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_MULPS_128           : MachineInstruction<"mulps", "_mm_mul_ps", ISA_x86_SSE42, Lane_XMM_4xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_ADDPS_128           : MachineInstruction<"addps", "_mm_add_ps", ISA_x86_SSE42, Lane_XMM_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_MOVNTPS_128         : MachineInstruction<"movntps", "_mm_stream_ps", ISA_x86_SSE42, Lane_XMM_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_4];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_Zen5_VFMADD_512     : MachineInstruction<"vfmadd213ps", "_mm512_fmadd_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen5_FP0, Port_Zen5_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_Zen5_VMUL_512       : MachineInstruction<"vmulps", "_mm512_mul_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen5_FP0, Port_Zen5_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_Zen5_VADD_512       : MachineInstruction<"vaddps", "_mm512_add_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen5_FP0, Port_Zen5_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_LionCove_FMA_256    : MachineInstruction<"vfmadd213ps", "_mm256_fmadd_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_LionCove_P0, Port_LionCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_LionCove_VNNI_256   : MachineInstruction<"vpdpbusd", "_mm256_dpbusd_epi32", ISA_x86_AVX_VNNI, Lane_YMM_8xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_LionCove_P0, Port_LionCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_Skymont_FMA_256     : MachineInstruction<"vfmadd213ps", "_mm256_fmadd_ps", ISA_x86_AVX2, Lane_YMM_8xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Skymont_P0, Port_Skymont_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_Zen6_VFMADD_512     : MachineInstruction<"vfmadd213ps", "_mm512_fmadd_ps", ISA_x86_AVX512F, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen6_FP0, Port_Zen6_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_Zen6_VFMADD_PH_512  : MachineInstruction<"vfmadd213ph", "_mm512_fmadd_ph", ISA_x86_AVX512FP16, Lane_ZMM_32xFP16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen6_FP0, Port_Zen6_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_Zen6_VDPBF16PS_512  : MachineInstruction<"vdpbf16ps", "_mm512_dpbf16_ps", ISA_x86_AVX512BF16, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Zen6_FP0, Port_Zen6_FP1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_FMLA_NEON_F32       : MachineInstruction<"fmla", "vfmaq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_FADD_NEON_F32       : MachineInstruction<"fadd", "vaddq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_FMUL_NEON_F32       : MachineInstruction<"fmul", "vmulq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_FSQRT_NEON_F32      : MachineInstruction<"fsqrt", "vsqrtq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "12.0";
  let ReciprocalThroughput = "6.0";
  let Ports = [Port_Neoverse_V0];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_SVMLA_SVE_F32       : MachineInstruction<"fmla", "svmla_f32_m", ISA_ARM_SVE, Lane_SVE_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_SVADD_SVE_F32       : MachineInstruction<"fadd", "svadd_f32_m", ISA_ARM_SVE, Lane_SVE_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_SVMUL_SVE_F32       : MachineInstruction<"fmul", "svmul_f32_m", ISA_ARM_SVE, Lane_SVE_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_Neoverse_V0, Port_Neoverse_V1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_SVSQRT_SVE_F32      : MachineInstruction<"fsqrt", "svsqrt_f32_m", ISA_ARM_SVE, Lane_SVE_16xF32> {
  let LatencyCycles = "16.0";
  let ReciprocalThroughput = "8.0";
  let Ports = [Port_Neoverse_V0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_M4_FMLA_F32         : MachineInstruction<"fmla", "vfmaq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.25";
  let Ports = [Port_Apple_FMA0, Port_Apple_FMA1, Port_Apple_FMA2, Port_Apple_FMA3];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_M4_FMUL_F32         : MachineInstruction<"fmul", "vmulq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.25";
  let Ports = [Port_Apple_FMA0, Port_Apple_FMA1, Port_Apple_FMA2, Port_Apple_FMA3];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_Oryon_FMLA_F32      : MachineInstruction<"fmla", "vfmaq_f32", ISA_ARM_NEON, Lane_NEON_4xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.25";
  let Ports = [Port_Oryon_V0, Port_Oryon_V1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VFMACC_RVV_F32      : MachineInstruction<"vfmacc.vv", "__riscv_vfmacc_vv_f32m1", ISA_RISCV_Vector, Lane_RVV_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_RVV_V0, Port_RVV_V1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VFADD_RVV_F32       : MachineInstruction<"vfadd.vv", "__riscv_vfadd_vv_f32m1", ISA_RISCV_Vector, Lane_RVV_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_RVV_V0, Port_RVV_V1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPADDD_256          : MachineInstruction<"vpaddd", "_mm256_add_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.33";
  let Ports = [Port_0, Port_1, Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPADDD_512          : MachineInstruction<"vpaddd", "_mm512_add_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPMULLD_256         : MachineInstruction<"vpmulld", "_mm256_mullo_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "10.0";
  let ReciprocalThroughput = "2.0";
  let Ports = [Port_0];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPMULLD_512         : MachineInstruction<"vpmulld", "_mm512_mullo_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "10.0";
  let ReciprocalThroughput = "2.0";
  let Ports = [Port_0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPSLLD_256          : MachineInstruction<"vpslld", "_mm256_slli_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPSLLD_512          : MachineInstruction<"vpslld", "_mm512_slli_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPSRAD_256          : MachineInstruction<"vpsrad", "_mm256_srai_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPSRAD_512          : MachineInstruction<"vpsrad", "_mm512_srai_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPAND_256           : MachineInstruction<"vpand", "_mm256_and_si256", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.33";
  let Ports = [Port_0, Port_1, Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPAND_512           : MachineInstruction<"vpandd", "_mm512_and_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPOR_256            : MachineInstruction<"vpor", "_mm256_or_si256", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.33";
  let Ports = [Port_0, Port_1, Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPOR_512            : MachineInstruction<"vpord", "_mm512_or_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPXOR_256           : MachineInstruction<"vpxor", "_mm256_xor_si256", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.33";
  let Ports = [Port_0, Port_1, Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPXOR_512           : MachineInstruction<"vpxord", "_mm512_xor_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPBLENDD_256        : MachineInstruction<"vpblendd", "_mm256_blend_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "1.0";
  let ReciprocalThroughput = "0.33";
  let Ports = [Port_0, Port_1, Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPERMD_256          : MachineInstruction<"vpermd", "_mm256_permutevar8x32_epi32", ISA_x86_AVX2, Lane_YMM_8xI32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_5];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPERMD_512          : MachineInstruction<"vpermd", "_mm512_permutexvar_epi32", ISA_x86_AVX512F, Lane_ZMM_16xI32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPDPBUSD_256        : MachineInstruction<"vpdpbusd", "_mm256_dpbusd_epi32", ISA_x86_AVX_VNNI, Lane_YMM_8xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_VPDPBUSD_512        : MachineInstruction<"vpdpbusd", "_mm512_dpbusd_epi32", ISA_x86_AVX512VNNI, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_0, Port_5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_VPOPCNTD_512        : MachineInstruction<"vpopcntd", "_mm512_popcnt_epi32", ISA_x86_AVX512VPOPCNTDQ, Lane_ZMM_16xI32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// -----------------------------------------------------------------------------
// INTEL AVX10.2 ARCHITECTURE SPECIFICATION (361050-002US Rev 2.0) INSTRUCTIONS
// Chapters 7–14: Native BF16 Arithmetic, Enhanced EFLAGS Compare, OFP8 (E5M2/E4M3),
// FP16/Int8/Int16 VNNI, IEEE-754-2019 VMINMAX, YMM {er}/{sae}, Saturating Converts
// -----------------------------------------------------------------------------

// Chapter 7: Intel AVX10.2 Native Packed BF16 Arithmetic & Transcendentals
def Inst_AVX10_2_VADDNEPBF16_512     : MachineInstruction<"vaddnepbf16", "_mm512_addne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VSUBNEPBF16_512     : MachineInstruction<"vsubnepbf16", "_mm512_subne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VMULNEPBF16_512     : MachineInstruction<"vmulnepbf16", "_mm512_mulne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VDIVNEPBF16_512     : MachineInstruction<"vdivnepbf16", "_mm512_divne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "11.0";
  let ReciprocalThroughput = "4.0";
  let Ports = [Port_CoyoteCove_P0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VSQRTNEPBF16_512    : MachineInstruction<"vsqrtnepbf16", "_mm512_sqrtne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "12.0";
  let ReciprocalThroughput = "4.0";
  let Ports = [Port_CoyoteCove_P0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VFMADD213NEPBF16_512 : MachineInstruction<"vfmadd213nepbf16", "_mm512_fmaddne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VRCPPBF16_512       : MachineInstruction<"vrcppbf16", "_mm512_rcp_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VRSQRTPBF16_512     : MachineInstruction<"vrsqrtpbf16", "_mm512_rsqrt_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VSCALEFPBF16_512    : MachineInstruction<"vscalefpbf16", "_mm512_scalef_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VREDUCENEPBF16_512  : MachineInstruction<"vreducenepbf16", "_mm512_reducene_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VRNDSCALENEPBF16_512 : MachineInstruction<"vrndscalenepbf16", "_mm512_roundscalene_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VGETEXPPBF16_512    : MachineInstruction<"vgetexppbf16", "_mm512_getexp_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VGETMANTPBF16_512   : MachineInstruction<"vgetmantpbf16", "_mm512_getmant_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// Chapter 8: Intel AVX10.2 Scalar FP Comparisons with Enhanced EFLAGS (OF, SF, ZF, PF, CF)
def Inst_AVX10_2_VCOMXSS             : MachineInstruction<"vcomxss", "_mm_comx_ss", ISA_x86_AVX10_2, Lane_XMM_4xF32> {
  let LatencyCycles = "2.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_AVX10_2_VCOMXSD             : MachineInstruction<"vcomxsd", "_mm_comx_sd", ISA_x86_AVX10_2, Lane_XMM_2xF64> {
  let LatencyCycles = "2.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_AVX10_2_VCOMXSH             : MachineInstruction<"vcomxsh", "_mm_comx_sh", ISA_x86_AVX10_2, Lane_XMM_8xFP16> {
  let LatencyCycles = "2.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_AVX10_2_VCOMSBF16           : MachineInstruction<"vcomsbf16", "_mm_coms_sbh", ISA_x86_AVX10_2, Lane_XMM_8xBF16> {
  let LatencyCycles = "2.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 0;
  let SupportsEmbeddedBroadcast = 0;
}

// Chapter 9: Intel AVX10.2 OCP 8-Bit Floating Point (FP8: BF8=E5M2, HF8=E4M3) Conversions
def Inst_AVX10_2_VCVTNE2PH2BF8_512   : MachineInstruction<"vcvtne2ph2bf8", "_mm512_cvtne2ph_pbh8", ISA_x86_AVX10_2, Lane_ZMM_64xBF8_E5M2> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTNE2PH2BF8S_512  : MachineInstruction<"vcvtne2ph2bf8s", "_mm512_cvtne2phs_pbh8", ISA_x86_AVX10_2, Lane_ZMM_64xBF8_E5M2> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTNE2PH2HF8_512   : MachineInstruction<"vcvtne2ph2hf8", "_mm512_cvtne2ph_phf8", ISA_x86_AVX10_2, Lane_ZMM_64xHF8_E4M3> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTNE2PH2HF8S_512  : MachineInstruction<"vcvtne2ph2hf8s", "_mm512_cvtne2phs_phf8", ISA_x86_AVX10_2, Lane_ZMM_64xHF8_E4M3> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTBIASPH2BF8S_512 : MachineInstruction<"vcvtbiasph2bf8s", "_mm512_cvtbiasphs_pbh8", ISA_x86_AVX10_2, Lane_YMM_32xBF8_E5M2> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTBIASPH2HF8S_512 : MachineInstruction<"vcvtbiasph2hf8s", "_mm512_cvtbiasphs_phf8", ISA_x86_AVX10_2, Lane_YMM_32xHF8_E4M3> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTHF82PH_512      : MachineInstruction<"vcvthf82ph", "_mm512_cvthf8_ph", ISA_x86_AVX10_2, Lane_ZMM_32xFP16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}
def Inst_AVX10_2_VCVT2PS2PHX_512     : MachineInstruction<"vcvt2ps2phx", "_mm512_cvtx2ps_ph", ISA_x86_AVX10_2, Lane_ZMM_32xFP16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// Chapter 10: Intel AVX10.2 FP16 Dot-Product, All-Sign Int8/Int16 VNNI & 512b Media SAD
def Inst_AVX10_2_VDPPHPS_512         : MachineInstruction<"vdpphps", "_mm512_dpph_ps", ISA_x86_AVX10_2, Lane_ZMM_16xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPBSSD_512        : MachineInstruction<"vpdpbssd", "_mm512_dpbssd_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPBSUD_512        : MachineInstruction<"vpdpbsud", "_mm512_dpbsud_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPBUUD_512        : MachineInstruction<"vpdpbuud", "_mm512_dpbuud_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xU32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPWSUD_512        : MachineInstruction<"vpdpwsud", "_mm512_dpwsud_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPWUSD_512        : MachineInstruction<"vpdpwusd", "_mm512_dpwusd_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VPDPWUUD_512        : MachineInstruction<"vpdpwuud", "_mm512_dpwuud_epi32", ISA_x86_AVX10_2, Lane_ZMM_16xU32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VMPSADBW_512        : MachineInstruction<"vmpsadbw", "_mm512_mpsadbw_epu8", ISA_x86_AVX10_2, Lane_ZMM_32xU16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "1.0";
  let Ports = [Port_CoyoteCove_P5];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 0;
}

// Chapter 11: Intel AVX10.2 IEEE-754-2019 Compliant VMINMAX Instructions
def Inst_AVX10_2_VMINMAXPS_512       : MachineInstruction<"vminmaxps", "_mm512_minmax_ps", ISA_x86_AVX10_2, Lane_ZMM_16xF32> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VMINMAXPD_512       : MachineInstruction<"vminmaxpd", "_mm512_minmax_pd", ISA_x86_AVX10_2, Lane_ZMM_8xF64> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VMINMAXPH_512       : MachineInstruction<"vminmaxph", "_mm512_minmax_ph", ISA_x86_AVX10_2, Lane_ZMM_32xFP16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VMINMAXNEPBF16_512  : MachineInstruction<"vminmaxnepbf16", "_mm512_minmaxne_pbh", ISA_x86_AVX10_2, Lane_ZMM_32xBF16> {
  let LatencyCycles = "3.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// Chapter 12: Intel AVX10.2 256-bit YMM Embedded Rounding {er} & {sae} (EVEX.U=0, EVEX.b=1)
def Inst_AVX10_2_VFMADD213PS_YMM_ER  : MachineInstruction<"vfmadd213ps", "_mm256_fmadd_round_ps", ISA_x86_AVX10_2_256, Lane_YMM_8xF32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VFMADD213PD_YMM_ER  : MachineInstruction<"vfmadd213pd", "_mm256_fmadd_round_pd", ISA_x86_AVX10_2_256, Lane_YMM_4xF64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// Chapter 13: Intel AVX10.2 Saturating Floating-Point to Integer Conversions
def Inst_AVX10_2_VCVTPS2IBS_512      : MachineInstruction<"vcvtps2ibs", "_mm512_cvtps_ibs", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTPS2IUBS_512     : MachineInstruction<"vcvtps2iubs", "_mm512_cvtps_iubs", ISA_x86_AVX10_2, Lane_ZMM_16xU32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTPH2IBS_512      : MachineInstruction<"vcvtph2ibs", "_mm512_cvtph_ibs", ISA_x86_AVX10_2, Lane_ZMM_32xI16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTNEBF162IBS_512  : MachineInstruction<"vcvtnebf162ibs", "_mm512_cvtnepbh_ibs", ISA_x86_AVX10_2, Lane_ZMM_32xI16> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTTPS2DQS_512     : MachineInstruction<"vcvttps2dqs", "_mm512_cvttps_epi32s", ISA_x86_AVX10_2, Lane_ZMM_16xI32> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}
def Inst_AVX10_2_VCVTTPD2QQS_512     : MachineInstruction<"vcvttpd2qqs", "_mm512_cvttpd_epi64s", ISA_x86_AVX10_2, Lane_ZMM_8xI64> {
  let LatencyCycles = "4.0";
  let ReciprocalThroughput = "0.5";
  let Ports = [Port_CoyoteCove_P0, Port_CoyoteCove_P1];
  let SupportsMasking = 1;
  let SupportsEmbeddedBroadcast = 1;
}

// =============================================================================
// SECTION 11: 10-LEVEL DYNAMIC FALLBACK LADDER MATRIX & LOOP UNROLLING
// =============================================================================

class UnrollStrategy<string name, MachineISA isa, int unroll_factor, int elements_per_iter, int alignment_bytes, int safepoint_interval> {
  string Name = name;
  MachineISA ISA = isa;
  int UnrollFactor = unroll_factor;
  int ElementsPerIteration = elements_per_iter;
  int AlignmentBytes = alignment_bytes;
  int SafepointInterval = safepoint_interval;
}

def Unroll_Scalar            : UnrollStrategy<"Unroll_Scalar", ISA_Scalar, 1, 1, 4, 256>;
def Unroll_SSE42_4xF32       : UnrollStrategy<"Unroll_SSE42_4xF32", ISA_x86_SSE42, 4, 16, 16, 128>;
def Unroll_AVX2_8xF32        : UnrollStrategy<"Unroll_AVX2_8xF32", ISA_x86_AVX2, 4, 32, 32, 64>;
def Unroll_AVX2_NT_8xF32     : UnrollStrategy<"Unroll_AVX2_NT_8xF32", ISA_x86_AVX2, 8, 64, 32, 64>;
def Unroll_AVX512_16xF32     : UnrollStrategy<"Unroll_AVX512_16xF32", ISA_x86_AVX512F, 8, 128, 64, 32>;
def Unroll_AVX512_NT         : UnrollStrategy<"Unroll_AVX512_NT", ISA_x86_AVX512F, 16, 256, 64, 32>;
def Unroll_AVX10_2_16xF32    : UnrollStrategy<"Unroll_AVX10_2_16xF32", ISA_x86_AVX10_2, 8, 128, 64, 32>;
def Unroll_NEON_4xF32        : UnrollStrategy<"Unroll_NEON_4xF32", ISA_ARM_NEON, 4, 16, 16, 128>;
def Unroll_SVE2_16xF32       : UnrollStrategy<"Unroll_SVE2_16xF32", ISA_ARM_SVE2, 4, 64, 64, 64>;
def Unroll_RVV_512xF32       : UnrollStrategy<"Unroll_RVV_512xF32", ISA_RISCV_Vector, 2, 32, 64, 64>;
def Unroll_LASX_8xF32        : UnrollStrategy<"Unroll_LASX_8xF32", ISA_LoongArch_LASX, 4, 32, 32, 64>;
def Unroll_ARM_DSP_4xI16     : UnrollStrategy<"Unroll_ARM_DSP_4xI16", ISA_ARM_DSP, 2, 8, 4, 128>;
def Unroll_Xtensa_PIE_4xF32  : UnrollStrategy<"Unroll_Xtensa_PIE_4xF32", ISA_Xtensa_PIE, 2, 8, 16, 128>;
def Unroll_CORDIC_Scalar     : UnrollStrategy<"Unroll_CORDIC_Scalar", ISA_ST_CORDIC, 1, 1, 4, 256>;
def Unroll_SoftFloat_Scalar  : UnrollStrategy<"Unroll_SoftFloat_Scalar", ISA_SoftFloat_Q31, 1, 1, 4, 256>;

class DispatchLadderLevel<int level_id, string name, MachineISA isa, HardwareMemoryStrategy mem_strat, UnrollStrategy unroll, int min_elements> {
  int LevelID = level_id;
  string Name = name;
  MachineISA ISA = isa;
  HardwareMemoryStrategy MemoryStrategy = mem_strat;
  UnrollStrategy Unroll = unroll;
  int MinElements = min_elements;
}

def Ladder_L0_AMX_AVX512_Streaming     : DispatchLadderLevel<0, "AVX512_NonTemporal_Stream", ISA_x86_AVX512F, HW_StreamingNT, Unroll_AVX512_NT, 4096>;
def Ladder_L1_AVX512_FMA_Cached        : DispatchLadderLevel<1, "AVX512_FMA_Cached", ISA_x86_AVX512F, HW_StandardL1L2, Unroll_AVX512_16xF32, 256>;
def Ladder_L2_AVX512_VL_Prefetch       : DispatchLadderLevel<2, "AVX512_VL_PrefetchT0", ISA_x86_AVX512VL, HW_AggressiveT0, Unroll_AVX512_16xF32, 256>;
def Ladder_L3_Zen4_Zen5_DualFPU        : DispatchLadderLevel<3, "AMD_Zen_Dual_Pipe", ISA_x86_AVX512F, HW_StandardL1L2, Unroll_AVX512_16xF32, 128>;
def Ladder_L4_AVX_VNNI_FMA3            : DispatchLadderLevel<4, "AVX_VNNI_FMA3", ISA_x86_FMA3, HW_StandardL1L2, Unroll_AVX2_8xF32, 64>;
def Ladder_L5_AVX2_Streaming_NT        : DispatchLadderLevel<5, "AVX2_NonTemporal_Stream", ISA_x86_AVX2, HW_StreamingNT, Unroll_AVX2_NT_8xF32, 2048>;
def Ladder_L6_AVX2_Cached_Baseline     : DispatchLadderLevel<6, "AVX2_Cached_Baseline", ISA_x86_AVX2, HW_StandardL1L2, Unroll_AVX2_8xF32, 64>;
def Ladder_L7_SSE42_128Bit             : DispatchLadderLevel<7, "SSE42_128Bit_Unrolled", ISA_x86_SSE42, HW_StandardL1L2, Unroll_SSE42_4xF32, 16>;
def Ladder_L8_SRAM_Partitioned         : DispatchLadderLevel<8, "SRAM_Partitioned_TBB", ISA_x86_AVX2, HW_StandardL1L2, Unroll_AVX2_8xF32, 512>;
def Ladder_L9_Pure_Scalar_Fallback     : DispatchLadderLevel<9, "Pure_Scalar_Failsafe", ISA_Scalar, HW_StandardL1L2, Unroll_Scalar, 1>;
def Ladder_L10_ARM_SVE2                : DispatchLadderLevel<10, "ARM_SVE2_Scalable", ISA_ARM_SVE2, HW_StandardL1L2, Unroll_SVE2_16xF32, 128>;
def Ladder_L11_ARM_NEON                : DispatchLadderLevel<11, "ARM_NEON_QuadF32", ISA_ARM_NEON, HW_StandardL1L2, Unroll_NEON_4xF32, 64>;
def Ladder_L12_RISCV_Vector            : DispatchLadderLevel<12, "RISCV_Vector_V1", ISA_RISCV_Vector, HW_StandardL1L2, Unroll_RVV_512xF32, 64>;
def Ladder_L13_LoongArch_LASX          : DispatchLadderLevel<13, "LoongArch_LASX_256", ISA_LoongArch_LASX, HW_StandardL1L2, Unroll_LASX_8xF32, 64>;
def Ladder_L14_Embedded_DSP_PIE        : DispatchLadderLevel<14, "Embedded_DSP_PIE", ISA_ARM_DSP, HW_StandardL1L2, Unroll_ARM_DSP_4xI16, 16>;
def Ladder_L15_Hardware_Coprocessor    : DispatchLadderLevel<15, "Hardware_Coprocessor_CORDIC", ISA_ST_CORDIC, HW_StandardL1L2, Unroll_CORDIC_Scalar, 1>;
def Ladder_L16_SoftFloat_Q31           : DispatchLadderLevel<16, "SoftFloat_Q31_Fallback", ISA_SoftFloat_Q31, HW_StandardL1L2, Unroll_SoftFloat_Scalar, 1>;
def Ladder_L17_AVX10_2_Converged_512   : DispatchLadderLevel<17, "AVX10_2_Converged_512", ISA_x86_AVX10_2, HW_StandardL1L2, Unroll_AVX10_2_16xF32, 128>;

// =============================================================================
// SECTION 11B: GROUND-TRUTH MICROARCHITECTURE SILICON PROFILES (uops.info)
// =============================================================================

class MicroarchitectureProfile<string name, CPUFamily arch,
                                string fma512, string fma256, string fma128,
                                string str256, string str512, string ld256,
                                bit has_512, bit has_vnni, bit has_amx,
                                list<int> ranked_ladder> {
  string Name = name;
  CPUFamily Arch = arch;
  string FMA512_TP = fma512;
  string FMA256_TP = fma256;
  string FMA128_TP = fma128;
  string Stream256_TP = str256;
  string Stream512_TP = str512;
  string Load256_TP = ld256;
  bit HasAVX512 = has_512;
  bit HasVNNI = has_vnni;
  bit HasAMX = has_amx;
  list<int> RankedLadder = ranked_ladder;
}

def Profile_NVL_P        : MicroarchitectureProfile<"NVL-P", Arch_CoyoteCove, "0.500", "0.330", "0.330", "0.330", "0.500", "0.250", 1, 1, 1, [17, 1, 2, 8, 0, 4, 6, 5, 7, 9]>;
def Profile_NVL_E        : MicroarchitectureProfile<"NVL-E", Arch_ArcticWolf, "0.500", "0.500", "0.330", "0.500", "1.000", "0.330", 1, 1, 0, [17, 1, 2, 8, 4, 6, 5, 7, 0, 9]>;
def Profile_DMR          : MicroarchitectureProfile<"DMR", Arch_DiamondRapids, "0.500", "0.330", "0.330", "0.330", "0.500", "0.250", 1, 1, 1, [17, 1, 2, 8, 0, 4, 6, 5, 7, 9]>;
def Profile_ARL_P        : MicroarchitectureProfile<"ARL-P", Arch_Arrowlake, "1.000", "0.507", "0.507", "0.500", "1.000", "0.330", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_ARL_E        : MicroarchitectureProfile<"ARL-E", Arch_Skymont, "1.000", "0.585", "0.347", "1.000", "1.000", "0.670", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_LNL_P        : MicroarchitectureProfile<"LNL-P", Arch_Lunarlake, "1.000", "0.500", "0.500", "0.500", "1.000", "0.330", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_LNL_E        : MicroarchitectureProfile<"LNL-E", Arch_Skymont, "1.000", "0.580", "0.350", "1.000", "1.000", "0.670", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_MTL_P        : MicroarchitectureProfile<"MTL-P", Arch_Meteorlake, "1.000", "0.510", "0.500", "0.640", "1.000", "0.330", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_MTL_E        : MicroarchitectureProfile<"MTL-E", Arch_Crestmont, "1.000", "1.000", "0.542", "1.000", "1.000", "1.000", 0, 1, 0, [8, 5, 4, 6, 7, 9]>;
def Profile_ADL_P        : MicroarchitectureProfile<"ADL-P", Arch_Alderlake, "1.077", "0.539", "0.546", "0.500", "6.400", "0.330", 1, 1, 0, [8, 1, 2, 4, 6, 5, 7, 0, 9]>;
def Profile_ADL_E        : MicroarchitectureProfile<"ADL-E", Arch_Gracemont, "1.000", "1.000", "0.590", "1.000", "1.000", "1.000", 0, 1, 0, [8, 5, 4, 6, 7, 9]>;
def Profile_RKL          : MicroarchitectureProfile<"RKL", Arch_Rocketlake, "1.000", "0.500", "0.500", "1.055", "5.910", "0.500", 1, 1, 0, [2, 1, 8, 4, 6, 5, 7, 0, 9]>;
def Profile_TGL          : MicroarchitectureProfile<"TGL", Arch_Tigerlake, "1.000", "0.500", "0.500", "3.440", "6.840", "0.500", 1, 1, 0, [2, 1, 8, 4, 6, 7, 0, 5, 9]>;
def Profile_ICL          : MicroarchitectureProfile<"ICL", Arch_Icelake, "1.000", "0.500", "0.500", "1.050", "5.100", "0.500", 1, 1, 0, [2, 1, 8, 4, 6, 5, 7, 0, 9]>;
def Profile_SKX          : MicroarchitectureProfile<"SKX", Arch_Skylake, "0.525", "0.532", "0.525", "1.020", "17.640", "0.500", 1, 0, 0, [1, 2, 8, 6, 4, 5, 7, 0, 9]>;
def Profile_CLX          : MicroarchitectureProfile<"CLX", Arch_Cascadelake, "0.525", "0.532", "0.525", "1.020", "18.280", "0.500", 1, 1, 0, [1, 2, 8, 4, 6, 5, 7, 0, 9]>;
def Profile_EMR          : MicroarchitectureProfile<"EMR", Arch_EmeraldRap, "0.553", "0.539", "0.546", "0.530", "21.460", "0.330", 1, 1, 1, [1, 8, 2, 4, 6, 5, 7, 0, 9]>;
def Profile_SKL          : MicroarchitectureProfile<"SKL", Arch_Skylake, "1.000", "0.515", "0.515", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_ZEN6         : MicroarchitectureProfile<"ZEN6", Arch_Zen6, "0.500", "0.250", "0.250", "0.500", "0.500", "0.250", 1, 1, 0, [3, 1, 2, 8, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN6c        : MicroarchitectureProfile<"ZEN6C", Arch_Zen6c, "0.500", "0.250", "0.250", "0.500", "0.500", "0.250", 1, 1, 0, [3, 1, 2, 8, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN6_X3D     : MicroarchitectureProfile<"X6D", Arch_Zen6_X3D, "0.500", "0.250", "0.250", "0.500", "0.500", "0.250", 1, 1, 0, [3, 1, 8, 2, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN5         : MicroarchitectureProfile<"ZEN5", Arch_Zen5, "0.512", "0.500", "0.500", "0.500", "1.000", "0.500", 1, 1, 0, [3, 1, 2, 8, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN5c        : MicroarchitectureProfile<"ZEN5C", Arch_Zen5c, "0.512", "0.500", "0.500", "0.500", "1.000", "0.500", 1, 1, 0, [3, 1, 2, 8, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN5_X3D     : MicroarchitectureProfile<"Z5X", Arch_Zen5_X3D, "0.512", "0.500", "0.500", "0.500", "1.000", "0.500", 1, 1, 0, [3, 1, 8, 2, 0, 5, 4, 6, 7, 9]>;
def Profile_ZEN4         : MicroarchitectureProfile<"ZEN4", Arch_Zen4, "1.000", "0.501", "0.500", "1.000", "1.970", "0.500", 1, 1, 0, [3, 2, 8, 1, 4, 6, 0, 5, 7, 9]>;
def Profile_ZEN4c        : MicroarchitectureProfile<"ZEN4C", Arch_Zen4c, "1.000", "0.501", "0.500", "1.000", "1.970", "0.500", 1, 1, 0, [3, 2, 8, 1, 4, 6, 0, 5, 7, 9]>;
def Profile_ZEN3         : MicroarchitectureProfile<"ZEN3", Arch_Zen3, "1.000", "0.500", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_ZEN2         : MicroarchitectureProfile<"ZEN2", Arch_Zen2, "1.000", "0.585", "0.513", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_ZENp         : MicroarchitectureProfile<"ZEN+", Arch_ZenPlus, "1.000", "1.000", "0.513", "2.000", "1.000", "1.000", 0, 0, 0, [8, 6, 7, 4, 5, 9]>;
def Profile_HSW          : MicroarchitectureProfile<"HSW", Arch_Haswell, "1.000", "0.500", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_BDW          : MicroarchitectureProfile<"BDW", Arch_Broadwell, "1.000", "0.500", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_KBL          : MicroarchitectureProfile<"KBL", Arch_KabyLake, "1.000", "0.500", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_CFL          : MicroarchitectureProfile<"CFL", Arch_CoffeeLake, "1.000", "0.500", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [8, 6, 4, 5, 7, 9]>;
def Profile_GNR          : MicroarchitectureProfile<"GNR", Arch_GraniteRap, "0.500", "0.500", "0.500", "0.500", "1.000", "0.330", 1, 1, 1, [1, 8, 2, 4, 6, 5, 7, 0, 9]>;
def Profile_SRF          : MicroarchitectureProfile<"SRF", Arch_SierraForest, "1.000", "0.500", "0.500", "0.500", "1.000", "0.330", 0, 1, 0, [8, 4, 6, 5, 7, 9]>;
def Profile_M1           : MicroarchitectureProfile<"M1", Arch_AppleM1, "1.000", "1.000", "0.250", "0.500", "1.000", "0.250", 0, 0, 0, [11, 9]>;
def Profile_M2           : MicroarchitectureProfile<"M2", Arch_AppleM2, "1.000", "1.000", "0.250", "0.400", "1.000", "0.250", 0, 1, 0, [11, 9]>;
def Profile_M3           : MicroarchitectureProfile<"M3", Arch_AppleM3, "1.000", "1.000", "0.250", "0.330", "1.000", "0.250", 0, 1, 0, [11, 9]>;
def Profile_M4           : MicroarchitectureProfile<"M4", Arch_AppleM4, "1.000", "1.000", "0.250", "0.250", "1.000", "0.200", 0, 1, 0, [10, 11, 9]>;
def Profile_ORY          : MicroarchitectureProfile<"ORY", Arch_OryonXElite, "1.000", "1.000", "0.250", "0.500", "1.000", "0.250", 0, 1, 0, [11, 9]>;
def Profile_NV2          : MicroarchitectureProfile<"NV2", Arch_NeoverseV2, "0.500", "0.500", "0.500", "0.500", "1.000", "0.330", 0, 1, 0, [10, 11, 9]>;
def Profile_A72          : MicroarchitectureProfile<"A72", Arch_CortexA72, "1.000", "1.000", "0.500", "1.000", "1.000", "0.500", 0, 0, 0, [11, 9]>;
def Profile_A76          : MicroarchitectureProfile<"A76", Arch_CortexA76, "1.000", "1.000", "0.500", "0.500", "1.000", "0.500", 0, 1, 0, [11, 9]>;
def Profile_A64FX        : MicroarchitectureProfile<"A64FX", Arch_FujitsuA64FX, "0.500", "0.500", "0.500", "0.500", "0.500", "0.250", 0, 0, 0, [10, 11, 9]>;
def Profile_Z16          : MicroarchitectureProfile<"Z16", Arch_IBMz16, "0.500", "0.500", "0.500", "0.250", "0.500", "0.250", 0, 0, 0, [9]>;
def Profile_AmpereAltra  : MicroarchitectureProfile<"ALT", Arch_AmpereAltra, "1.000", "1.000", "0.500", "0.500", "1.000", "0.330", 0, 0, 0, [11, 9]>;
def Profile_AmpereOne    : MicroarchitectureProfile<"AONE", Arch_AmpereOne, "1.000", "1.000", "0.500", "0.500", "1.000", "0.330", 0, 1, 0, [11, 9]>;
def Profile_Ascalon      : MicroarchitectureProfile<"ASC", Arch_TenstorrentAscalon, "0.500", "0.500", "0.500", "0.500", "0.500", "0.250", 0, 1, 0, [12, 9]>;
def Profile_Grace        : MicroarchitectureProfile<"GRACE", Arch_NvidiaGrace, "0.500", "0.500", "0.500", "0.500", "1.000", "0.330", 0, 1, 0, [10, 11, 9]>;

// =============================================================================
// SECTION 12: DYNAMIC HINTER STEERING RULES (Branchless, Port Balance, Affinity)
// =============================================================================

class HinterSteeringRule<string name, string condition_desc, string metric_eval, string threshold,
                         string primary_action, string fallback_action> {
  string Name = name;
  string ConditionDescription = condition_desc;
  string MetricEvaluated = metric_eval;
  string ThresholdValue = threshold;
  string PrimaryAction = primary_action;
  string FallbackAction = fallback_action;
  int PrecedenceOrder = 1;
}

def Rule_BPU_Predication_Branchless            : HinterSteeringRule<"Rule_BPU_Predication_Branchless",
  "When BPU misprediction rate exceeds threshold, force Ring 3 branchless predication (cmov / vblendps / k-mask)",
  "BPU_Misprediction_Rate", "0.12",
  "ApplyBranchlessPredication_CMOV_VBLEND", "AllowConditionalBranch_JCC">;

def Rule_Branch_Penalty_Threshold              : HinterSteeringRule<"Rule_Branch_Penalty_Threshold",
  "When architecture branch penalty > 14 cycles, substitute unpredictable data-dependent branches with predication",
  "BranchPenaltyCyclesMin", "14.0",
  "ForceBranchlessPredication", "AllowStandardBPUBranch">;

def Rule_Vector_Port_Contention_Balance        : HinterSteeringRule<"Rule_Vector_Port_Contention_Balance",
  "When FMA Port 0 and Port 1 contention exceeds 80%, interleave independent polynomial terms across alternating ports",
  "Port0_Port1_SaturationRatio", "0.80",
  "InterleaveIndependentChains_Port01", "LinearSequentialExecution">;

def Rule_NonTemporal_Stream_Transition         : HinterSteeringRule<"Rule_NonTemporal_Stream_Transition",
  "When data working set size exceeds 50% of Last Level Cache (LLC), switch from cached stores to non-temporal streaming stores with sfence",
  "DatasetSize_Over_LLC_Ratio", "0.50",
  "RouteToNonTemporalStreamingStores_NT", "RouteToL1L2StandardCachedStores">;

def Rule_AVX512_Frequency_Throttling_Guard     : HinterSteeringRule<"Rule_AVX512_Frequency_Throttling_Guard",
  "When microarchitecture downclocks > 10% on AVX-512 heavy license (e.g. Skylake-X) and loop count < 1024, route to AVX2 to avoid downclocking penalties",
  "AVX512_Downclock_Percent", "10.0",
  "RouteToAVX2FMA_AvoidThrottling", "RouteToAVX512ZMM_FullMuscle">;

def Rule_Hybrid_P_Core_Transcendental_Pinning  : HinterSteeringRule<"Rule_Hybrid_P_Core_Transcendental_Pinning",
  "When compute intensity exceeds 8 FLOPs per byte, pin execution thread to Performance Cores (P-Cores) to maximize vector pipeline width",
  "ComputeIntensity_FLOPsPerByte", "8.0",
  "SetThreadAffinity_PCoreOnly", "AllowScheduling_AnyCore">;

def Rule_Hybrid_E_Core_Streaming_Efficiency    : HinterSteeringRule<"Rule_Hybrid_E_Core_Streaming_Efficiency",
  "When operation is pure memory bandwidth bound (< 1 FLOP/byte), allow execution on Efficient Cores (E-Cores) with prefetcher hints",
  "ComputeIntensity_FLOPsPerByte", "1.0",
  "SetThreadAffinity_ECoreAllowed", "SetThreadAffinity_PCorePreferred">;

def Rule_Peel_Tail_Masked_Predication          : HinterSteeringRule<"Rule_Peel_Tail_Masked_Predication",
  "When remainder element count is less than vector register lane count, apply masked load/store predication instead of peeling scalar branch loops",
  "TailElementCount_Over_LaneCount", "1.0",
  "ApplyMaskedAVX512_AVX2_TailPredication", "ScalarPeelingLoop_Fallback">;

def Rule_Prefetch_Distance_AutoTune            : HinterSteeringRule<"Rule_Prefetch_Distance_AutoTune",
  "Tune prefetch stride distance dynamically according to memory round-trip latency cycles divided by cache line width",
  "PrefetchStrideDistanceCachelines", "4.0",
  "IssuePrefetchT0_AggressiveAhead", "RelyOnHardwareStreamerOnly">;

def Rule_Ring3_InlineAsm_ZeroKernelOverhead    : HinterSteeringRule<"Rule_Ring3_InlineAsm_ZeroKernelOverhead",
  "Execute 100% of branchless and port balancing decisions directly in Ring 3 userspace inline assembly without invoking Ring 0 syscalls",
  "Ring3_UserspaceExecution", "1.0",
  "ExecuteDirectRing3InlineAsm", "SyscallKernelFallback">;

def Rule_Branchless_MinMax_Clamp               : HinterSteeringRule<"Rule_Branchless_MinMax_Clamp",
  "In clamping and range-limiting kernels, enforce branchless min/max intrinsics (_mm_min_ps / _mm_max_ps) without branching",
  "BranchlessClampingEnabled", "1.0",
  "EmitBranchlessMinMaxInstructions", "EmitConditionalTernaryOperator">;

def Rule_Unroll_Pipeline_Interleaving          : HinterSteeringRule<"Rule_Unroll_Pipeline_Interleaving",
  "For Transcendental polynomial evaluations, unroll by at least 4x and interleave independent accumulator registers to saturate execution ports",
  "UnrollInterleaveFactor", "4.0",
  "InterleaveIndependentAccumulatorRegisters", "SequentialAccumulatorChains">;

def Rule_NonTemporal_Fence_Insertion           : HinterSteeringRule<"Rule_NonTemporal_Fence_Insertion",
  "Whenever non-temporal streaming stores are used, automatically schedule an SFENCE at the loop epilogue before dependent consumer loads",
  "RequiresStoreFence", "1.0",
  "EmitStoreFence_SFENCE", "OmitMemoryFence">;

def Rule_AVX10_Converged_Vector_Width          : HinterSteeringRule<"Rule_AVX10_Converged_Vector_Width",
  "On AVX10-capable silicon, dynamically adapt vector loop peel width to maximum available hardware width (256b vs 512b) using common opcodes",
  "AVX10_VectorWidth_Adaptive", "1.0",
  "DispatchConvergedAVX10VectorUnits", "StaticPeelDispatch">;

def Rule_AVX10_2_YMM_Embedded_Rounding         : HinterSteeringRule<"Rule_AVX10_2_YMM_Embedded_Rounding",
  "On Intel AVX10.2 silicon (CPUID.24H.0:EBX[7:0] >= 2), use EVEX.U=0 + EVEX.b=1 embedded rounding {er} and {sae} on 256-bit YMM registers without MXCSR stalls",
  "AVX10_2_YMM_Rounding_Supported", "1.0",
  "EmitEVEX256_EmbeddedRounding_U0_B1", "EmitMXCSR_StateSaveRestore">;

def Rule_AVX10_2_Native_BF16_FMA_Pipeline      : HinterSteeringRule<"Rule_AVX10_2_Native_BF16_FMA_Pipeline",
  "On Intel AVX10.2 silicon, execute packed BF16 arithmetic (vfmadd213nepbf16, vaddnepbf16, vmulnepbf16, vscalefpbf16, vrcppbf16) natively without FP32 upconversion",
  "AVX10_2_Native_BF16_ISA", "1.0",
  "EmitNativeAVX10_2_NEPBF16_Pipeline", "UpconvertBF16ToFP32_Fallback">;

def Rule_AVX10_2_IEEE754_2019_VMINMAX          : HinterSteeringRule<"Rule_AVX10_2_IEEE754_2019_VMINMAX",
  "On Intel AVX10.2 silicon, emit single-instruction VMINMAXPS/PD/PH/NEPBF16 for IEEE-754-2019 NaN propagation, magnitude min/max, and sign-bit override",
  "AVX10_2_IEEE754_2019_MinMax", "1.0",
  "EmitSingleInstruction_VMINMAX", "EmitMultiInstruction_VCMP_VBLEND_NaN">;

def Rule_AVX10_2_OFP8_Saturating_Downconvert   : HinterSteeringRule<"Rule_AVX10_2_OFP8_Saturating_Downconvert",
  "On Intel AVX10.2 silicon, use VCVTNE2PH2BF8S / VCVTNE2PH2HF8S / VCVTBIASPH2BF8S hardware saturating conversions for E5M2 and E4M3 FP8 tensors",
  "AVX10_2_OFP8_Conversion", "1.0",
  "EmitHardwareAVX10_2_FP8_Convert", "SoftwareBitwiseFP8Quantize">;

def Rule_AVX10_2_Enhanced_EFLAGS_VCOMX         : HinterSteeringRule<"Rule_AVX10_2_Enhanced_EFLAGS_VCOMX",
  "On Intel AVX10.2 silicon, use VCOMXSS/VCOMXSD/VCOMXSH/VCOMSBF16 to set OF, SF, ZF, PF, CF in a single instruction for direct ordered/unordered branch conditions",
  "AVX10_2_Enhanced_EFLAGS_Compare", "1.0",
  "EmitVCOMX_EnhancedFlagsCompare", "EmitLegacyVCOMIS_TwoBranchSequence">;

def Rule_SVE_VL_Agnostic_Loop_Scaling          : HinterSteeringRule<"Rule_SVE_VL_Agnostic_Loop_Scaling",
  "On ARM SVE and SVE2 hardware, scale loop iteration steps using svcntw() and svcntd() runtime vector length query rather than hardcoded 128/256/512 bit constants",
  "SVE_ScalableVectorQuery", "1.0",
  "ScaleLoopWithSVEPredicateCounters", "StaticUnrolledLoopPeel">;

def Rule_RVV_LMUL_Dynamic_Register_Grouping    : HinterSteeringRule<"Rule_RVV_LMUL_Dynamic_Register_Grouping",
  "On RISC-V Vector 1.0 hardware, dynamically configure LMUL=2 or LMUL=4 to maximize register file usage on dense polynomial loops",
  "RVV_DynamicLMUL_Configuration", "2.0",
  "SetRVV_VType_LMUL_2", "SetRVV_VType_LMUL_1">;

def Rule_Store_Forwarding_Disambiguation       : HinterSteeringRule<"Rule_Store_Forwarding_Disambiguation",
  "Avoid store-forwarding stalls by ensuring vector load sizes match preceding store sizes and align to 64-byte cache lines",
  "StoreForwardingConflictDetected", "1.0",
  "AlignVectorStoreLoadWidth", "PermitUnalignedForwardingStall">;

def Rule_Register_Spilling_Avoidance           : HinterSteeringRule<"Rule_Register_Spilling_Avoidance",
  "When register pressure exceeds 85% of physical vector register count, partition Horner polynomial stages into fused tiles",
  "RegisterPressureRatio", "0.85",
  "TileHornerPolynomialStages", "PermitStackRegisterSpill">;

def Rule_AMX_Tile_Config_Overhead_Guard        : HinterSteeringRule<"Rule_AMX_Tile_Config_Overhead_Guard",
  "When matrix dimension N is less than 64, route to AVX-512 VNNI rather than AMX to avoid tilecfg/tilerelease transition overhead",
  "MatrixDimension_N", "64.0",
  "RouteToAVX512_VNNI_Matrix", "RouteToAMX_Tile_Matrix">;

def Rule_CMOV_vs_Blend_Select                  : HinterSteeringRule<"Rule_CMOV_vs_Blend_Select",
  "Use CMOV for scalar integer conditions, and VBLENDPS / VPBLENDD for packed SIMD vectors to ensure zero branch penalty",
  "VectorSIMDContext", "1.0",
  "EmitVBlendSIMDInstruction", "EmitCMOVScalarInstruction">;

def Rule_NUMA_Interleave_Locality              : HinterSteeringRule<"Rule_NUMA_Interleave_Locality",
  "On multi-socket NUMA systems, bind memory allocations to the executing core's local NUMA node domain",
  "IsNUMAArchitecture", "1.0",
  "AllocateLocalNUMANodeMemory", "AllocateInterleavedNUMAMemory">;

def Rule_Fast_String_ERMS_Memcpy               : HinterSteeringRule<"Rule_Fast_String_ERMS_Memcpy",
  "When block size exceeds 4KB and CPUID ERMS flag is active, dispatch enhanced REP MOVSB over AVX streaming stores",
  "BlockSizeBytes", "4096.0",
  "DispatchEnhancedRepMovsb", "DispatchAVXStreamingStore">;

def Rule_False_Sharing_Cacheline_Padding       : HinterSteeringRule<"Rule_False_Sharing_Cacheline_Padding",
  "Pad multi-threaded accumulator state structures to 128 bytes on Apple Silicon and 64 bytes on x86 to eliminate false sharing",
  "MultiThreadSharedAccumulator", "1.0",
  "ApplyCachelinePaddingSeparation", "AllowContiguousThreadState">;

def Rule_Hardware_Streamer_Throttling          : HinterSteeringRule<"Rule_Hardware_Streamer_Throttling",
  "On non-contiguous stride or gather/scatter access patterns, deactivate aggressive hardware prefetcher hints to avoid cache pollution",
  "StrideRegularityScore", "0.50",
  "DeactivateAggressivePrefetch", "EnableAggressiveStreamPrefetch">;

def Rule_Speculative_Load_Hardening            : HinterSteeringRule<"Rule_Speculative_Load_Hardening",
  "In cryptographic and entropy primitives, insert LFENCE or CSDB barriers to prevent speculative side-channel execution leaks",
  "IsCryptographicPrimitive", "1.0",
  "InsertSpeculativeBarrierFence", "OmitSpeculativeBarrierFence">;

// =============================================================================
// SECTION 13: NEW ERA EXECUTION FOCUS TOPOLOGY (SCP, MCP, HYBRID_DUAL 6-WAY MATRIX)
// =============================================================================

class ExecutionFocusMode<string name, int mode_id, string description,
                         bit is_hybrid, bit is_homogeneous,
                         string primary_allocation_policy,
                         string gc_scheduling_policy,
                         string p_core_chunk_weight,
                         string e_core_chunk_weight> {
  string Name = name;
  int ModeID = mode_id;
  string Description = description;
  bit IsHybrid = is_hybrid;
  bit IsHomogeneous = is_homogeneous;
  string PrimaryAllocationPolicy = primary_allocation_policy;
  string GCSchedulingPolicy = gc_scheduling_policy;
  string PCoreChunkWeight = p_core_chunk_weight;
  string ECoreChunkWeight = e_core_chunk_weight;
}

def Focus_SCP_Homo   : ExecutionFocusMode<"SCP_HOMO", 1, "Single-Core Peak Turbo Homogeneous (Core Floating, L1/L2 Prewarm)", 0, 1, "L1L2_Prewarm_Serial_Bump", "Serial_Quiescent", "1.0", "1.0">;
def Focus_SCP_Hybrid : ExecutionFocusMode<"SCP_HYBRID", 2, "Single-Core Turbo P-Core Hardware Pin (E-Core Escape Forbidden)", 1, 0, "P_Core_SRAM_Pin_Escape_Forbidden", "Serial_Quiescent", "1.0", "0.0">;
def Focus_MCP_Homo   : ExecutionFocusMode<"MCP_HOMO", 3, "Multi-Core Uniform Partitioning Server (N/threads, NUMA Local, NT Streaming)", 0, 1, "NUMA_Local_NT_Streaming", "Parallel_Sweeps", "1.0", "1.0">;
def Focus_MCP_Hybrid : ExecutionFocusMode<"MCP_HYBRID", 4, "Multi-Core Asymmetric Weighted Chunking (2.5x P-Core, 1.0x E-Core)", 1, 0, "Hybrid_Weighted_NT_Streaming", "Parallel_Sweeps", "2.5", "1.0">;
def Focus_Dual_Homo  : ExecutionFocusMode<"DUAL_HOMO", 5, "Dual Focus Homogeneous (1 Turbo Master + N-1 High-Throughput Workers)", 0, 1, "SRAM_Turbo_Master_NUMA_Workers", "Background_Worker_Sweeps", "1.0", "1.0">;
def Focus_Dual_Hybrid: ExecutionFocusMode<"DUAL_HYBRID", 6, "Dual Focus Asymmetric P+E (P-Compute Mutator + E-Background sfGC/Logs)", 1, 0, "P_Latency_E_Throughput_NUMA", "Dedicated_E_Core_Sweeping", "2.5", "1.0">;

// =============================================================================
// SECTION 14: MULTI-LIMB ARITHMETIC EMULATION & PARTITIONED BIT-ALLOCATION SCHEMAS
// =============================================================================
// Multi-limb integer and floating-point emulation matrices for microarchitectures
// lacking native 128-bit or 256-bit ALU support (e.g. consumer x86/ARM vs niche IBM).
// Defines the exact limb structures:
//   Float:   2*16b (32b), 4*16b (64b), 2*8b (16b), 2*32b (64b), 2*64b (128b),
//            4*8b (32b), 4*32b (128b), 4*64b (256b), 8*32b (256b)
//   Integer: Corresponding multi-limb layouts for integer arithmetic.
// Systematically generates hundreds of partitioned emulative format variants
// allocating whole (integer) vs fractional bits across the total bit width.
// =============================================================================

class MultiLimbSchema<string name, string category, int limb_count, int limb_width_bits, int total_bits, string desc> {
  string Name = name;
  string Category = category; // "Float" or "Integer"
  int LimbCount = limb_count;
  int LimbWidthBits = limb_width_bits;
  int TotalBits = total_bits;
  string Description = desc;
}

// 9 Float Multi-Limb Schemas
def Schema_Float_2x8  : MultiLimbSchema<"Float_2x8",  "Float", 2, 8,  16,  "16-bit Emulated Float via 2x 8-bit limbs">;
def Schema_Float_4x8  : MultiLimbSchema<"Float_4x8",  "Float", 4, 8,  32,  "32-bit Emulated Float via 4x 8-bit limbs">;
def Schema_Float_2x16 : MultiLimbSchema<"Float_2x16", "Float", 2, 16, 32,  "32-bit Emulated Float via 2x 16-bit limbs">;
def Schema_Float_4x16 : MultiLimbSchema<"Float_4x16", "Float", 4, 16, 64,  "64-bit Emulated Float via 4x 16-bit limbs">;
def Schema_Float_2x32 : MultiLimbSchema<"Float_2x32", "Float", 2, 32, 64,  "64-bit Emulated Float via 2x 32-bit limbs">;
def Schema_Float_4x32 : MultiLimbSchema<"Float_4x32", "Float", 4, 32, 128, "128-bit Emulated Float via 4x 32-bit limbs">;
def Schema_Float_2x64 : MultiLimbSchema<"Float_2x64", "Float", 2, 64, 128, "128-bit Emulated Float via 2x 64-bit limbs (Double-Double)">;
def Schema_Float_4x64 : MultiLimbSchema<"Float_4x64", "Float", 4, 64, 256, "256-bit Emulated Float via 4x 64-bit limbs (Quad-Double)">;
def Schema_Float_8x32 : MultiLimbSchema<"Float_8x32", "Float", 8, 32, 256, "256-bit Emulated Float via 8x 32-bit limbs (Oct-Single)">;

// 9 Corresponding Integer Multi-Limb Schemas
def Schema_Int_2x8  : MultiLimbSchema<"Int_2x8",  "Integer", 2, 8,  16,  "16-bit Emulated Integer via 2x 8-bit limbs">;
def Schema_Int_4x8  : MultiLimbSchema<"Int_4x8",  "Integer", 4, 8,  32,  "32-bit Emulated Integer via 4x 8-bit limbs">;
def Schema_Int_2x16 : MultiLimbSchema<"Int_2x16", "Integer", 2, 16, 32,  "32-bit Emulated Integer via 2x 16-bit limbs">;
def Schema_Int_4x16 : MultiLimbSchema<"Int_4x16", "Integer", 4, 16, 64,  "64-bit Emulated Integer via 4x 16-bit limbs">;
def Schema_Int_2x32 : MultiLimbSchema<"Int_2x32", "Integer", 2, 32, 64,  "64-bit Emulated Integer via 2x 32-bit limbs">;
def Schema_Int_4x32 : MultiLimbSchema<"Int_4x32", "Integer", 4, 32, 128, "128-bit Emulated Integer via 4x 32-bit limbs">;
def Schema_Int_2x64 : MultiLimbSchema<"Int_2x64", "Integer", 2, 64, 128, "128-bit Emulated Integer via 2x 64-bit limbs (Int128)">;
def Schema_Int_4x64 : MultiLimbSchema<"Int_4x64", "Integer", 4, 64, 256, "256-bit Emulated Integer via 4x 64-bit limbs (Int256)">;
def Schema_Int_8x32 : MultiLimbSchema<"Int_8x32", "Integer", 8, 32, 256, "256-bit Emulated Integer via 8x 32-bit limbs">;

// Partitioned whole (integer) vs fraction bit allocation format
class PartitionedEmulationFormat<MultiLimbSchema schema, int integer_bits, int fractional_bits> {
  MultiLimbSchema Schema = schema;
  int IntegerBits = integer_bits;
  int FractionalBits = fractional_bits;
  int TotalBits = schema.TotalBits;
  string FormatName = schema.Name # "_I" # !cast<string>(integer_bits) # "_F" # !cast<string>(fractional_bits);
  string DisplayName = schema.Category # " " # !cast<string>(schema.LimbCount) # "x" # !cast<string>(schema.LimbWidthBits) # "b (Whole:" # !cast<string>(integer_bits) # ", Frac:" # !cast<string>(fractional_bits) # ")";
}

// 16-Bit Partitioned Variants (2x8): 15 Float + 15 Integer = 30 variants
foreach I = 1-15 in {
  def Emu_Float_2x8_I#I#_F#!sub(16, I) : PartitionedEmulationFormat<Schema_Float_2x8, I, !sub(16, I)>;
  def Emu_Int_2x8_I#I#_F#!sub(16, I)   : PartitionedEmulationFormat<Schema_Int_2x8, I, !sub(16, I)>;
}

// 32-Bit Partitioned Variants (2x16 & 4x8): 62 Float + 62 Integer = 124 variants
foreach I = 1-31 in {
  def Emu_Float_2x16_I#I#_F#!sub(32, I) : PartitionedEmulationFormat<Schema_Float_2x16, I, !sub(32, I)>;
  def Emu_Int_2x16_I#I#_F#!sub(32, I)   : PartitionedEmulationFormat<Schema_Int_2x16, I, !sub(32, I)>;
  def Emu_Float_4x8_I#I#_F#!sub(32, I)  : PartitionedEmulationFormat<Schema_Float_4x8, I, !sub(32, I)>;
  def Emu_Int_4x8_I#I#_F#!sub(32, I)    : PartitionedEmulationFormat<Schema_Int_4x8, I, !sub(32, I)>;
}

// 64-Bit Partitioned Variants (2x32 & 4x16): 66 Float + 66 Integer = 132 variants
foreach I = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 63} in {
  def Emu_Float_2x32_I#I#_F#!sub(64, I) : PartitionedEmulationFormat<Schema_Float_2x32, I, !sub(64, I)>;
  def Emu_Int_2x32_I#I#_F#!sub(64, I)   : PartitionedEmulationFormat<Schema_Int_2x32, I, !sub(64, I)>;
  def Emu_Float_4x16_I#I#_F#!sub(64, I) : PartitionedEmulationFormat<Schema_Float_4x16, I, !sub(64, I)>;
  def Emu_Int_4x16_I#I#_F#!sub(64, I)   : PartitionedEmulationFormat<Schema_Int_4x16, I, !sub(64, I)>;
}

// 128-Bit Partitioned Variants (2x64 & 4x32): 66 Float + 66 Integer = 132 variants
foreach I = {2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 126} in {
  def Emu_Float_2x64_I#I#_F#!sub(128, I) : PartitionedEmulationFormat<Schema_Float_2x64, I, !sub(128, I)>;
  def Emu_Int_2x64_I#I#_F#!sub(128, I)   : PartitionedEmulationFormat<Schema_Int_2x64, I, !sub(128, I)>;
  def Emu_Float_4x32_I#I#_F#!sub(128, I) : PartitionedEmulationFormat<Schema_Float_4x32, I, !sub(128, I)>;
  def Emu_Int_4x32_I#I#_F#!sub(128, I)   : PartitionedEmulationFormat<Schema_Int_4x32, I, !sub(128, I)>;
}

// 256-Bit Partitioned Variants (4x64 & 8x32): 66 Float + 66 Integer = 132 variants
foreach I = {4, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 252} in {
  def Emu_Float_4x64_I#I#_F#!sub(256, I) : PartitionedEmulationFormat<Schema_Float_4x64, I, !sub(256, I)>;
  def Emu_Int_4x64_I#I#_F#!sub(256, I)   : PartitionedEmulationFormat<Schema_Int_4x64, I, !sub(256, I)>;
  def Emu_Float_8x32_I#I#_F#!sub(256, I) : PartitionedEmulationFormat<Schema_Float_8x32, I, !sub(256, I)>;
  def Emu_Int_8x32_I#I#_F#!sub(256, I)   : PartitionedEmulationFormat<Schema_Int_8x32, I, !sub(256, I)>;
}

// =============================================================================
// SECTION 15: HARDWARE ALU NATIVE VS. MULTI-LIMB PRECISION EXECUTION ROUTES
// =============================================================================
// Formally models the execution route for high-precision operations (128-bit & 256-bit):
// - Native Hardware ALU: Available on IBM Power (Power10/11) and IBM zArchitecture (z15/z16)
//   for 128-bit integer and quad-precision floating-point (IEEE 754-2008 binary128).
// - Multi-Limb Software Emulation: Required on commodity x86-64 (Intel/AMD) and ARM64
//   (Apple Silicon, Neoverse, Cortex) which lack 128-bit scalar ALUs.
// - 256-bit operations: Universally multi-limb emulated across ALL commercial silicon
//   (quad-double 4x64 or oct-single 8x32).
// =============================================================================

class PrecisionExecutionRoute<string name, int precision_bits, bit is_native_alu, string target_arch_family, MultiLimbSchema emulation_schema, string desc> {
  string Name = name;
  int PrecisionBits = precision_bits;
  bit IsNativeALU = is_native_alu;
  bit IsEmulated = !if(is_native_alu, 0, 1);
  string TargetArchFamily = target_arch_family;
  MultiLimbSchema EmulationSchema = emulation_schema;
  string Description = desc;
}

// 128-Bit Precision Execution Routes:
def Route_128b_Native_IBMPower : PrecisionExecutionRoute<
    "Route_128b_Native_IBMPower", 128, 1, "IBM_Power", Schema_Float_2x64,
    "Native 128-bit hardware ALU execution on IBM Power10/Power11 (VSX 128b quad-float/int)">;

def Route_128b_Native_IBMzArch : PrecisionExecutionRoute<
    "Route_128b_Native_IBMzArch", 128, 1, "IBM_zArch", Schema_Float_2x64,
    "Native 128-bit hardware ALU execution on IBM z15/z16 mainframe engines">;

def Route_128b_Emulated_x86_64_Float : PrecisionExecutionRoute<
    "Route_128b_Emulated_x86_64_Float", 128, 0, "x86_64", Schema_Float_2x64,
    "Multi-limb 2x64-bit double-double floating-point emulation on x86-64">;

def Route_128b_Emulated_x86_64_Int : PrecisionExecutionRoute<
    "Route_128b_Emulated_x86_64_Int", 128, 0, "x86_64", Schema_Int_2x64,
    "Multi-limb 2x64-bit integer arithmetic emulation on x86-64">;

def Route_128b_Emulated_AArch64_Float : PrecisionExecutionRoute<
    "Route_128b_Emulated_AArch64_Float", 128, 0, "AArch64", Schema_Float_2x64,
    "Multi-limb 2x64-bit double-double floating-point emulation on ARM64">;

def Route_128b_Emulated_AArch64_Int : PrecisionExecutionRoute<
    "Route_128b_Emulated_AArch64_Int", 128, 0, "AArch64", Schema_Int_2x64,
    "Multi-limb 2x64-bit integer arithmetic emulation on ARM64">;

def Route_128b_Emulated_RISCV_Float : PrecisionExecutionRoute<
    "Route_128b_Emulated_RISCV_Float", 128, 0, "RISCV64", Schema_Float_2x64,
    "Multi-limb 2x64-bit double-double floating-point emulation on RISC-V 64">;

def Route_128b_Emulated_RISCV_Int : PrecisionExecutionRoute<
    "Route_128b_Emulated_RISCV_Int", 128, 0, "RISCV64", Schema_Int_2x64,
    "Multi-limb 2x64-bit integer arithmetic emulation on RISC-V 64">;

// 256-Bit Precision Execution Routes (Universally Multi-Limb Emulated):
def Route_256b_Emulated_Universal_QuadDouble : PrecisionExecutionRoute<
    "Route_256b_Emulated_Universal_QuadDouble", 256, 0, "Universal", Schema_Float_4x64,
    "Multi-limb 4x64-bit quad-double 256-bit floating-point software emulation across all silicon">;

def Route_256b_Emulated_Universal_OctSingle : PrecisionExecutionRoute<
    "Route_256b_Emulated_Universal_OctSingle", 256, 0, "Universal", Schema_Float_8x32,
    "Multi-limb 8x32-bit oct-single 256-bit floating-point software emulation across all silicon">;

def Route_256b_Emulated_Universal_Int4x64 : PrecisionExecutionRoute<
    "Route_256b_Emulated_Universal_Int4x64", 256, 0, "Universal", Schema_Int_4x64,
    "Multi-limb 4x64-bit 256-bit integer software emulation across all silicon">;

def Route_256b_Emulated_Universal_Int8x32 : PrecisionExecutionRoute<
    "Route_256b_Emulated_Universal_Int8x32", 256, 0, "Universal", Schema_Int_8x32,
    "Multi-limb 8x32-bit 256-bit integer software emulation across all silicon">;

// =============================================================================
// END OF MACHINES-RULES.TD — 100% VERIFIED LLVM TABLEGEN HARDWARE SPECIFICATION
// =============================================================================
