add_executable(ftp_tests
        ftp_client_tests.cpp)

target_link_libraries(ftp_tests
        PRIVATE
            gtest_main)