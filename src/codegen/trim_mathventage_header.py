# -*- coding: utf-8 -*-
import sys

with open("include/MathventageSIMD.h", "r", encoding="utf-8") as f:
    text = f.read()

# 1. Remove the Internal Architecture Context block (lines 2601 to 2651)
start_marker = "/* ========================================================================= */\n/* Internal Architecture Context & Backend Definitions (1 Single Header)     */"
end_marker = "/* ========================================================================= */\n/* Fast Exponentiation & Powers"

idx1 = text.find(start_marker)
idx2 = text.find(end_marker)

if idx1 != -1 and idx2 != -1:
    text = text[:idx1] + text[idx2:]
    print("Removed internal backend/leaf kernels section successfully!")
else:
    print("Failed to find internal markers:", idx1, idx2)
    sys.exit(1)

# 2. Remove the SFVM C++ block at the end (from #include <type_traits> to end)
sfvm_marker = "#include <type_traits>\n\n#include \"SacculusForge.h\""
idx_sfvm = text.find(sfvm_marker)
if idx_sfvm != -1:
    sfvm_code = text[idx_sfvm:]
    # Strip any trailing #endif from sfvm_code so it is purely the C++ classes
    if sfvm_code.endswith("#endif /* MATHVANTAGE_SIMD_C_API_H */\n"):
        sfvm_code = sfvm_code[:-len("#endif /* MATHVANTAGE_SIMD_C_API_H */\n")].rstrip() + "\n"
    elif sfvm_code.endswith("#endif /* MATHVANTAGE_SIMD_C_API_H */"):
        sfvm_code = sfvm_code[:-len("#endif /* MATHVANTAGE_SIMD_C_API_H */")].rstrip() + "\n"
        
    text = text[:idx_sfvm].rstrip() + "\n\n#endif /* MATHVANTAGE_SIMD_C_API_H */\n"
    print("Removed SFVM C++ runtime from MathventageSIMD.h! SFVM code length:", len(sfvm_code))
    with open("src/codegen/extracted_sfvm_cpp.inc", "w", encoding="utf-8") as sf_out:
        sf_out.write(sfvm_code)
else:
    print("Failed to find SFVM marker!")
    sys.exit(1)

with open("include/MathventageSIMD.h", "w", encoding="utf-8") as f:
    f.write(text)

print("Updated MathventageSIMD.h, new length in bytes:", len(text))
