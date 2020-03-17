add_executable(ftp_cmdline
        command.hpp
        commands_handler.cpp
        commands_handler.hpp
        local_exception.hpp
        main.cpp
        parser.cpp
        parser.hpp
        user_interface.cpp
        user_interface.hpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

target_link_libraries(ftp_cmdline
        PRIVATE
            ftp
            utils
            ${Boost_LIBRARIES})

target_include_directories(ftp_cmdline
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ..)