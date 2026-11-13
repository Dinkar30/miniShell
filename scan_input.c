#include"main.h"

job_t *Jobs = NULL;
volatile sig_atomic_t child_event = 0;
int status = 0;
pid_t pid;
void scan_input(char* prompt,char *input_string){
   char *command;
   char *argv[25];
   char *token;
   int argc = 0;
   int command_type;
   int next_job_id = 1;
    // register the required signals
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGTSTP,&sa,NULL) == -1){
      perror("sigaction");
      exit(EXIT_FAILURE);
    }
    if(sigaction(SIGCHLD,&sa,NULL) == -1){
      perror("sigaction");
      exit(EXIT_FAILURE);
    }

   while(1){

      if(child_event){
         child_event = 0;
         pid_t finished_pid;
         while((finished_pid = waitpid(-1,&status,WNOHANG)) > 0){
            job_t *current = Jobs;
            job_t *previous = NULL;

            while(current != NULL && current->pid != finished_pid){
               previous = current;
               current = current->next;
            }
            if(current == NULL) continue;
            if(WIFEXITED(status)){
               printf("[%d] %s exited with status %d\n",
                           current->job_id,
                           current->command,
                           WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
               printf("[%d] %s terminated by signal %d\n",
                           current->job_id,
                           current->command,
                           WTERMSIG(status));
            }

            if(previous == NULL) Jobs = current->next;
            else previous->next=current->next;
            free(current);
            // printf("background process terminated\n");
         }

      }

    //display the prompt
     printf(ANSI_COLOR_RED "[%s]$" ANSI_COLOR_RESET,prompt);
     // clear the output buffer
     fflush(stdout);
     // scan the input till end of line
     if(scanf("%[^\n]s",input_string) == EOF){
      clearerr(stdin);
      memset(input_string,'\0',25);
      continue;
     };
     // clear the input buffer
     getchar();

     // customize the prompt 
     if(!strncmp("PS1=",input_string,4)){
        if(input_string[4] != ' '){
            strcpy(prompt,&input_string[4]);
            // flush the input string 
            memset(input_string,'\0',25);
            continue;
        }
     }
    command = get_command(input_string);
    // check the command: if it's internal or external
    command_type = check_command_type(command);
    //printf("The command type is %d\n",command_type);

    
    if(command_type == EXTERNAL){

      signal(SIGTSTP, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);
      signal(SIGTTIN, SIG_IGN);

      // inspect incase '&' is used for background process !
      int background = 0;
      if(input_string[strlen(input_string)-1] == '&') background = 1;

      // create a child process
      pid = fork();
      if(pid == -1){
         perror("fork");
         exit(1);
      }
      else if(pid == 0){
         //child 
         if(setpgid(0,0) == -1) perror("setpgid");
         signal(SIGINT,SIG_DFL);
         signal(SIGTSTP,SIG_DFL);
         signal(SIGTTOU, SIG_DFL);
         signal(SIGTTIN, SIG_DFL);
         argc = 0;
         token = strtok(input_string," \t");
         while(token != NULL && argc < 24){
            argv[argc++] = token;
            token = strtok(NULL , " \t");
         }
         argv[argc] = NULL;
         if(argc > 0 && strcmp(argv[argc-1],"&") == 0){
            argv[--argc] = NULL;
         }
            execvp(argv[0],argv);
            perror("execvp");
            _exit(EXIT_FAILURE);
         

      }
      else {
         // parent 
         
         if(setpgid(pid,pid) == -1) perror("setpgid");
         if(background){
            // background process initiated
            job_t *new_job = malloc(sizeof(*new_job));
            new_job->job_id = next_job_id++;
            new_job->state = RUNNING;
            new_job->pid = pid;
            new_job->pgid = pid;

            strncpy(new_job->command, input_string,sizeof(new_job->command)-1);

            new_job->command[sizeof(new_job->command)-1] = '\0';
            new_job->next = Jobs;
            Jobs = new_job;
            printf("started background process %d\n",pid);
         }
         else {
            if(tcsetpgrp(STDIN_FILENO,pid) == -1) perror("tcsetpgrp");
            signal(SIGTSTP,SIG_IGN);
            if(waitpid(pid,&status,WUNTRACED) == -1) perror("waitpid");
            else if(WIFSTOPPED(status)){
               job_t *new_job = malloc(sizeof(*new_job));
               new_job->job_id = next_job_id++;
               new_job->state = STOPPED;
               new_job->pid = pid;
               new_job->pgid = pid;

               strncpy(new_job->command, input_string,sizeof(new_job->command)-1);

               new_job->command[sizeof(new_job->command)-1] = '\0';
               new_job->next = Jobs;
               Jobs = new_job;
               printf("\nchild with pid %d stopped by signal %d\n",pid,WSTOPSIG(status));
               }
            else if(WIFEXITED(status)) printf("child with pid: %d terminated normally\n",pid); 
            else if(WIFSIGNALED(status)) printf("child with pid %d terminated by signal no. %d\n",pid,WTERMSIG(status));
            if(tcsetpgrp(STDIN_FILENO,getpgrp()) == -1) perror("tcsetpgrp");
         }

         if(sigaction(SIGTSTP,&sa,NULL) == -1) perror("sigaction");
         pid = 0;
         
      }
      // clear the input string for next iteration 
      memset(input_string,'\0',25);
    }
   // call the functions to implement echo command
   echo(input_string,status);
   // internal commands
   execute_internal_commands(input_string);
   // not a valid command 
   if(command_type != NO_COMMAND && command_type != BUILTIN && command_type != EXTERNAL) printf("%s: command not found\n",input_string);
    // clear the input string for next iteration 
      memset(input_string,'\0',25);
   }
}