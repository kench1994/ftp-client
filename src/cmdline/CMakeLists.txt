add_executable(ftp_cmdline
        cmdline_interface.cpp
        cmdline_interface.hpp
        command.hpp
        command_executor.cpp
        command_executor.hpp
        command_parser.cpp
        command_parser.hpp
        local_exception.hpp
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