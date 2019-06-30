add_executable(ftp_client
        commands_handler.cpp
        commands_handler.hpp
        parser.cpp
        parser.hpp
        local_exception.hpp
        main.cpp
        commands.hpp
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