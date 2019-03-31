include_directories(../includes)
add_executable (server server.c ../includes/utility.c) 
target_link_libraries (server pthread)