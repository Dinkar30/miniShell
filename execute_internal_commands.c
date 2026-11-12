#include"main.h"

void execute_internal_commands(char *input_string){
    //exit pwd and cd 
    char *path;
    if(strncmp("exit",input_string,4) == 0){
        exit(1);
    }
    // pwd 
    if(strncmp("pwd",input_string,3) == 0){
        system("pwd");
    }
    if(strncmp("cd",input_string,2) == 0){
       // chdir(&input_string[3]); // works only for single space
       path = strtok(input_string, " ");
       char *backUp;
       while(path != NULL){
        backUp = path;
        path = strtok(NULL," ");
       }
       chdir(backUp);
    }

}