cmake_minimum_required(VERSION 3.20)
project(SFVM_Native_Benchmark_Harness LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MATHVANTAGE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(JINGZERO_NATIVES "C:/Users/aziml/my_projects/JingZero/src/main/resources/natives/mathvantage/windows-x64")

find_library(SFMSIMD_LIB
    NAMES sfmsimd libsfmsimd
    PATHS
        "${MATHVANTAGE_ROOT}/build"
        "${MATHVANTAGE_ROOT}/build/Release"
        "${MATHVANTAGE_ROOT}/build/RelWithDebInfo"
        "${JINGZERO_NATIVES}"
    NO_DEFAULT_PATH
)

if(NOT SFMSIMD_LIB)
    set(SFMSIMD_LIB "${MATHVANTAGE_ROOT}/build/sfmsimd.lib")
endif()

include_directories(
    "${MATHVANTAGE_ROOT}/include"
    "${MATHVANTAGE_ROOT}/src"
)

# Benchmark Main Executable
add_executable(sf_bench_main bench_main.cpp)
target_link_libraries(sf_bench_main PRIVATE "${SFMSIMD_LIB}")

# Copy runtime DLLs next to binaries
if(WIN32)
    add_custom_command(TARGET sf_bench_main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${JINGZERO_NATIVES}/libsfmsimd.dll"
        $<TARGET_FILE_DIR:sf_bench_main>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${JINGZERO_NATIVES}/tbb12.dll"
        $<TARGET_FILE_DIR:sf_bench_main>
    )
endif()
