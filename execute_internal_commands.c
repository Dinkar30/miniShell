#include"main.h"

extern job_t *Jobs;
extern int status;
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
    if(strncmp("fg",input_string,2) == 0){
        strtok(input_string," \t");
        char *pid_text = strtok(NULL," \t");

        job_t *current = Jobs;
        if(current == NULL){ printf("No current job\n"); return; }
        job_t *previous = NULL;
        if(pid_text != NULL){
            pid_t requested_pid = atoi(pid_text);
            while(current != NULL && current->pid != requested_pid){
                previous = current;
                current = current->next;
            }
        }
        
        if(current == NULL) {printf("fg: no such job\n");return;}
        else {
            printf("process %d is foregrounded\n",current->pid);
            if(current->state == STOPPED){
                if(kill(-current->pgid,SIGCONT) == -1){ perror("kill");return;}
                else current->state = RUNNING;
            }

            if(tcsetpgrp(STDIN_FILENO,current->pgid) == -1) {perror("tcsetpgrp"); return;}
            if(waitpid(current->pid,&status,WUNTRACED) == -1) perror("waitpid");
            else if(WIFEXITED(status) || WIFSIGNALED(status)){
                if(previous == NULL) Jobs = current->next;
                else previous->next = current->next;
                if(WIFEXITED(status)) printf("child with pid: %d terminated normally\n",current->pid);
                if(WIFSIGNALED(status)) printf("child with pid %d terminated by signal no. %d\n",current->pid,WTERMSIG(status));
                free(current);
            } else if(WIFSTOPPED(status)) {
                current->state = STOPPED;
                printf("\nchild with pid %d stopped by signal %d\n",current->pid,WSTOPSIG(status));
            }

            if(tcsetpgrp(STDIN_FILENO,getpgrp()) == -1) perror("tcsetpgrp");

        }
    }

}