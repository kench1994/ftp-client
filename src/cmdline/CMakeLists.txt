add_executable(ftp_cmdline
        cmdline_exception.hpp
        cmdline_interface.cpp
        cmdline_interface.hpp
        command.hpp
        command_handler.cpp
        command_handler.hpp
        command_parser.cpp
        command_parser.hpp
        main.cpp)

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