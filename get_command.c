#include"main.h"

// Extract a command until a space is encounterred
char *get_command(char *input_string){
    // declare an array to store the command 
    static char command[25] = "\0";
    int i=0;
    while(1){
        if(*input_string == ' ' || *input_string == '\0') break;
        command[i++] = *input_string;
        input_string++;
    }
    // append NULL at the end 
    command[i] = '\0';
    return command;
}
