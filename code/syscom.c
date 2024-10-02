#include "functions.h"

process_struct latest_fg_proc;

bool process_flag = false;

void syscom_function(char* command, bool background, bool* print_CLI_flag, char** command_temp, long* elapsed_time_temp) {
    printnl();
    pid_t pid = fork();
    if(pid == 0) {
        time_t start_time, end_time;
        time(&start_time);

        execlp("sh", "sh", "-c", command, NULL);

        printf("\033[0;31mERROR: exec failed - %s\n\033[0m", strerror(errno));
        exit(1);
    }
    else if(pid > 0) {
        if(background == false) {
            int status;

            latest_fg_proc.pid = pid;
            latest_fg_proc.state = 'R';
            strcpy(latest_fg_proc.command, command);

            time_t start_time, end_time;
            time(&start_time);
            waitpid(pid, &status, WUNTRACED);
            time(&end_time);

            double time_temp = difftime(end_time, start_time);
            long elapsed_time = (long)floor(time_temp);

            if(elapsed_time > 2) {
                *print_CLI_flag = true;
                *command_temp = strdup(command);
                *elapsed_time_temp = elapsed_time;
            }
        }
        else {
            add_process(pid, command);
            *print_CLI_flag = false;
            printf("PID: %i\n", pid);
        }
    }
    else {
        printf("\033[0;31mERROR: fork failed - %s\n\033[0m", strerror(errno));
    }
    printnl();
}