cmake_minimum_required (VERSION 2.6)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project (ChatApp C)
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "../bin"
)

set ( ChatApp_MAJOR_VERSION 1 )
set ( ChatApp_MINOR_VERSION 0 )

add_subdirectory(client)
add_subdirectory(server)