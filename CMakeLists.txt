cmake_minimum_required(VERSION 3.10)
project(ftp_client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src/ftp)
add_subdirectory(src/ncurses_wrappers)

add_executable(${PROJECT_NAME}
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
        src/user_interface.hpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ftp)
target_link_libraries(${PROJECT_NAME} ncurses_wrappers)