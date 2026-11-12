#include "main.h"

int check_command_type(char *command){
    char *built_ins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
    

	if(strcmp(command,"\0") == 0) return NO_COMMAND;
	for(int i=0; built_ins[i]!=NULL;i++){
		if(strcmp(command,built_ins[i]) == 0) return BUILTIN;
	}
	char *external_commands[155] = {NULL};
	extract_external_commands(external_commands);
	for(int i=0;external_commands[i]!= NULL;i++){
		if(strcmp(command,external_commands[i]) == 0) return EXTERNAL;
	}
}