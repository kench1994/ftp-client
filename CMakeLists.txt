cmake_minimum_required(VERSION 3.10)
project(ftp_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(ftp_client
        src/command_handler.cpp
        src/command_handler.hpp
        src/command_parser.cpp
        src/command_parser.hpp
        src/local_exception.hpp
        src/main.cpp
        src/resources.cpp
        src/resources.hpp
        src/tools.cpp
        src/tools.hpp
        src/user_interface.cpp
        src/user_interface.hpp
        # ftp
        src/ftp/client.cpp
        src/ftp/client.hpp
        src/ftp/control_connection.cpp
        src/ftp/control_connection.hpp
        src/ftp/data_connection.cpp
        src/ftp/data_connection.hpp
        src/ftp/data_transfer_mode.cpp
        src/ftp/data_transfer_mode.hpp
        src/ftp/ftp_exception.hpp
        src/ftp/negative_completion_code.hpp
        # ncurses
        src/ncurses_wrappers/ncurses.cpp
        src/ncurses_wrappers/ncurses.hpp
        src/ncurses_wrappers/ncurses_exception.hpp
        src/ncurses_wrappers/ncurses_initializer.hpp)

target_link_libraries(ftp_client ${Boost_LIBRARIES})
target_link_libraries(ftp_client ${CURSES_LIBRARIES})