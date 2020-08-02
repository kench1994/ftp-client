add_executable(ftp_tests
        client_tests.cpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system filesystem)

target_link_libraries(ftp_tests
        PRIVATE
            ftp
            ${Boost_LIBRARIES}
            gtest_main)

target_include_directories(ftp_tests
        PRIVATE
            ${Boost_INCLUDE_DIRS})