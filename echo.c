#include "main.h"

void echo(char *input_string, int status)
{
    // printf("INSIDE ECHO: [%s]\n", input_string);
    fflush(stdout);

    if(strncmp("echo $?", input_string,7) == 0)
        printf("%d\n", status);

    else if(strncmp("echo $$", input_string,7) == 0)
        printf("%d\n", getpid());

    else if(strncmp("echo $SHELL", input_string,11) == 0)
    {
        printf("MATCH FOUND!\n");
        fflush(stdout);

        system("pwd");

        printf("SYSTEM CALL FINISHED!\n");
    }
}