add_executable(client client.c ../includes/utility.c) 
target_link_libraries (client pthread)