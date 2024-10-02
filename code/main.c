#include "functions.h"

extern bool redirection_flag;
extern bool alias_function_flag;

bool log_execute_flag = false;
bool print_CLI_flag = false;
bool neonate_flag = false;

char* username;
char* hostname;
char* homedir;
char* curdir;
char* prevdir;
char* log_file_path;
char* alias_file_path;
char* command_temp;

long elapsed_time_temp;
int log_line_count;
int alias_line_count;
pid_t shell_pid;

int main() {
    setup_signal_handling();

    shell_pid = getpid();

    username = getlogin();

    hostname = (char*)malloc(MAX_LENGTH * sizeof(char));
    gethostname(hostname, MAX_LENGTH);

    homedir = (char*)malloc(MAX_LENGTH * sizeof(char));
    getcwd(homedir, MAX_LENGTH);

    curdir = (char*)malloc(MAX_LENGTH * sizeof(char));
    getcwd(curdir, MAX_LENGTH);

    prevdir = (char*)malloc(MAX_LENGTH * sizeof(char));
    getcwd(prevdir, MAX_LENGTH);

    char* log_file_name = "log.txt";
    log_file_path = (char*)malloc(MAX_LENGTH * sizeof(char));
    snprintf(log_file_path, MAX_LENGTH, "%s/%s", homedir, log_file_name);
    FILE* log_file = fopen(log_file_path, "a+");  
    fclose(log_file);

    char* alias_file_name = ".myshrc";
    alias_file_path = (char*)malloc(MAX_LENGTH * sizeof(char));
    snprintf(alias_file_path, MAX_LENGTH, "%s/%s", homedir, alias_file_name);
    FILE* alias_file = fopen(alias_file_path, "a+");  
    fclose(alias_file);
    
    char* input = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* nullinput = "nullinput";
    elapsed_time_temp = -1;
    command_temp = NULL;

    while(1) {
        getcwd(curdir, MAX_LENGTH);
        int checkdir = ishomedir(curdir, homedir);

        if(log_execute_flag == false) {
            if(print_CLI_flag == false) printCLI(checkdir, username, hostname, homedir, curdir);
            else {
                int retval = printCLI_with_comtime(checkdir, username, hostname, homedir, curdir, command_temp, elapsed_time_temp);
                print_CLI_flag = false;
                if(retval == -1) continue;
            }
        }

        log_line_count = 0;
        char** log_file_text = log_to_array_helper(log_file_path, &log_line_count);

        alias_line_count = 0;
        char** alias_file_text = alias_to_array_helper(alias_file_path, &alias_line_count);

        if(log_file_text == NULL || alias_file_text == NULL) {
            printe("ERROR: log_file_text and/or alias_file_text is NULL\n");
            break;
        }

        int scanf_retval = -1;
        if(log_execute_flag == false) {
            scanf_retval = scanf("%[^\n]", input);
            getchar();
        }

        if(log_execute_flag == false && scanf_retval == EOF) {
            kill_all_processes();
            exit(1);
        }

        if(log_execute_flag == false && strcmp(input, nullinput) == 0) continue;

        if(strstr(input, "log") == NULL) {
            log_file_helper(input, log_file_text, log_line_count, log_file_path);
        }

        log_execute_flag = false;

        if(strchr(input, '|')) handle_pipe(input, alias_file_text, log_file_text);
        else command_handler(input, alias_file_text, log_file_text);

        if(log_execute_flag == false) strcpy(input, nullinput);
    }
}