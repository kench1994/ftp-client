add_executable(user_interface_tests
        parser_tests.cpp
        ../../src/parser.cpp)

target_link_libraries(user_interface_tests
        PRIVATE
            gtest_main)

target_include_directories(user_interface_tests
        PRIVATE
            ../../src)