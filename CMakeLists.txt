cmake_minimum_required(VERSION 3.28)
project(injector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --static")

add_executable(injector main.cpp process.cpp)

set_target_properties(injector PROPERTIES
        OUTPUT_NAME "injector"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out"
        PREFIX "")
