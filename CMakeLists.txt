cmake_minimum_required(VERSION 3.22)
project(kaleidoscope)

include(FetchContent)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.1.0)
FetchContent_MakeAvailable(fmt)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs core)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:DEBUG>:-fsanitize=address>
        $<$<CONFIG:DEBUG>:-g3>
        $<$<CONFIG:DEBUG>:-Og>
        $<$<CONFIG:RELEASE>:-g0>
        $<$<CONFIG:RELEASE>:-O3>
)

add_link_options(
        $<$<CONFIG:DEBUG>:-fsanitize=address>
)

include_directories(include)

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
        include/AST/PrototypeAST.h
        include/AST/FunctionAST.h
        lib/Parser/Parser.cpp
        include/Parser/Parser.h
        lib/CodeGen/CodeGen.cpp
        include/CodeGen/CodeGen.h
        include/Error/Log.h
)
target_link_libraries(kaleidoscope
        fmt::fmt
        ${llvm_libs}
)
