#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void eprint(const char* fmt, ...);
void wprint(const char* fmt, ...);
void sprint(const char* fmt, ...);
char *trim(char *str);