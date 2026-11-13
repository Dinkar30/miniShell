#include"main.h"

extern job_t *Jobs;
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

    if(strncmp("jobs",input_string,4) == 0){
        job_t *current = Jobs;
        while(current != NULL){
            printf("[%d] %s\t%d\n",
                current->job_id,
                current->state == RUNNING ? "Running":"Stopped",
                current->pid);
            current = current->next;
        }
    }

    if(strncmp("bg",input_string,2) == 0){
        job_t *current = Jobs;
        while(current != NULL && current->state != STOPPED) current = current->next;
        if(current != NULL){
            if(kill(-current->pgid,SIGCONT) == -1) perror("kill");
            else current->state = RUNNING;
        } 
         
    }

}