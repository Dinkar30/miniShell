#include"main.h"

extern pid_t pid;
void signal_handler(int sig_num){
   if(sig_num == SIGINT || sig_num == SIGTSTP){
      write(STDOUT_FILENO,"\n",1);
    }

    
    
}