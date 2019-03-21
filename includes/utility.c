#include "utility.h"

void eprint(const char* fmt, ...){
    va_list args;
    va_start(args,fmt);
    char err_msg[100] = {0};
    sprintf(err_msg,"\x1b[1;37;41mError: %s \x1b[0m\n",fmt);
    vprintf(err_msg,args);
    va_end(args);
}

void wprint(const char* fmt, ...){
    va_list args;
    va_start(args,fmt);
    char w_msg[100]={0};
    sprintf(w_msg,"\x1b[1;37;43mWarning: %s \x1b[0m\n",fmt);
    vprintf(w_msg,args);
    va_end(args);
}

void sprint(const char* fmt, ...){
    va_list args;
    va_start(args,fmt);
    char s_msg[100]={0};
    sprintf(s_msg,"\x1b[1;37;42mSuccess: %s \x1b[0m\n",fmt);
    vprintf(s_msg,args);
    va_end(args);
}


char *trim(char *str){
  char *end;

  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  end[1] = '\0';

  return str;
}
