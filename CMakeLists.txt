cmake_minimum_required(VERSION 3.10)
project(ftp_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(ftp_client src/main.cpp src/ftp_client.cpp src/ftp_client.hpp src/user_interface.cpp src/user_interface.hpp src/request_handler.cpp src/request_handler.hpp src/request_parser.cpp src/request_parser.hpp src/requests.hpp src/session.cpp src/session.hpp)
    target_link_libraries(ftp_client ${Boost_LIBRARIES})
endif()