add_library(ncurses_wrappers
        STATIC
            ncurses.cpp
            ncurses.hpp
            ncurses_exception.hpp
            ncurses_initializer.hpp)

find_package(Curses REQUIRED)
find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(ncurses_wrappers
        PRIVATE
            ${CURSES_LIBRARIES}
            ${Boost_LIBRARIES})

target_include_directories(ncurses_wrappers
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${CURSES_INCLUDE_DIR}
            ${Boost_INCLUDE_DIRS})