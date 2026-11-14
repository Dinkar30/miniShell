#include "main.h"

int execute_pipeline(char *input_string)
{
    char *stages[25];
    char *stage_save;
    char *stage;
    int stage_count = 0;
    pid_t children[25];
    pid_t pipeline_pgid = 0;
    int previous_read = -1;

    stage = strtok_r(input_string, "|", &stage_save);
    while (stage != NULL && stage_count < 25) {
        while (*stage == ' ' || *stage == '\t')
            stage++;
        stages[stage_count++] = stage;
        stage = strtok_r(NULL, "|", &stage_save);
    }

    if (stage_count < 2) {
        fprintf(stderr, "Invalid pipe\n");
        return -1;
    }

    for (int i = 0; i < stage_count; i++) {
        int pipe_fd[2] = {-1, -1};

        if (i < stage_count - 1 && pipe(pipe_fd) == -1) {
            perror("pipe");
            return -1;
        }

        children[i] = fork();
        if (children[i] == -1) {
            perror("fork");
            return -1;
        }

        if (children[i] == 0) {
            char *argv[25];
            char *token;
            char *token_save;
            int argc = 0;

            if (pipeline_pgid == 0)
                pipeline_pgid = getpid();
            if (setpgid(0, pipeline_pgid) == -1)
                perror("setpgid");

            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);

            if (previous_read != -1)
                dup2(previous_read, STDIN_FILENO);
            if (pipe_fd[1] != -1)
                dup2(pipe_fd[1], STDOUT_FILENO);

            if (previous_read != -1)
                close(previous_read);
            if (pipe_fd[0] != -1)
                close(pipe_fd[0]);
            if (pipe_fd[1] != -1)
                close(pipe_fd[1]);

            token = strtok_r(stages[i], " \t", &token_save);
            while (token != NULL && argc < 24) {
                argv[argc++] = token;
                token = strtok_r(NULL, " \t", &token_save);
            }
            argv[argc] = NULL;

            if (argc == 0)
                _exit(EXIT_FAILURE);

            execvp(argv[0], argv);
            perror("execvp");
            _exit(127);
        }

        if (pipeline_pgid == 0)
            pipeline_pgid = children[i];
        if (setpgid(children[i], pipeline_pgid) == -1)
            perror("setpgid");

        if (previous_read != -1)
            close(previous_read);
        if (pipe_fd[1] != -1)
            close(pipe_fd[1]);
        previous_read = pipe_fd[0];
    }

    if (previous_read != -1)
        close(previous_read);

    if (tcsetpgrp(STDIN_FILENO, pipeline_pgid) == -1)
        perror("tcsetpgrp");

    for (int i = 0; i < stage_count; i++) {
        int status;
        if (waitpid(children[i], &status, WUNTRACED) == -1)
            perror("waitpid");
    }

    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
        perror("tcsetpgrp");

    return 0;
}
