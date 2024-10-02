#include "functions.h"

process_struct processes[MAX_PROCESSES];
int process_count = 0;

extern process_struct latest_fg_proc;

void setup_signal_handling() {
    struct sigaction sa, sa_int, sa_tstp;
    
    sa.sa_handler = bg_process_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    sa_int.sa_handler = ctrlc;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    sa_tstp.sa_handler = ctrlz;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = 0;
    sigaction(SIGTSTP, &sa_tstp, NULL);

    return;
}

void bg_process_signal(int sig) {
    printnl();
    printnl();
    int saved_errno = errno;
    pid_t pid;
    int status;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if(WIFEXITED(status)) {
            printf("Process %i exited normally with status %i\n", pid, WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)) {
            printf("Process %i exited due to signal %i\n", pid, WTERMSIG(status));
        }
    }

    errno = saved_errno;
}

void ctrlc(int sig) {
    printnl();
    if(latest_fg_proc.pid > 0) {
        if(kill(latest_fg_proc.pid, SIGINT) == 0) {
            printf("Sent SIGINT to process %d\n", latest_fg_proc.pid);
            latest_fg_proc.pid = -1;
        }
        else {
            printf("\033[0;31mERROR: Could not send signal to foreground process %i\n\n\033[0m", latest_fg_proc.pid);
        }
    }
    else {
        printe("ERROR: No foreground process to interrupt.\n");
    }
    printnl();
}

void kill_all_processes() {
    // printnl();
    // for(int i=0; i<process_count; i++) {
    //     if(kill(processes[i].pid, SIGKILL) == 0) {
    //         printf("Killed process with PID %d\n", processes[i].pid);
    //     }
    //     else {
    //         printe("ERROR: Failed to kill process with PID %d\n", processes[i].pid);
    //     }
    // }
    // printnl();
    for(int i=0; i<process_count; i++) {
        kill(processes[i].pid, SIGKILL);
    }
    printf("Killed all processes\n");
}

void ctrlz(int sig) {
    printnl();
    if(latest_fg_proc.pid != -1) {

        bool present_in_processes = false;
        for(int i=0; i<process_count; i++) if(processes[i].pid == latest_fg_proc.pid) present_in_processes = true;
        if(!present_in_processes) add_process(latest_fg_proc.pid, latest_fg_proc.command);

        if(kill(latest_fg_proc.pid, SIGSTOP) == 0) {
            printf("Sent SIGSTP to process %d\n", latest_fg_proc.pid);
            latest_fg_proc.pid = -1;
            // tcsetpgrp(STDIN_FILENO, getpid());
        }
        else {
            printf("\033[0;31mERROR: Could not send signal to process %i\n\033[0m", latest_fg_proc.pid);
        }
    }
    else {
        printe("ERROR: No foreground process to interrupt.\n");
    }
    printnl();
}

int compare_processes(const void* a, const void* b) {
    process_struct* process1 = (process_struct*)a;
    process_struct* process2 = (process_struct*)b;
    return process1->pid - process2->pid;
}

char process_state(pid_t pid) {
    char* path = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* line = (char*)malloc(MAX_LENGTH * sizeof(char));
    char state = 'U';

    snprintf(path, MAX_LENGTH, "/proc/%i/stat", pid);

    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        return state;
    }

    if(fgets(line, MAX_LENGTH, fp) != NULL) {
        char *token = strtok(line, " ");
        int field_index = 1;

        while (token != NULL) {
            if(field_index == 3) {
                state = token[0];
                break;
            }

            token = strtok(NULL, " ");
            field_index++;
        }
    }
    fclose(fp);

    free(path);
    free(line);
    return state;
}

void add_process(pid_t pid, const char* command) {
    if(process_count >= MAX_PROCESSES) {
        printe("\nERROR: Maximum number of processes reached\n\n");
        return;
    }
    processes[process_count].pid = pid;
    strcpy(processes[process_count].command, command);
    processes[process_count].state = process_state(pid);
    process_count++;
}

void update_process_states() {
    for(int i=0; i<process_count; i++) {
        processes[i].state = process_state(processes[i].pid);
    }
}

void activities() {
    printnl();
    update_process_states();
    
    qsort(processes, process_count, sizeof(process_struct), compare_processes);

    for(int i=0; i<process_count; i++) {
        if(processes[i].state != 'D' && processes[i].state != 'U') {
            char* state;
            if(processes[i].state == 'R' || processes[i].state == 'S' || processes[i].state == 'Z') state = "Running";
            else state = "Stopped";

            printf("%i : %s - %s\n", processes[i].pid, processes[i].command, state);
        }
    }
    printnl();
}

void ping_function(char* command) {
    printnl();
    setup_signal_handling();
    char* token = strtok(command, " ");
    token = strtok(NULL, " ");
    if(token == NULL) {
        printe("ERROR: PID not given\n");
        printnl();
        return;
    }
    pid_t pid = atoi(token);

    token = strtok(NULL, " ");
    if(token == NULL) {
        printe("ERROR: Ping type not given\n");
        printnl();
        return;
    }

    int ping_type = atoi(token);
    ping_type = ping_type % MAX_SIGNALS;

    if(ping_type <= 0) {
        printe("ERROR: Invalid ping type\n\n");
        printnl();
        return;
    }

    // printf("%i %i\n", pid, ping_type);

    if(kill(pid, ping_type) == -1) {
        if(errno == ESRCH) printe("ERROR: No such process found.\n");
        else if(errno == EINVAL) printe("ERROR: Invalid signal number.\n");
        else printf("\033[0;31mERROR: Could not send signal to process %i: %s\n\033[0m", pid, strerror(errno));
    }
    else printf("Sent signal %i to process with pid %i\n", ping_type, pid);
    printnl();
}