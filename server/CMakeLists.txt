project (Chat_App)
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "../bin"
) 
add_executable(server server.c ../includes/utility.c ../includes/utility.h) 