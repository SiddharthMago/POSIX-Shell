#include "functions.h"

bool redirection_flag = false;

char* redirection_function(char* command) {
    redirection_flag = false;
    bool append_flag = false;

    char* file_name = (char*)malloc(MAX_LENGTH * sizeof(char));

    if(strstr(command, ">>") != NULL) append_flag = true;

    redirection_flag = true;

    char* command_copy = (char*)malloc(MAX_LENGTH * sizeof(char));
    strcpy(command_copy, command);

    char* command2 = strtok(command_copy, ">");
    char* command2_copy = (char*)malloc(MAX_LENGTH * sizeof(char));
    strcpy(command2_copy, command2);

    command2 = strtok(NULL, ">");
    strcpy(file_name, command2);
    file_name = trim(file_name);

    if(file_name == NULL) {
        printf("\nERROR: No file name given\n\n");
        redirection_flag = false;
        free(file_name);
        return command;
    }

    command2_copy = trim(command2_copy);
    command = command2_copy;

    int fd = open(file_name, O_WRONLY | O_CREAT, 0644);

    if(append_flag == true) freopen(file_name, "a+", stdout);
    else freopen(file_name, "w", stdout);

    return command;
}

void handle_pipe(char* input, char** alias_file_text, char** log_file_text) {
    printnl();
    redirection_flag = true;

    input = trim(input);
    if(input[0] == '|' || input[strlen(input) - 1] == '|') {
        printf("ERROR: Incomplete pipe sequence.\n");
        redirection_flag = false;
        printnl();
        return;
    }

    char* pipe_saveptr;
    char* pipe_command = strtok_r(input, "|", &pipe_saveptr);

    char** pipe_commands = (char**)malloc(MAX_PIPE_COMMANDS * sizeof(char*));
    for(int i=0; i<MAX_PIPE_COMMANDS; i++) pipe_commands[i] = (char*)malloc(MAX_LENGTH * sizeof(char));

    int pipe_count = 0;

    while(pipe_command != NULL) {
        strcpy(pipe_commands[pipe_count++], pipe_command);
        pipe_command = strtok_r(NULL, "|", &pipe_saveptr);
    }

    int input_fd = STDIN_FILENO;
    int pipe_fd[2];

    for(int i=0; i<pipe_count; i++) {
        if(i < pipe_count - 1) {
            if(pipe(pipe_fd) == -1) {
                printf("ERROR: pipe failed\n");
                exit(1);
            }
        }

        pid_t pid = fork();
        if(pid == 0) {

            if(i > 0) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            if(i < pipe_count - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            command_handler(pipe_commands[i], alias_file_text, log_file_text);
            exit(0);

        }

        else if(pid > 0) {
            if(i > 0) close(input_fd);
            if(i < pipe_count - 1) {
                input_fd = pipe_fd[0];
                close(pipe_fd[1]);
            }
            waitpid(pid, NULL, 0);
        }

        else {
            printf("ERROR: piping fork failed\n");
            exit(1);
        }
    }

    for(int i=0; i<pipe_count; i++) free(pipe_commands[i]);
    free(pipe_commands);

    redirection_flag = false;
    printnl();
}