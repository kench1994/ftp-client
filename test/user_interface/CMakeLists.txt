add_executable(user_interface_tests
        parser_tests.cpp
        ../../src/parser.cpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(user_interface_tests
        PRIVATE
            ${Boost_LIBRARIES}
            gtest_main)

target_include_directories(user_interface_tests
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ../../src)