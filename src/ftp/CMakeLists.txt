add_library(ftp
        STATIC
            client.cpp
            client.hpp
            control_connection.cpp
            control_connection.hpp
            data_connection.cpp
            data_connection.hpp
            data_transfer_mode.cpp
            data_transfer_mode.hpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(ftp
        PRIVATE
            ${Boost_LIBRARIES})

target_include_directories(ftp
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${Boost_INCLUDE_DIRS})