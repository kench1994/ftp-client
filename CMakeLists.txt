cmake_minimum_required(VERSION 3.10)
project(ftp_client)

set(CMAKE_CXX_STANDARD 17)

add_executable(ftp_client src/main.cpp src/ftp_client.cpp src/ftp_client.hpp)