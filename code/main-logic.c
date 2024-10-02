#include "functions.h"

extern bool redirection_flag;
extern bool alias_function_flag;
extern bool print_CLI_flag;
extern bool log_execute_flag;
extern bool neonate_flag;

extern char* username;
extern char* hostname;
extern char* homedir;
extern char* curdir;
extern char* prevdir;
extern char* log_file_path;
extern char* alias_file_path;
extern char* command_temp;

extern long elapsed_time_temp;
extern int log_line_count;
extern int alias_line_count;

extern process_struct latest_fg_proc;

void command_handler(char* input, char** alias_file_text, char** log_file_text) {
    setup_signal_handling();

    char* input_copy = strdup(input);
    char* saveptr;
    char* command = strtok_r(input_copy, ";&", &saveptr);

    while(command != NULL) {
        command = ftrim(command);

        char delimiter = input[command - input_copy + strlen(command)];
        bool background = false;

        if(delimiter == '&') {
            background = true;
            print_CLI_flag = true;
        }

        command = search_alias_file_text(command, alias_file_text, alias_line_count);
        if(alias_function_flag == true) {
            alias_function_flag = false;
            log_execute_flag = true;
            strcpy(input, command);
            break;
        }

        if(strstr(command, ">") != NULL) command = redirection_function(command);

        if(strncmp(command, "exit", 4) == 0) {
            free(username);
            free(hostname);
            free(homedir);
            free(curdir);
            free(prevdir);
            free(log_file_path);
            free(alias_file_path);
            free(input);
            free(command);
            for(int i=0; i<log_line_count; i++) free(log_file_text[i]);
            for(int i=0; i<alias_line_count; i++) free(alias_file_text[i]);
            free(log_file_text);
            exit(1);
        }

        else if(strncmp(command, "hop", 3) == 0 && strncmp(command, "hop_seek", 8) != 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    hop_function(command, homedir, curdir, prevdir);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                hop_function(command, homedir, curdir, prevdir);
            }
        }
        
        else if(strncmp(command, "reveal", 6) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    reveal_function(command, homedir);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                reveal_function(command, homedir);
            }
        }

        else if(strncmp(command, "log", 3) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    command = log_function(command, log_file_path, log_file_text, log_line_count, &log_execute_flag);
                    if(log_execute_flag == true) {
                        strcpy(input, command);
                        break;
                    }
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                command = log_function(command, log_file_path, log_file_text, log_line_count, &log_execute_flag);
                if(command == NULL) {
                    log_execute_flag = false;
                    break;
                }
                if(log_execute_flag == true) {
                    strcpy(input, command);
                    break;
                }
            }
        }

        else if(strncmp(command, "proclore", 8) == 0) {
            char* token = strtok(command, " \t");
            token = strtok(NULL, " \t");
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    proclore_function(token, background);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                proclore_function(token, background);
            }
        }

        else if(strncmp(command, "seek", 4) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    seek_function(command);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                seek_function(command);
            }
        }

        else if(strncmp(command, "alias", 5) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    alias_function(command, alias_file_path);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                alias_function(command, alias_file_path);
            }
        }

        else if(strcmp(command, "print_alias") == 0) {
            print_aliases(alias_file_text, alias_line_count);
        }

        else if(strncmp(command, "activities", 10) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    activities();
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                activities();
            }
        }

        else if(strncmp(command, "ping", 4) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    ping_function(command);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                ping_function(command);
            }
        }

        else if(strncmp(command, "fg", 2) == 0) {
            setup_signal_handling();
            char* token = strtok(command, " \t");
            token = strtok(NULL, " \t");
            if(token == NULL) {
                printe("ERROR: missing argument\n");
            }
            else {
                pid_t pid = atoi(token);
                if(pid == 0) printe("ERROR: invalid argument\n");
                else {
                    char* temp = (char*)malloc(MAX_LENGTH * sizeof(char));
                    snprintf(temp, MAX_LENGTH, "ping %i 18", pid);
                    ping_function(temp);
                    latest_fg_proc.pid = pid;
                    int status;
                    waitpid(pid, &status, WUNTRACED);
                    free(temp);
                }
            }
        }

        else if(strncmp(command, "bg", 2) == 0) {
            setup_signal_handling();
            char* token = strtok(command, " \t");
            token = strtok(NULL, " \t");
            if(token == NULL) {
                printe("ERROR: missing argument\n");
            }
            else {
                int pid = atoi(token);
                if(pid == 0) {
                    printe("ERROR: invalid argument\n");
                }
                else {
                    char* temp = (char*)malloc(MAX_LENGTH * sizeof(char));
                    snprintf(temp, MAX_LENGTH, "ping %i 18", pid);
                    ping_function(temp);
                    free(temp);
                }
            }
        }

        else if(strncmp(command, "neonate", 7) == 0) {
            neonate_function(command);
            // neonate_flag = true;
        }

        else if(strncmp(command, "iMan", 4) == 0) {
            if(background == true) {
                pid_t pid = fork();
                if(pid == 0) {
                    iMan_function(command);
                    exit(0);
                }
                else {
                    printf("PID: %i\n", pid);
                    wait(NULL);
                }
            }
            else {
                iMan_function(command);
            }
        }

        else {
            syscom_function(command, background, &print_CLI_flag, &command_temp, &elapsed_time_temp);
        }
        
        if(redirection_flag == true) {
            freopen("/dev/tty", "w", stdout);
            redirection_flag = false;
        }

        command = strtok_r(NULL, ";&", &saveptr);
    }
}