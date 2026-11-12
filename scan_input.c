#include"main.h"

pid_t pid;
void scan_input(char* prompt,char *input_string){
   char *command;
    int command_type,status = 0;
    int ret_status;

    // register the SIGINT signal
    signal(SIGINT,signal_handler);

   while(1){

    //display the prompt
     printf(ANSI_COLOR_RED "[%s]$" ANSI_COLOR_RESET,prompt);
     // clear the output buffer
     fflush(stdout);
     // scan the input till end of line
     scanf("%[^\n]s",input_string);
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
      // create a child process
      pid = fork();
      if(pid == -1){
         perror("fork");
         exit(1);
      }
      else if(pid == 0){
         //child 
         signal(SIGINT,SIG_DFL);
         ret_status = system(input_string);
         if(ret_status == 0) exit(0);
         else exit(1);

      }
      else {
         // parent 
         wait(&status);
         if(WIFEXITED(status)) printf("child with pid: %d terminated normally\n",pid); 
         else if(WIFSIGNALED(status)) printf("child with pid %d terminated by a signal no. %d\n",pid,WTERMSIG(status));
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
    
   
   