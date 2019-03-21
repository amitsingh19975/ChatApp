project (Chat_App)
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "../bin"
) 
add_executable(client client.c ../includes/utility.c) 