add_executable (server server.c ../includes/utility.c ../includes/utility.h) 
target_link_libraries (server pthread)