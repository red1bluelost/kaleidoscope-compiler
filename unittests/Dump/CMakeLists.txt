add_executable(
        unittests_dump
        XMLDump.cpp
        ../TestUtil.h
)
target_link_libraries(
        unittests_dump
        gtest_main
        kaleidoscope_library
)

gtest_discover_tests(unittests_dump)