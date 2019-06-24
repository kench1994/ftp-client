add_executable(ftp_client
        command_handler.cpp
        command_handler.hpp
        command_parser.cpp
        command_parser.hpp
        local_exception.hpp
        main.cpp
        resources.cpp
        resources.hpp
        tools.cpp
        tools.hpp
        user_interface.cpp
        user_interface.hpp)

add_subdirectory(ftp)
add_subdirectory(ncurses_wrappers)

find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

target_link_libraries(ftp_client
        PRIVATE
            ftp
            ncurses_wrappers
            ${Boost_LIBRARIES})

target_include_directories(ftp_client
        PRIVATE
            ${Boost_INCLUDE_DIRS})