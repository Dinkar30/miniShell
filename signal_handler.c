#include"main.h"
extern volatile sig_atomic_t child_event;
extern pid_t pid;
void signal_handler(int sig_num){
   if(sig_num == SIGINT || sig_num == SIGTSTP){
      write(STDOUT_FILENO,"\n",1);
    }
    if(sig_num == SIGCHLD) child_event = 1;

    
    
}