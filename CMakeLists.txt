cmake_minimum_required(VERSION 3.20)

project(JOCKY
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==================================================
# Third-party dependencies
# ==================================================

# --------------------------------------------------
# ZLIB
# --------------------------------------------------

set(ZLIB_DIR
    "C:/vcpkg/installed/x64-windows/share/zlib"
)

find_package(ZLIB REQUIRED CONFIG)

message(STATUS "Found ZLIB")
message(STATUS "ZLIB directory: ${ZLIB_DIR}")

# --------------------------------------------------
# zstd
# --------------------------------------------------

set(zstd_DIR
    "C:/vcpkg/installed/x64-windows-static/share/zstd"
)

find_package(zstd CONFIG REQUIRED)

message(STATUS "Found zstd")
message(STATUS "zstd directory: ${zstd_DIR}")

# ==================================================
# LLVM
# ==================================================

set(LLVM_ROOT
    "C:/Program Files/LLVM/clang+llvm-23.1.2-x86_64-pc-windows-msvc/"
)

set(LLVM_DIR
    "${LLVM_ROOT}/lib/cmake/llvm"
)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM directory: ${LLVM_DIR}")

include_directories(
    ${LLVM_INCLUDE_DIRS}
)

separate_arguments(
    LLVM_DEFINITIONS_LIST
    NATIVE_COMMAND
    ${LLVM_DEFINITIONS}
)

add_definitions(
    ${LLVM_DEFINITIONS_LIST}
)

# ==================================================
# JOCKY Compiler
# ==================================================

add_executable(jocky

    compiler/driver/main.cpp

    compiler/lexer/lexer.cpp

    compiler/parser/parser.cpp

    compiler/semantic/semantic.cpp

    compiler/jir/jir.cpp

    compiler/llvm/llvm_codegen.cpp

    runtime/jocky_runtime.cpp
)

# ==================================================
# JOCKY Include Directories
# ==================================================

target_include_directories(jocky PRIVATE

    compiler/lexer

    compiler/parser

    compiler/semantic

    compiler/jir

    compiler/llvm

    runtime
)

# ==================================================
# LLVM Libraries
# ==================================================

llvm_map_components_to_libnames(
    LLVM_LIBS
    core
    support
)

target_link_libraries(
    jocky PRIVATE
    ${LLVM_LIBS}
)

# ==================================================
# Windows dependencies
# ==================================================

if(WIN32)

    target_link_libraries(
        jocky PRIVATE
        ntdll
        iphlpapi
    )

endif()