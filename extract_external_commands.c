#include"main.h"

void extract_external_commands(char **external_commands){
    //variable declaration 
    int fd,i,j;
    char ch;
    char buffer[25] = {'\0'};
    // open the file in read mode 
    fd = open("external_commands.txt",O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    // read the command one by one 
    i=0,j=0;
    while(read(fd,&ch,1) > 0){
        if(ch != '\n') buffer[i++] = ch;
        else {
            // allocate some sufficient memory first
            external_commands[j] = calloc(strlen(buffer)+1,sizeof(char));
            // copy the contents: strcpy
            strcpy(external_commands[j++],buffer);
            external_commands[j] = '\0';
            // clear the buffer
            memset(buffer,'\0',25);
            i = 0; //re-initialize 
        }

    }
}