add_executable(cmdline_tests
        parser_tests.cpp
        ../../src/cmdline/command_parser.cpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(cmdline_tests
        PRIVATE
            ${Boost_LIBRARIES}
            gtest_main)

target_include_directories(cmdline_tests
        PRIVATE
            ${Boost_INCLUDE_DIRS})