cmake_minimum_required(VERSION 3.20)
project(kaleidoscope)

include(FetchContent)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs core orcjit native)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.1.0)
FetchContent_MakeAvailable(fmt)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)")
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -march=native
            $<$<CONFIG:DEBUG>:-fsanitize=address>
            $<$<CONFIG:DEBUG>:-g3>
            $<$<CONFIG:DEBUG>:-Og>
            $<$<CONFIG:RELEASE>:-g0>
    )
    add_link_options(
            -rdynamic
            $<$<CONFIG:DEBUG>:-fsanitize=address>
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(
            /W4
    )
else ()
    message(FATAL_ERROR "Unknown compiler '${CMAKE_CXX_COMPILER_ID}'")
endif ()

set(KALEIDOSCOPE_SOURCES
        lib/Lexer/Lexer.cpp
        lib/AST/NumberExprAST.cpp
        lib/AST/VariableExprAST.cpp
        lib/AST/BinaryExprAST.cpp
        lib/AST/CallExprAST.cpp
        lib/AST/IfExprAST.cpp
        lib/AST/ForExprAST.cpp
        lib/AST/PrototypeAST.cpp
        lib/AST/FunctionAST.cpp
        lib/AST/Dump/XMLDump.cpp
        lib/Parser/Parser.cpp
        lib/CodeGen/CodeGen.cpp
        lib/Driver/Driver.cpp
        lib/JIT/KaleidoscopeJIT.cpp)
set(KALEIDOSCOPE_HEADERS
        include/kaleidoscope/Lexer/Lexer.h
        include/kaleidoscope/AST/AST.h
        include/kaleidoscope/AST/ASTVisitor.h
        include/kaleidoscope/AST/ExprAST.h
        include/kaleidoscope/AST/NumberExprAST.h
        include/kaleidoscope/AST/VariableExprAST.h
        include/kaleidoscope/AST/BinaryExprAST.h
        include/kaleidoscope/AST/CallExprAST.h
        include/kaleidoscope/AST/IfExprAST.h
        include/kaleidoscope/AST/ForExprAST.h
        include/kaleidoscope/AST/PrototypeAST.h
        include/kaleidoscope/AST/FunctionAST.h
        include/kaleidoscope/AST/Dump/XMLDump.h
        include/kaleidoscope/Parser/Parser.h
        include/kaleidoscope/CodeGen/CodeGen.h
        include/kaleidoscope/Util/Error/Log.h
        include/kaleidoscope/Driver/Driver.h
        include/kaleidoscope/JIT/KaleidoscopeJIT.h)

include_directories(include)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_library(
        kaleidoscope_library
        ${KALEIDOSCOPE_SOURCES}
        ${KALEIDOSCOPE_HEADERS}
)
target_link_libraries(
        kaleidoscope_library
        fmt::fmt
        ${llvm_libs}
)

add_executable(
        kaleidoscope
        main.cpp
        ${KALEIDOSCOPE_HEADERS}
)
target_link_libraries(
        kaleidoscope
        kaleidoscope_library
)
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set_property(TARGET kaleidoscope PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

enable_testing()
add_subdirectory(unittests)