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
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU|Clang")
    # G++
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            $<$<CONFIG:DEBUG>:-fsanitize=address>
            $<$<CONFIG:DEBUG>:-g3>
            $<$<CONFIG:DEBUG>:-Og>
            $<$<CONFIG:RELEASE>:-g0>
    )
    add_link_options(
            -export-dynamic
            $<$<CONFIG:DEBUG>:-fsanitize=address>
    )
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    # MSVC
    add_compile_options(
            /W3
    )
endif()



include_directories(include)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_executable(kaleidoscope
        main.cpp
        lib/Lexer/Lexer.cpp
        include/Lexer/Lexer.h
        include/AST/ExprAST.h
        lib/AST/NumberExprAST.cpp
        include/AST/NumberExprAST.h
        lib/AST/VariableExprAST.cpp
        include/AST/VariableExprAST.h
        lib/AST/BinaryExprAST.cpp
        include/AST/BinaryExprAST.h
        lib/AST/CallExprAST.cpp
        include/AST/CallExprAST.h
        lib/AST/IfExprAST.cpp
        include/AST/IfExprAST.h
        lib/AST/ForExprAST.cpp
        include/AST/ForExprAST.h
        lib/AST/PrototypeAST.cpp
        include/AST/PrototypeAST.h
        lib/AST/FunctionAST.cpp
        include/AST/FunctionAST.h
        lib/Parser/Parser.cpp
        include/Parser/Parser.h
        lib/CodeGen/CodeGen.cpp
        include/CodeGen/CodeGen.h
        include/Error/Log.h
        lib/Driver/Driver.cpp
        include/Driver/Driver.h
        lib/JIT/KaleidoscopeJIT.cpp
        include/JIT/KaleidoscopeJIT.h
)
target_link_libraries(kaleidoscope
        fmt::fmt
        ${llvm_libs}
)
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set_property(TARGET kaleidoscope PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()