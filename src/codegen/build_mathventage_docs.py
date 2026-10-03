# -*- coding: utf-8 -*-
"""
Script to assemble the complete, English-only, corporate Doxygen version of include/MathventageSIMD.h.
"""

import re
import docs_top_lifecycle
import docs_simd_multilimb
import docs_physics_math_advanced
import docs_hardware_extensions
import docs_sections_28_to_46
import docs_cpp_runtime

with open(r"C:\Users\aziml\my_projects\MathventageSIMD\include\MathventageSIMD.h", "r", encoding="utf-8") as f:
    text = f.read()

# Replace any lingering non-ASCII / Turkish artifacts
text = text.replace('\u2011', '-')  # non-breaking hyphen
text = text.replace('\u2014', '--') # em-dash
text = text.replace('Möller-Trumbore', 'Moller-Trumbore')

# 1. Top Section (Lines 1 to Marker 1)
marker_const = text.find('/* ========================================================================= */\n/* FUNDAMENTAL MATHEMATICAL CONSTANTS')
top_header = text[:marker_const]

# 2. Opaque Handles section (Marker 2 to Marker 3)
marker_handles = text.find('/* ========================================================================= */\n/* Opaque Handles')
marker_msc = text.find('/* MSC2020 Mathematical Disciplines Mapping')
handles_section = text[marker_handles:marker_msc]

# 3. Middle structs & types (Marker 4 to Marker 5)
marker_backend = text.find('typedef enum msimd_backend_t {')
marker_lifecycle = text.find('/* ========================================================================= */\n/* Context & Memory Lifecycle')
structs_section = text[marker_backend:marker_lifecycle]

# 4. Section 1 to Golden Seeds (Marker 6 to Marker 7)
marker_sec1 = text.find('/* ========================================================================= */\n/* SECTION 1: PURE LINEAR ALGEBRA & TENSOR MATHEMATICS')
marker_vec128 = text.find('/* ========================================================================= */\n/* 128-Bit & Sub-128-Bit Vector Acceleration Operations')
sec1_to_seeds = text[marker_sec1:marker_vec128]

# Assemble the file
assembled = []
assembled.append(top_header)
assembled.append(docs_top_lifecycle.TOP_CONSTANTS_ENGLISH)
assembled.append("\n\n")
assembled.append(handles_section)
assembled.append(docs_top_lifecycle.MSC_AND_TIERS_ENGLISH)
assembled.append("\n\n")
assembled.append(structs_section)
assembled.append(docs_top_lifecycle.LIFECYCLE_FUNCTIONS_DOC)
assembled.append("\n\n")
assembled.append(sec1_to_seeds)
assembled.append(docs_simd_multilimb.SIMD_MULTILIMB_DOCS)
assembled.append("\n\n")
assembled.append(docs_physics_math_advanced.PHYSICS_MATH_DOCS)
assembled.append("\n\n")
assembled.append(docs_hardware_extensions.HARDWARE_EXTENSIONS_DOCS)
assembled.append("\n\n")
assembled.append(docs_sections_28_to_46.SECTIONS_28_TO_46_DOCS)
assembled.append("\n\n")
assembled.append("""#ifdef __cplusplus
} /* extern "C" */
#endif

""")
assembled.append(docs_cpp_runtime.CPP_RUNTIME_DOCS)
assembled.append("\n\n#endif /* MATHVANTAGE_SIMD_C_API_H */\n")

final_content = "".join(assembled)

# Verify no non-ASCII characters remain
non_ascii = [(i, c) for i, c in enumerate(final_content) if ord(c) > 127]
print(f"Total non-ASCII characters in assembled header: {len(non_ascii)}")
if non_ascii:
    print("First 5 non-ASCII:", non_ascii[:5])

with open(r"C:\Users\aziml\my_projects\MathventageSIMD\include\MathventageSIMD.h", "w", encoding="utf-8") as f:
    f.write(final_content)

print(f"Successfully generated MathventageSIMD.h ({len(final_content)} bytes)")
