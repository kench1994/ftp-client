add_library(utils
        STATIC
            utils.cpp
            utils.hpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(utils
        PRIVATE
            ${Boost_LIBRARIES})

target_include_directories(utils
        PRIVATE
            ${Boost_INCLUDE_DIRS})