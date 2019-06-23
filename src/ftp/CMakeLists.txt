cmake_minimum_required(VERSION 3.10)
project(ftp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

add_library(${PROJECT_NAME} STATIC
        src/client.cpp
        src/control_connection.cpp
        src/data_connection.cpp
        src/data_transfer_mode.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include/ftp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})