cmake_minimum_required(VERSION 3.10)
project(ftp_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system filesystem)
find_package(Curses REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${CURSES_INCLUDE_DIR})
    add_executable(ftp_client src/main.cpp src/user_interface.cpp src/user_interface.hpp src/client.cpp src/client.hpp src/resources.hpp src/control_connection.cpp src/control_connection.hpp src/resources.cpp src/local_exception.hpp src/tools.cpp src/tools.hpp src/data_connection.cpp src/data_connection.hpp src/negative_completion_code.hpp src/command_handler.cpp src/command_handler.hpp src/data_transfer_mode.cpp src/data_transfer_mode.hpp src/base_exception.hpp src/command_parser.cpp src/command_parser.hpp src/ftp_exception.hpp src/ncurses_wrappers/ncurses.hpp src/ncurses_wrappers/ncurses_exception.hpp src/ncurses_wrappers/ncurses.cpp src/ncurses_wrappers/ncurses_initializer.hpp)
    target_link_libraries(ftp_client ${Boost_LIBRARIES})
    target_link_libraries(ftp_client ${CURSES_LIBRARIES})
endif()