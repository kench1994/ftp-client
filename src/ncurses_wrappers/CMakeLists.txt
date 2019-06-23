cmake_minimum_required(VERSION 3.10)
project(ncurses_wrappers)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)

add_library(${PROJECT_NAME} STATIC src/ncurses.cpp)

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include/ncurses_wrappers)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)