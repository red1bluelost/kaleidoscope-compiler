FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        unittests
        Lexer.cpp
        Parser.cpp
        TestUtil.h
)
target_link_libraries(
        unittests
        gtest_main
        kaleidoscope_library
)

include(GoogleTest)
gtest_discover_tests(unittests)

add_subdirectory(Dump)