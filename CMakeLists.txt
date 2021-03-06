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

# Let's set compiler-specific flags
if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)")
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -march=native
            $<$<CONFIG:DEBUG>:-fsanitize=address>
            $<$<CONFIG:DEBUG>:-g3>
            # $<$<CONFIG:DEBUG>:-Og>
            $<$<CONFIG:RELEASE>:-g0>
    )
    add_link_options(
            -rdynamic
            $<$<CONFIG:DEBUG>:-fsanitize=address>
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # MSVC
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
        lib/Parser/Parser.cpp
        lib/CodeGen/CodeGen.cpp
        lib/Driver/Driver.cpp
        lib/JIT/KaleidoscopeJIT.cpp)
set(KALEIDOSCOPE_HEADERS
        include/Lexer/Lexer.h
        include/AST/AST.h
        include/AST/ASTVisitor.h
        include/AST/ExprAST.h
        include/AST/NumberExprAST.h
        include/AST/VariableExprAST.h
        include/AST/BinaryExprAST.h
        include/AST/CallExprAST.h
        include/AST/IfExprAST.h
        include/AST/ForExprAST.h
        include/AST/PrototypeAST.h
        include/AST/FunctionAST.h
        include/Parser/Parser.h
        include/CodeGen/CodeGen.h
        include/Util/Error/Log.h
        include/Driver/Driver.h
        include/JIT/KaleidoscopeJIT.h)

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

add_subdirectory(unittests)