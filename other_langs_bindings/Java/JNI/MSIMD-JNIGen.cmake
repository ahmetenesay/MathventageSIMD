# =========================================================================
# MathventageSIMD — MSIMD-JNIGen.cmake
# Modular CMake Generator & Build Module for Java JNI Bindings
# =========================================================================

cmake_minimum_required(VERSION 3.20)

project(MathventageSIMD_JNI LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------------
# 1. JNI Header Discovery (Auto Find or Fallback to JDK 25)
# -------------------------------------------------------------------------
find_package(JNI QUIET)

if(NOT JNI_FOUND)
    set(DEFAULT_JDK_PATHS
        "C:/Program Files/Java/jdk-25"
        "C:/Program Files/Java/jdk-24"
        "C:/Program Files/Java/jdk-21"
        "$ENV{JAVA_HOME}"
    )
    foreach(JDK_PATH ${DEFAULT_JDK_PATHS})
        if(EXISTS "${JDK_PATH}/include/jni.h")
            set(JNI_INCLUDE_DIRS
                "${JDK_PATH}/include"
                "${JDK_PATH}/include/win32"
                "${JDK_PATH}/include/linux"
                "${JDK_PATH}/include/darwin"
            )
            set(JNI_FOUND TRUE)
            message(STATUS "[MSIMD-JNI] Located JDK JNI headers at: ${JDK_PATH}/include")
            break()
        endif()
    endforeach()
else()
    message(STATUS "[MSIMD-JNI] System JNI headers found: ${JNI_INCLUDE_DIRS}")
endif()

if(NOT JNI_FOUND)
    message(FATAL_ERROR "[MSIMD-JNI] ERROR: Could not locate JNI headers (jni.h)! Please set JAVA_HOME or install JDK.")
endif()

# -------------------------------------------------------------------------
# 2. Directory Topography & Target Paths
# -------------------------------------------------------------------------
set(JNI_ROOT_DIR   "${CMAKE_CURRENT_LIST_DIR}")
set(JNI_SRC_DIR    "${JNI_ROOT_DIR}/src")
set(JNI_OTOGEN_DIR "${JNI_ROOT_DIR}/otogen")
set(JNI_OUT_DIR    "${JNI_ROOT_DIR}/out")

file(MAKE_DIRECTORY "${JNI_OUT_DIR}")

# -------------------------------------------------------------------------
# 3. Automation Target: Run abg.ps1 to generate Java classes & JNI C++ wrapper
# -------------------------------------------------------------------------
find_program(POWERSHELL_BIN NAMES pwsh powershell.exe powershell)

set(JNI_GENERATED_WRAPPER_CPP "${JNI_SRC_DIR}/msimd_jni_bridge.cpp")
set(JAVA_OUT_LOADER          "${JNI_OUT_DIR}/MSIMD_JNI_Loader.java")
set(JAVA_OUT_MATHVENTAGE     "${JNI_OUT_DIR}/MathventageSIMD.java")
set(JAVA_OUT_SACCULUSFORGE   "${JNI_OUT_DIR}/SacculusForge.java")

if(POWERSHELL_BIN)
    add_custom_command(
        OUTPUT
            "${JNI_GENERATED_WRAPPER_CPP}"
            "${JAVA_OUT_LOADER}"
            "${JAVA_OUT_MATHVENTAGE}"
            "${JAVA_OUT_SACCULUSFORGE}"
        COMMAND "${POWERSHELL_BIN}" -NoProfile -ExecutionPolicy Bypass -File "${JNI_OTOGEN_DIR}/abg.ps1"
        DEPENDS
            "${JNI_OTOGEN_DIR}/abg.ps1"
            "${JNI_SRC_DIR}/SacculusForge.h"
            "${JNI_SRC_DIR}/MathventageSIMD.h"
        WORKING_DIRECTORY "${JNI_ROOT_DIR}"
        COMMENT "[MSIMD-JNI] Executing abg.ps1: Synthesizing 3 Java classes & msimd_jni_bridge.cpp..."
    )
    add_custom_target(generate_jni_bindings
        DEPENDS
            "${JNI_GENERATED_WRAPPER_CPP}"
            "${JAVA_OUT_LOADER}"
            "${JAVA_OUT_MATHVENTAGE}"
            "${JAVA_OUT_SACCULUSFORGE}"
    )
endif()

# -------------------------------------------------------------------------
# 4. Build JNI Bridge Shared Library (libsfmsimd_jni.dll / .so)
# -------------------------------------------------------------------------
add_library(sfmsimd_jni SHARED
    "${JNI_GENERATED_WRAPPER_CPP}"
)

target_include_directories(sfmsimd_jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${JNI_SRC_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
)

target_compile_definitions(sfmsimd_jni PRIVATE
    MATHVANTAGE_EXPORTS
    MSIMD_JNI_BUILD
)

if(TARGET mathvantage)
    target_link_libraries(sfmsimd_jni PRIVATE mathvantage)
else()
    # Standalone mode: link directly against core libsfmsimd
    target_link_libraries(sfmsimd_jni PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/${MSIMD_OS}-${MSIMD_ARCH}/libsfmsimd")
endif()

set_target_properties(sfmsimd_jni PROPERTIES
    OUTPUT_NAME "sfmsimd_jni"
    LIBRARY_OUTPUT_DIRECTORY "${JNI_OUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${JNI_OUT_DIR}"
)

add_custom_command(TARGET sfmsimd_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:sfmsimd_jni>"
        "${PROJECT_SOURCE_DIR}/out/${MSIMD_OS}-${MSIMD_ARCH}/$<TARGET_FILE_NAME:sfmsimd_jni>"
    COMMENT "[MSIMD-JNI] Deploying sfmsimd_jni to out/${MSIMD_OS}-${MSIMD_ARCH}..."
)

message(STATUS "[MSIMD-JNI] Configured JNI Binding target: sfmsimd_jni -> ${JNI_OUT_DIR}")
