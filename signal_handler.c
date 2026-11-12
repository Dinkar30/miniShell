#include"main.h"

extern pid_t pid;
void signal_handler(int sig_num){
   if(sig_num == SIGINT){
    if(pid) kill(pid,SIGINT);
    else write(STDOUT_FILENO,"\n",1);
        }
    
    
}