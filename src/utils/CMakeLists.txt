add_library(utils
        STATIC
            utils.cpp
            utils.hpp)

find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

target_link_libraries(utils
        PUBLIC
            ${Boost_LIBRARIES})

target_include_directories(utils
        PUBLIC
            ${Boost_INCLUDE_DIRS})