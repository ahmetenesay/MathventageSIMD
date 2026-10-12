cmake_minimum_required(VERSION 3.20)

# =========================================================================
# HARDWARE & SILICON EXPLOITATION: Enforce LLVM Clang & Disable MSVC (cl.exe)
# =========================================================================
# =========================================================================
# UNIVERSAL LLVM DETECTION (Cross-Platform & Multi-Version Auto-Discovery)
# Resolves C:/Program Files/LLVM/*, ENV{LLVM_DIR}, ENV{LLVM_HOME}, PATH, Linux/macOS
# =========================================================================
if(NOT DEFINED DEFAULT_LLVM_DIR)
    set(LLVM_SEARCH_PATHS "")
    if(DEFINED ENV{LLVM_DIR})
        list(APPEND LLVM_SEARCH_PATHS "$ENV{LLVM_DIR}")
    endif()
    if(DEFINED ENV{LLVM_HOME})
        list(APPEND LLVM_SEARCH_PATHS "$ENV{LLVM_HOME}")
    endif()

    if(WIN32)
        # 1. Search C:/Program Files/LLVM/* (Sorted descending to grab newest version e.g. 22 > 19)
        file(GLOB LLVM_INSTALLED_DIRS "C:/Program Files/LLVM/*")
        if(LLVM_INSTALLED_DIRS)
            list(SORT LLVM_INSTALLED_DIRS ORDER DESCENDING)
            list(APPEND LLVM_SEARCH_PATHS ${LLVM_INSTALLED_DIRS})
        endif()
        # 2. Standard Windows Fallback Paths
        list(APPEND LLVM_SEARCH_PATHS
            "C:/Program Files/LLVM"
            "C:/LLVM"
            "D:/Program Files/LLVM"
            "D:/LLVM"
        )
    elseif(APPLE)
        list(APPEND LLVM_SEARCH_PATHS
            "/opt/homebrew/opt/llvm"
            "/usr/local/opt/llvm"
        )
    else()
        file(GLOB LINUX_LLVM_DIRS "/usr/lib/llvm-*")
        if(LINUX_LLVM_DIRS)
            list(SORT LINUX_LLVM_DIRS ORDER DESCENDING)
            list(APPEND LLVM_SEARCH_PATHS ${LINUX_LLVM_DIRS})
        endif()
        list(APPEND LLVM_SEARCH_PATHS "/usr/lib/llvm" "/usr/local/llvm")
    endif()

    foreach(candidate ${LLVM_SEARCH_PATHS})
        if(EXISTS "${candidate}/bin/clang-cl.exe" OR EXISTS "${candidate}/bin/clang.exe" OR EXISTS "${candidate}/bin/clang")
            set(DEFAULT_LLVM_DIR "${candidate}")
            break()
        endif()
    endforeach()
endif()

if(DEFAULT_LLVM_DIR)
    message(STATUS "Universal LLVM Root Resolved: ${DEFAULT_LLVM_DIR}")
endif()

if(WIN32)
    # Default to LLVM clang-cl / clang and lld-link if not explicitly overridden
    if(NOT DEFINED CMAKE_C_COMPILER AND DEFAULT_LLVM_DIR AND EXISTS "${DEFAULT_LLVM_DIR}/bin/clang-cl.exe")
        set(CMAKE_C_COMPILER "${DEFAULT_LLVM_DIR}/bin/clang-cl.exe" CACHE FILEPATH "C compiler" FORCE)
    endif()
    if(NOT DEFINED CMAKE_CXX_COMPILER AND DEFAULT_LLVM_DIR AND EXISTS "${DEFAULT_LLVM_DIR}/bin/clang-cl.exe")
        set(CMAKE_CXX_COMPILER "${DEFAULT_LLVM_DIR}/bin/clang-cl.exe" CACHE FILEPATH "C++ compiler" FORCE)
    endif()
    if(NOT DEFINED CMAKE_LINKER AND DEFAULT_LLVM_DIR AND EXISTS "${DEFAULT_LLVM_DIR}/bin/lld-link.exe")
        set(CMAKE_LINKER "${DEFAULT_LLVM_DIR}/bin/lld-link.exe" CACHE FILEPATH "Linker" FORCE)
    endif()
    if(NOT DEFINED CMAKE_RC_COMPILER AND DEFAULT_LLVM_DIR AND EXISTS "${DEFAULT_LLVM_DIR}/bin/llvm-rc.exe")
        set(CMAKE_RC_COMPILER "${DEFAULT_LLVM_DIR}/bin/llvm-rc.exe" CACHE FILEPATH "RC compiler" FORCE)
    endif()
endif()

project(MathventageSIMD
    VERSION 1.0.0
    DESCRIPTION "100% Pure CPU SIMD Mathematical, Spatial & Cryptographic Acceleration Engine"
    LANGUAGES C CXX
)

# =========================================================================
# LLVM DIAGNOSTIC AUDIT & VERIFICATION REPORTING
# =========================================================================
# Report 1: LLVM Installation Completely Missing
# Report 2: LLVM Found, but Incomplete / Missing Critical Development Components
# =========================================================================

if(NOT DEFAULT_LLVM_DIR)
    message(FATAL_ERROR
        "\n================================================================================\n"
        "[FATAL ERROR: LLVM INSTALLATION NOT FOUND]\n"
        "CMake was unable to detect any valid LLVM/Clang installation on this system!\n"
        "Checked Locations:\n"
        "  - Environment Variables : LLVM_DIR, LLVM_HOME, PATH\n"
        "  - Windows Default Paths : C:/Program Files/LLVM/*, C:/LLVM, D:/Program Files/LLVM\n"
        "  - Linux / macOS Paths   : /usr/lib/llvm-*, /usr/local/opt/llvm, /opt/homebrew/opt/llvm\n"
        "\n"
        "How to resolve this:\n"
        "  1. Install LLVM with Clang (Recommended: LLVM 18+ or 22+ x86_64).\n"
        "  2. Or explicitly pass the LLVM root path to CMake:\n"
        "     cmake -B build -G \"Ninja\" -DDEFAULT_LLVM_DIR=\"C:/Path/To/Your/LLVM\"\n"
        "================================================================================\n"
    )
else()
    set(LLVM_AUDIT_PASSED TRUE)
    set(LLVM_FOUND_LIST "")
    set(LLVM_MISSING_LIST "")

    # 1. Compiler Component Audit (clang-cl on Windows, clang++ on Unix)
    if(WIN32)
        if(EXISTS "${DEFAULT_LLVM_DIR}/bin/clang-cl.exe")
            list(APPEND LLVM_FOUND_LIST "  [+] C/C++ Compiler     : FOUND (${DEFAULT_LLVM_DIR}/bin/clang-cl.exe)")
        else()
            set(LLVM_AUDIT_PASSED FALSE)
            list(APPEND LLVM_MISSING_LIST "  [-] clang-cl.exe       : MISSING (Required for Windows Clang-CL driver)")
        endif()
    else()
        if(EXISTS "${DEFAULT_LLVM_DIR}/bin/clang++")
            list(APPEND LLVM_FOUND_LIST "  [+] C/C++ Compiler     : FOUND (${DEFAULT_LLVM_DIR}/bin/clang++)")
        else()
            set(LLVM_AUDIT_PASSED FALSE)
            list(APPEND LLVM_MISSING_LIST "  [-] clang++            : MISSING (Required for compiling C++)")
        endif()
    endif()

    # 2. TableGen Component Audit (llvm-tblgen)
    if(EXISTS "${DEFAULT_LLVM_DIR}/bin/llvm-tblgen.exe" OR EXISTS "${DEFAULT_LLVM_DIR}/bin/llvm-tblgen")
        list(APPEND LLVM_FOUND_LIST "  [+] TableGen Compiler  : FOUND (llvm-tblgen)")
    else()
        set(LLVM_AUDIT_PASSED FALSE)
        list(APPEND LLVM_MISSING_LIST "  [-] llvm-tblgen        : MISSING (Required for TableGen Cartesian Cross-Product)")
    endif()

    # 3. Linker Component Audit (lld-link on Windows, lld/ld.lld on Unix)
    if(WIN32)
        if(EXISTS "${DEFAULT_LLVM_DIR}/bin/lld-link.exe")
            list(APPEND LLVM_FOUND_LIST "  [+] Fast Linker        : FOUND (lld-link.exe)")
        else()
            list(APPEND LLVM_MISSING_LIST "  [!] lld-link.exe       : OPTIONAL/MISSING (Linker will fallback to MSVC link.exe)")
        endif()
    else()
        if(EXISTS "${DEFAULT_LLVM_DIR}/bin/ld.lld" OR EXISTS "${DEFAULT_LLVM_DIR}/bin/lld")
            list(APPEND LLVM_FOUND_LIST "  [+] Fast Linker        : FOUND (lld)")
        else()
            list(APPEND LLVM_MISSING_LIST "  [!] ld.lld             : OPTIONAL/MISSING (Linker will fallback to system ld)")
        endif()
    endif()

    # 4. LLVM Development Headers Audit (include/llvm)
    if(EXISTS "${DEFAULT_LLVM_DIR}/include/llvm" OR EXISTS "${DEFAULT_LLVM_DIR}/include/llvm-c")
        list(APPEND LLVM_FOUND_LIST "  [+] LLVM C/C++ Headers : FOUND (${DEFAULT_LLVM_DIR}/include)")
    else()
        set(LLVM_AUDIT_PASSED FALSE)
        list(APPEND LLVM_MISSING_LIST "  [-] include/llvm       : MISSING (Runtime-only distribution without dev headers)")
    endif()

    # 5. LLVM CMake Configuration Audit (lib/cmake/llvm)
    if(EXISTS "${DEFAULT_LLVM_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
        list(APPEND LLVM_FOUND_LIST "  [+] CMake Modules      : FOUND (${DEFAULT_LLVM_DIR}/lib/cmake/llvm)")
    else()
        list(APPEND LLVM_MISSING_LIST "  [!] LLVMConfig.cmake   : MISSING (find_package(LLVM) may require fallback)")
    endif()

    string(JOIN "\n" LLVM_FOUND_FMT ${LLVM_FOUND_LIST})
    string(JOIN "\n" LLVM_MISSING_FMT ${LLVM_MISSING_LIST})

    if(NOT LLVM_AUDIT_PASSED)
        message(FATAL_ERROR
            "\n================================================================================\n"
            "[FATAL ERROR: INCOMPLETE LLVM INSTALLATION DETECTED]\n"
            "CMake resolved an LLVM root directory at:\n"
            "  ${DEFAULT_LLVM_DIR}\n\n"
            "However, this installation lacks essential developer tools or headers required\n"
            "to compile MathventageSIMD! (Many pre-packaged installers only include clang binary).\n\n"
            "Component Audit Breakdown:\n"
            "${LLVM_FOUND_FMT}\n"
            "${LLVM_MISSING_FMT}\n\n"
            "How to resolve this:\n"
            "  1. Make sure you install a full LLVM Development SDK (not a minimal runtime).\n"
            "  2. On Windows: ensure 'Development libraries' and 'Tools' (llvm-tblgen) are installed.\n"
            "  3. On Linux: sudo apt install llvm-dev clang lld (or pacman -S llvm clang lld)\n"
            "  4. Or point CMake to a complete LLVM SDK: -DDEFAULT_LLVM_DIR=\"<path>\"\n"
            "================================================================================\n"
        )
    else()
        message(STATUS
            "\n================================================================================\n"
            "[LLVM INSTALLATION AUDIT: FULL SDK VERIFIED]\n"
            "Root: ${DEFAULT_LLVM_DIR}\n"
            "${LLVM_FOUND_FMT}\n"
            "${LLVM_MISSING_FMT}\n"
            "================================================================================\n"
        )
    endif()
endif()

# -------------------------------------------------------------------------
# HARD DISABLE: Reject Pure Microsoft MSVC (cl.exe)
# -------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR
        "\n=========================================================================\n"
        "[FATAL ERROR] Microsoft MSVC (cl.exe) is explicitly DISABLED for MathventageSIMD!\n"
        "MathventageSIMD requires LLVM Clang for advanced vectorization, ORCJIT, and\n"
        "W^X JITLink codegen.\n"
        "Configured LLVM path: ${DEFAULT_LLVM_DIR}/bin/clang-cl.exe\n"
        "How to build with Clang:\n"
        "  - Visual Studio Generator: pass '-T ClangCL'\n"
        "  - Ninja / CLI Generator   : pass '-DCMAKE_CXX_COMPILER=\"${DEFAULT_LLVM_DIR}/bin/clang-cl.exe\"'\n"
        "=========================================================================\n"
    )
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Determine OS and Architecture string
string(TOLOWER "${CMAKE_SYSTEM_NAME}" MSIMD_OS)
if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Darwin|MacOS")
    set(MSIMD_OS "macos")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

if(CMAKE_GENERATOR_PLATFORM)
    set(SYS_ARCH "${CMAKE_GENERATOR_PLATFORM}")
else()
    set(SYS_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()
if(CMAKE_CXX_COMPILER MATCHES "aarch64|arm64")
    set(SYS_ARCH "arm64")
endif()
string(TOLOWER "${SYS_ARCH}" MSIMD_ARCH)

if(MSIMD_ARCH MATCHES "amd64|x86_64|x64")
    set(MSIMD_ARCH "x64")
elseif(MSIMD_ARCH MATCHES "arm64|aarch64")
    set(MSIMD_ARCH "arm64")
endif()
if(NOT DEFINED NATIVES_OUT_DIR)
    set(NATIVES_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/${MSIMD_OS}-${MSIMD_ARCH}")
endif()
set(SDK_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

# Autodetect LLVM, OpenVINO and oneTBB on Windows if installed
set(DEFAULT_OPENVINO_DIR "C:/Intel/openvino_toolkit_windows_2025.4.0.20398.8fdad55727d_x86_64")
set(DEFAULT_TBB_DIR "C:/Program Files (x86)/Intel/oneAPI/tbb/latest")

if(WIN32 AND MSIMD_ARCH STREQUAL "x64")
    if(EXISTS "${DEFAULT_LLVM_DIR}/lib/cmake/llvm")
        list(APPEND CMAKE_PREFIX_PATH "${DEFAULT_LLVM_DIR}/lib/cmake/llvm")
    endif()
    if(EXISTS "${DEFAULT_OPENVINO_DIR}/runtime/cmake")
        list(APPEND CMAKE_PREFIX_PATH "${DEFAULT_OPENVINO_DIR}/runtime/cmake")
    endif()
    if(EXISTS "${DEFAULT_TBB_DIR}/lib/cmake/tbb")
        list(APPEND CMAKE_PREFIX_PATH "${DEFAULT_TBB_DIR}/lib/cmake/tbb")
    endif()
endif()

set(CODEGEN_INTERMEDIATE_CPP "${CMAKE_CURRENT_BINARY_DIR}/TableGen_Derived.cpp")
set(CODEGEN_OUTPUT_CPP      "${CMAKE_CURRENT_BINARY_DIR}/MathventageSIMD_Derived.cpp")

set(SOURCES
    "${CODEGEN_OUTPUT_CPP}"
    src/SacFor_VM.cpp
)
set(HEADERS
    include/MathventageSIMD.h
)

add_library(mathvantage SHARED ${SOURCES} ${HEADERS})

# Apple Accelerate Framework
if(APPLE)
    find_library(ACCELERATE_FRAMEWORK Accelerate)
    if(ACCELERATE_FRAMEWORK)
        target_link_libraries(mathvantage PRIVATE ${ACCELERATE_FRAMEWORK})
        target_compile_definitions(mathvantage PRIVATE USE_APPLE_ACCELERATE)
        message(STATUS "Apple Accelerate Framework linked.")
    endif()
endif()

# =========================================================================
# ACCELERATED ENGINE CONTROLS & STANDALONE RESILIENCE
# =========================================================================
option(ENABLE_TBB "Enable Intel oneTBB acceleration" ON)
option(ENABLE_OPENVINO "Enable OpenVINO runtime acceleration" ON)
option(MATHVANTAGE_STANDALONE "Build 100% self-contained binary with ZERO external DLL dependencies" OFF)

if(MATHVANTAGE_STANDALONE)
    set(ENABLE_TBB OFF CACHE BOOL "Force disable TBB for standalone build" FORCE)
    set(ENABLE_OPENVINO OFF CACHE BOOL "Force disable OpenVINO for standalone build" FORCE)
    message(STATUS "[STANDALONE BUILD] TBB and OpenVINO disabled. Pure native C++20 thread pool and SIMD kernels enabled.")
endif()

# TBB and OpenVINO
if(ENABLE_TBB)
    find_package(TBB QUIET)
    if((WIN32 OR APPLE) AND MSIMD_ARCH MATCHES "arm64")
        if(NOT TBB_FOUND)
            set(TBB_FOUND FALSE)
        endif()
    endif()
    if(TBB_FOUND)
        target_link_libraries(mathvantage PRIVATE TBB::tbb)
        target_compile_definitions(mathvantage PRIVATE USE_TBB)
        message(STATUS "Intel TBB Found. Flow Graph Pipeline enabled.")
        if(WIN32)
            target_link_libraries(mathvantage PRIVATE delayimp.lib)
            target_link_options(mathvantage PRIVATE "/DELAYLOAD:tbb12.dll")
            target_link_options(mathvantage PRIVATE "/DELAYLOAD:tbb12_debug.dll")
        endif()
    else()
        message(WARNING "Intel TBB not found. Falling back to standard OS Threads/Scalar execution.")
    endif()
endif()

if(ENABLE_OPENVINO)
    find_package(OpenVINO QUIET)
    if((WIN32 OR APPLE) AND MSIMD_ARCH MATCHES "arm64")
        set(OpenVINO_FOUND FALSE)
    endif()
    if(OpenVINO_FOUND)
        target_link_libraries(mathvantage PRIVATE openvino::runtime)
        target_compile_definitions(mathvantage PRIVATE USE_OPENVINO)
        message(STATUS "OpenVINO Found. AI Scheduler enabled.")
        if(WIN32)
            target_link_libraries(mathvantage PRIVATE delayimp.lib)
            target_link_options(mathvantage PRIVATE "/DELAYLOAD:openvino.dll")
        endif()
    else()
        message(WARNING "OpenVINO not found. AI Scheduler will fallback to static CPU heuristics.")
    endif()
endif()

# =========================================================================
# LLVM ORCJIT, Passes & Static Embedding Configuration
# =========================================================================
find_package(LLVM CONFIG QUIET)
if(NOT LLVM_FOUND AND EXISTS "${DEFAULT_LLVM_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
    get_filename_component(LLVM_DIR "${DEFAULT_LLVM_DIR}/lib/cmake/llvm" ABSOLUTE)
    find_package(LLVM CONFIG QUIET PATHS "${LLVM_DIR}" NO_DEFAULT_PATH)
endif()

if(LLVM_FOUND)
    message(STATUS "LLVM ${LLVM_PACKAGE_VERSION} Found: ${LLVM_DIR}")
    target_include_directories(mathvantage PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(mathvantage PRIVATE SACFOR_HAS_LLVM)

    # LLVM Compile definitions (-D_CRT_SECURE_NO_WARNINGS, etc.)
    if(LLVM_DEFINITIONS)
        separate_arguments(LLVM_DEFS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
        target_compile_options(mathvantage PRIVATE ${LLVM_DEFS_LIST})
    endif()

    # Map required LLVM static components
    llvm_map_components_to_libnames(LLVM_REQ_LIBS
        orcjit
        jitlink
        bitreader
        passes
        x86codegen
        support
        core
        analysis
        target
        mc
        mcparser
    )

    if(TARGET LLVMDebugInfoPDB)
        get_target_property(PDB_LIBS LLVMDebugInfoPDB INTERFACE_LINK_LIBRARIES)
        if(PDB_LIBS)
            list(FILTER PDB_LIBS EXCLUDE REGEX "diaguids\\.lib")
            set_target_properties(LLVMDebugInfoPDB PROPERTIES INTERFACE_LINK_LIBRARIES "${PDB_LIBS}")
        endif()
    endif()

    # In MSVC/Clang-CL, match LLVM's static MT runtime and embed static libraries
    if(MSVC)
        # LLVM 22.1.5 on this system is built with MultiThreaded (/MT, _ITERATOR_DEBUG_LEVEL=0)
        set_property(TARGET mathvantage PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
        target_compile_definitions(mathvantage PRIVATE _ITERATOR_DEBUG_LEVEL=0)
        # Windows system dependencies required by static LLVMSupport
        target_link_libraries(mathvantage PRIVATE
            ${LLVM_REQ_LIBS}
            version
            ntdll
            ws2_32
            psapi
            shlwapi
            ole32
            advapi32
        )
    else()
        target_link_libraries(mathvantage PRIVATE ${LLVM_REQ_LIBS})
    endif()
    message(STATUS "LLVM Static Libs embedded: ${LLVM_REQ_LIBS}")
else()
    message(STATUS "LLVM not linked. SacFor VM will operate in AOT/Puzzle mode.")
endif()

# =========================================================================
# AUTONOMOUS IN-MEMORY CODEGEN & BITCODE SYNTHESIZER
# =========================================================================
find_package(Python3 COMPONENTS Interpreter QUIET)
if(NOT Python3_EXECUTABLE)
    find_program(Python3_EXECUTABLE NAMES py python3 python)
endif()

set(CODEGEN_GUIDES          "${CMAKE_CURRENT_SOURCE_DIR}/src/rules/GUIDES.cgr")
set(CODEGEN_ASSEMBLER       "${CMAKE_CURRENT_SOURCE_DIR}/src/rules/Assembler.td")
set(CODEGEN_MATH            "${CMAKE_CURRENT_SOURCE_DIR}/src/rules/Mathematics-Rules.td")
set(CODEGEN_MACHINES        "${CMAKE_CURRENT_SOURCE_DIR}/src/rules/Machines-Rules.td")
set(CODEGEN_UOPS            "${CMAKE_CURRENT_SOURCE_DIR}/src/codegen/instructions.xml")
set(CODEGEN_INTERMEDIATE_CPP "${CMAKE_CURRENT_BINARY_DIR}/TableGen_Derived.cpp")
set(CODEGEN_OUTPUT_CPP      "${CMAKE_CURRENT_BINARY_DIR}/MathventageSIMD_Derived.cpp")
set(CODEGEN_HEADER          "${CMAKE_CURRENT_SOURCE_DIR}/include/MathventageSIMD.h")

# Locate LLVM TableGen (llvm-tblgen.exe)
set(DEFAULT_LLVM_TBLGEN "${DEFAULT_LLVM_DIR}/bin/llvm-tblgen.exe")
find_program(LLVM_TABLEGEN_EXE NAMES llvm-tblgen HINTS "${DEFAULT_LLVM_DIR}/bin")
if(NOT LLVM_TABLEGEN_EXE AND EXISTS "${DEFAULT_LLVM_TBLGEN}")
    set(LLVM_TABLEGEN_EXE "${DEFAULT_LLVM_TBLGEN}" CACHE FILEPATH "LLVM TableGen executable" FORCE)
endif()
if(LLVM_TABLEGEN_EXE)
    message(STATUS "LLVM TableGen compiler detected: ${LLVM_TABLEGEN_EXE}")
else()
    message(WARNING "llvm-tblgen not found in ${DEFAULT_LLVM_DIR}/bin. TableGen validation will use internal parser.")
endif()

if(Python3_EXECUTABLE)
    set(CODEGEN_AST_JSON "${CMAKE_CURRENT_BINARY_DIR}/TableGen_AST.json")

    # Stage 1a: TableGen AST Generation via Direct Native llvm-tblgen.exe
    if(LLVM_TABLEGEN_EXE)
        add_custom_command(
            OUTPUT "${CODEGEN_AST_JSON}"
            COMMAND "${LLVM_TABLEGEN_EXE}"
                "${CODEGEN_ASSEMBLER}"
                -I="${CMAKE_CURRENT_SOURCE_DIR}/src/rules"
                --dump-json
                -o "${CODEGEN_AST_JSON}"
            DEPENDS
                "${CODEGEN_ASSEMBLER}"
                "${CODEGEN_MATH}"
                "${CODEGEN_MACHINES}"
                "${LLVM_TABLEGEN_EXE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "[MathventageSIMD] Stage 1a/3: LLVM TableGen compilation & cross-product (Assembler.td -> TableGen_AST.json)..."
        )
    endif()

    # Stage 1b: Master Public Header & Accelerated SIMD Codex Synthesis
    # TableGen AST Contract (TableGen_AST.json) -> 1 H (include/MathventageSIMD.h) + 1 C++ (build/MathventageSIMD_Derived.cpp)
    add_custom_command(
        OUTPUT
            "${CODEGEN_HEADER}"
            "${CODEGEN_OUTPUT_CPP}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/src/codegen/codegen.py"
            --ast-json "${CODEGEN_AST_JSON}"
            --guides "${CODEGEN_GUIDES}"
            --uops "${CODEGEN_UOPS}"
            --output-h "${CODEGEN_HEADER}"
            --output-cpp "${CODEGEN_OUTPUT_CPP}"
            --no-verify
        DEPENDS
            "${CODEGEN_AST_JSON}"
            "${CODEGEN_GUIDES}"
            "${CODEGEN_UOPS}"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/codegen/codegen.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "[MathventageSIMD] Stage 1b/2: TableGen AST Ingestion & Deep Silicon Synthesis (TableGen_AST.json -> MathventageSIMD.h + MathventageSIMD_Derived.cpp)..."
    )

    add_custom_target(generate_mathvantage_sources
        DEPENDS
            "${CODEGEN_OUTPUT_CPP}"
            "${CODEGEN_HEADER}"
    )

    # Stage 2: LLVM Bitcode Target (Mathventory.bc compiled directly via Clang)
    find_program(CLANG_COMPILER_EXE NAMES clang++ clang clang-cl HINTS "${DEFAULT_LLVM_DIR}/bin")
    if(CLANG_COMPILER_EXE)
        set(MATHVENTORY_BC_OUT "${CMAKE_CURRENT_BINARY_DIR}/Mathventory.bc")
        set(BC_ARCH_FLAGS "")
        if(MSIMD_ARCH MATCHES "x64|x86_64|amd64")
            if(CLANG_COMPILER_EXE MATCHES "clang-cl")
                set(BC_ARCH_FLAGS /arch:AVX2 /clang:-mavx2 /clang:-mfma)
            else()
                set(BC_ARCH_FLAGS -mavx2 -mfma)
            endif()
        endif()
        if(CLANG_COMPILER_EXE MATCHES "clang-cl")
            set(CLANG_BC_FLAGS /O2 /std:c++20 ${BC_ARCH_FLAGS} /clang:-emit-llvm /clang:-c /DMATHVANTAGE_EXPORTS /clang:-o /clang:"${MATHVENTORY_BC_OUT}")
        else()
            set(CLANG_BC_FLAGS -O3 -std=c++20 ${BC_ARCH_FLAGS} -emit-llvm -c -DMATHVANTAGE_EXPORTS -o "${MATHVENTORY_BC_OUT}")
        endif()

        add_custom_command(
            OUTPUT "${MATHVENTORY_BC_OUT}"
            COMMAND "${CLANG_COMPILER_EXE}"
                ${CLANG_BC_FLAGS}
                -I"${CMAKE_CURRENT_SOURCE_DIR}/include"
                -I"${CMAKE_CURRENT_SOURCE_DIR}/src"
                -I"${CMAKE_CURRENT_BINARY_DIR}"
                "${CODEGEN_OUTPUT_CPP}"
            DEPENDS
                "${CODEGEN_OUTPUT_CPP}"
                "${CODEGEN_HEADER}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "[MathventageSIMD] Stage 3/3: Compiling LLVM Bitcode artifact Mathventory.bc via Clang..."
        )

        add_custom_target(generate_mathventory_bc
            DEPENDS "${MATHVENTORY_BC_OUT}"
        )
    endif()
endif()

set_target_properties(mathvantage PROPERTIES
    PREFIX "lib"
    OUTPUT_NAME "sfmsimd"
    LIBRARY_OUTPUT_DIRECTORY "${NATIVES_OUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${NATIVES_OUT_DIR}"
)

target_include_directories(mathvantage PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_compile_definitions(mathvantage PRIVATE MATHVANTAGE_EXPORTS)
if(TARGET generate_mathvantage_sources)
    add_dependencies(mathvantage generate_mathvantage_sources)
endif()
if(TARGET generate_mathventory_bc)
    add_dependencies(mathvantage generate_mathventory_bc)
endif()

# =========================================================================
# UNIVERSAL SYSTEM CONCURRENCY (Auto-Scales to 100% of Host CPU Cores)
# =========================================================================
cmake_host_system_information(RESULT HOST_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
if(NOT HOST_LOGICAL_CORES OR HOST_LOGICAL_CORES LESS 1)
    set(HOST_LOGICAL_CORES 4)
endif()
message(STATUS "Host hardware concurrency detected: ${HOST_LOGICAL_CORES} logical cores")

if(NOT DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
    set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${HOST_LOGICAL_CORES}")
endif()

# Compiler Optimizations (LLVM Clang / Clang-CL Tuned)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Configuring optimizations for LLVM Clang: ${CMAKE_CXX_COMPILER}")
    if(CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC" OR MSVC)
        # Clang-CL Driver with multi-process translation unit compilation (/MP auto-saturates all host cores)
        target_compile_options(mathvantage PRIVATE
            /O2 /Oi /Ot /fp:fast /EHsc /bigobj /permissive-
            $<$<BOOL:${MSVC_IDE}>:/MP>
            /clang:-O3
            /clang:-ffast-math
        )
        if(MSIMD_ARCH STREQUAL "x64")
            target_compile_options(mathvantage PRIVATE
                /arch:AVX2
                /clang:-mavx2
                /clang:-mfma
            )
        endif()

        # Multi-Tool Task execution for Visual Studio MSBuild
        set_property(TARGET mathvantage PROPERTY VS_GLOBAL_UseMultiToolTask true)
        set_property(TARGET mathvantage PROPERTY VS_GLOBAL_EnforceProcessCountInBuildMessages true)
    else()
        # Pure Clang++ Driver
        target_compile_options(mathvantage PRIVATE -O3 -ffast-math -fvisibility=hidden -Wall -Wextra)
        if(MSIMD_ARCH STREQUAL "x64")
            target_compile_options(mathvantage PRIVATE -mavx2 -mfma)
        endif()
    endif()

    # Fast Linker: LLVM lld-link scaling to all available worker threads
    if(WIN32 AND DEFAULT_LLVM_DIR AND EXISTS "${DEFAULT_LLVM_DIR}/bin/lld-link.exe")
        target_link_options(mathvantage PRIVATE /threads:${HOST_LOGICAL_CORES})
    endif()

else()
    target_compile_options(mathvantage PRIVATE -O3 -ffast-math -fvisibility=hidden -Wall -Wextra)
    if(MSIMD_ARCH STREQUAL "x64")
        target_compile_options(mathvantage PRIVATE -mavx2 -mfma)
    endif()
endif()

# =========================================================================
# AUTO-DEPLOYMENT: Copy Dependency DLLs into Resources
# =========================================================================
if(WIN32 AND MSIMD_ARCH STREQUAL "x64")
    add_custom_command(TARGET mathvantage POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DEFAULT_OPENVINO_DIR}/runtime/bin/intel64/Release"
            "${NATIVES_OUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DEFAULT_TBB_DIR}/bin/tbb12.dll"
            "${NATIVES_OUT_DIR}/tbb12.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DEFAULT_TBB_DIR}/bin/tbb12_debug.dll"
            "${NATIVES_OUT_DIR}/tbb12_debug.dll"
        COMMENT "Auto-deploying OpenVINO and oneTBB DLL dependencies to ${NATIVES_OUT_DIR}..."
    )
endif()

# =========================================================================
# CROSS-COMPILATION & AUTOMATION TARGETS
# =========================================================================

# Target to build Linux x86_64 via WSL
add_custom_target(build_linux_x64
    COMMAND C:/Windows/System32/wsl.exe bash -c "mkdir -p /mnt/c/Users/aziml/my_projects/MathventageSIMD/out/linux-x64 && g++ -std=c++20 -O3 -mavx2 -mfma -DMATHVANTAGE_EXPORTS -fPIC -shared -I/mnt/c/Users/aziml/my_projects/MathventageSIMD/include -I/mnt/c/Users/aziml/my_projects/MathventageSIMD/src /mnt/c/Users/aziml/my_projects/MathventageSIMD/build/MathventageSIMD_Derived.cpp /mnt/c/Users/aziml/my_projects/MathventageSIMD/src/SacFor_VM.cpp -o /mnt/c/Users/aziml/my_projects/MathventageSIMD/out/linux-x64/libsfmsimd.so"
    DEPENDS generate_mathvantage_sources
    COMMENT "Building Linux x86_64 via WSL -> out/linux-x64/libsfmsimd.so..."
)

# Target to build Linux ARM64 via WSL
add_custom_target(build_linux_arm64
    COMMAND C:/Windows/System32/wsl.exe bash -c "mkdir -p /mnt/c/Users/aziml/my_projects/MathventageSIMD/out/linux-arm64 && aarch64-linux-gnu-g++ -std=c++20 -O3 -march=armv8-a+simd -DMATHVANTAGE_EXPORTS -fPIC -shared -I/mnt/c/Users/aziml/my_projects/MathventageSIMD/include -I/mnt/c/Users/aziml/my_projects/MathventageSIMD/src /mnt/c/Users/aziml/my_projects/MathventageSIMD/build/MathventageSIMD_Derived.cpp /mnt/c/Users/aziml/my_projects/MathventageSIMD/src/SacFor_VM.cpp -o /mnt/c/Users/aziml/my_projects/MathventageSIMD/out/linux-arm64/libsfmsimd.so"
    DEPENDS generate_mathvantage_sources
    COMMENT "Building Linux ARM64 via WSL -> out/linux-arm64/libsfmsimd.so..."
)

# Target to build ALL cross-platform binaries
add_custom_target(build_all_cross
    DEPENDS mathvantage build_linux_x64 build_linux_arm64
    COMMENT "All Cross-Platform binaries (Windows x64 + Linux x64 + Linux ARM64) built successfully!"
)

# SDK Packaging Target: Packages include/ headers and out/ binaries into a clean SDK zip
add_custom_target(package_sdk
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_OUT_DIR}/sdk/include"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_OUT_DIR}/sdk/bin"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_OUT_DIR}/sdk/bindings/java"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/include/SacculusForge.h" "${SDK_OUT_DIR}/sdk/include/SacculusForge.h"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/include/MathventageSIMD.h" "${SDK_OUT_DIR}/sdk/include/MathventageSIMD.h"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SDK_OUT_DIR}/windows-x64" "${SDK_OUT_DIR}/sdk/bin/windows-x64"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SDK_OUT_DIR}/linux-x64" "${SDK_OUT_DIR}/sdk/bin/linux-x64"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SDK_OUT_DIR}/linux-arm64" "${SDK_OUT_DIR}/sdk/bin/linux-arm64"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/other_langs_bindings/Java/JNI/out/MSIMD_JNI_Loader.java" "${SDK_OUT_DIR}/sdk/bindings/java/MSIMD_JNI_Loader.java"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/other_langs_bindings/Java/JNI/out/SacculusForge.java" "${SDK_OUT_DIR}/sdk/bindings/java/SacculusForge.java"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/other_langs_bindings/Java/JNI/out/MathventageSIMD.java" "${SDK_OUT_DIR}/sdk/bindings/java/MathventageSIMD.java"
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${SDK_OUT_DIR}/MathventageSIMD-SDK.zip" --format=zip "${SDK_OUT_DIR}/sdk"
    DEPENDS build_all_cross sfmsimd_jni
    COMMENT "Packaging MathventageSIMD-SDK.zip into ${SDK_OUT_DIR}..."
)

# Jextract Custom Target (Regenerates Java Panama Bindings)
add_custom_target(generate_java_bindings
    COMMAND "C:/Program Files/Java/jextract-25/bin/jextract.bat"
        --output "${CMAKE_SOURCE_DIR}/../../src/main/java"
        -t net.chiretallyn.mathvantage
        --header-class-name MathventageSIMD_API
        "${CMAKE_SOURCE_DIR}/include/MathventageSIMD.h"
    COMMENT "Regenerating Java Panama FFM Bindings via jextract-25..."
)

# Modular Java JNI Subsystem
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/other_langs_bindings/Java/JNI/MSIMD-JNIGen.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/other_langs_bindings/Java/JNI/MSIMD-JNIGen.cmake")
endif()

# =========================================================================
# BARE-METAL EMBEDDED FIRMWARE TARGET (INTEL HEX STUB)
# Activated via CLI option: cmake -B build -DBUILD_EMBEDDED_HEX=ON (Default: OFF)
# =========================================================================
option(BUILD_EMBEDDED_HEX "Build embedded bare-metal firmware stub producing Intel HEX image" OFF)

if(BUILD_EMBEDDED_HEX)
    message(STATUS "Configuring Embedded Bare-Metal Firmware Stub (.hex output enabled)...")

    # Locate llvm-objcopy in default LLVM dir or system PATH
    find_program(LLVM_OBJCOPY_BIN
        NAMES llvm-objcopy llvm-objcopy.exe
        PATHS "${DEFAULT_LLVM_DIR}/bin" "C:/Program Files/LLVM/bin"
    )

    add_executable(sf_firmware_stub
        src/embedded/firmware_main.cpp
    )
    target_include_directories(sf_firmware_stub PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(sf_firmware_stub PRIVATE mathvantage)

    if(LLVM_OBJCOPY_BIN)
        add_custom_command(TARGET sf_firmware_stub POST_BUILD
            COMMAND "${LLVM_OBJCOPY_BIN}" -O ihex $<TARGET_FILE:sf_firmware_stub> "${CMAKE_BINARY_DIR}/sf_firmware_stub.hex"
            COMMENT "Converting firmware binary to Intel HEX image: ${CMAKE_BINARY_DIR}/sf_firmware_stub.hex"
            VERBATIM
        )
        message(STATUS "Intel HEX post-build conversion configured via: ${LLVM_OBJCOPY_BIN}")
    else()
        message(WARNING "llvm-objcopy not found! Skipping post-build .hex conversion.")
    endif()
endif()

# =========================================================================
# SFVM ARCHITECTURAL VERIFICATION SUITE
# =========================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_sfvm_all_features.cpp")
    add_executable(sf_verification_test
        tests/verify_sfvm_all_features.cpp
    )
    target_include_directories(sf_verification_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    set_target_properties(sf_verification_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${NATIVES_OUT_DIR}"
    )
    if(MSVC)
        set_property(TARGET sf_verification_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
        target_compile_definitions(sf_verification_test PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    endif()
    target_link_libraries(sf_verification_test PRIVATE mathvantage)
    enable_testing()
    add_test(NAME sf_verification_test COMMAND sf_verification_test)
endif()

message(STATUS "Configured for ${MSIMD_OS}-${MSIMD_ARCH}. Output -> ${NATIVES_OUT_DIR}")


