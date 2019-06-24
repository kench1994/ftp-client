cmake_minimum_required(VERSION 3.10)
project(ncurses_wrappers)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)

add_library(${PROJECT_NAME}
        STATIC
            ncurses.cpp
            ncurses.hpp
            ncurses_exception.hpp
            ncurses_initializer.hpp)

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${CURSES_INCLUDE_DIR})