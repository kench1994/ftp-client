cmake_minimum_required(VERSION 3.10)
project(ftp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

add_library(${PROJECT_NAME}
        STATIC
            client.cpp
            client.hpp
            control_connection.cpp
            control_connection.hpp
            data_connection.cpp
            data_connection.hpp
            data_transfer_mode.cpp
            data_transfer_mode.hpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${Boost_INCLUDE_DIRS})