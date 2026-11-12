#include"main.h"

extern pid_t pid;
void signal_handler(int sig_num){
   if(sig_num == SIGINT){
     if(pid == 0) write(STDOUT_FILENO,"\n",1);
    }

    
    
}