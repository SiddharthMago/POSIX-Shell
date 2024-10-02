#include "functions.h"

void proclore_function(char *pid, bool background) {
    printnl();
    
    char* path = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* line = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* exec_path = (char*)malloc(MAX_LENGTH * sizeof(char));
    char state;
    int ppid;
    int pgid;
    int vsize;

    pid_t selfpid = -1;
    if (pid == NULL) {
        selfpid = getpid();
        snprintf(path, MAX_LENGTH, "/proc/%i/stat", selfpid);
    }
    else snprintf(path, MAX_LENGTH, "/proc/%s/stat", pid);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("ERROR: Error opening process stat file\n");
        printnl();
        return;
    }

    if (fgets(line, MAX_LENGTH, fp) != NULL) {
        char *token = strtok(line, " ");
        int field_index = 1;

        while (token != NULL) {
            if(field_index == 3) state = token[0]; // Field 3: Process state
            else if(field_index == 5) ppid = atoi(token); // Field 5: Parent process ID (PPID)
            else if(field_index == 8) pgid = atoi(token); // Field 6: Process group ID (PGID)    
            else if(field_index == 23) vsize = atoi(token); // Field 23: Virtual memory size (VSIZE)

            token = strtok(NULL, " ");
            field_index++;
        }
    }
    fclose(fp);

    char *status;
    if(state == 'R') status = "Running";
    else if(state == 'S') status = "Sleeping in an interruptible wait";
    else if(state == 'Z') status = "Zombie";
    else if(state == 'D') status = "Uninterruptible sleep";
    else if(state == 'T') status = "Stopped";
    else status = "Unknown";

    if(selfpid == -1) printf("Pid: %s\n", pid);
    else printf("Pid: %i\n", selfpid);
    printf("Process status: %c%c\n", state, background ? ' ' : '+');
    printf("Process group: %i\n", pgid);
    printf("Virtual memory: %i kB\n", vsize);

    snprintf(path, MAX_LENGTH, "/proc/%s/exe", pid);
    ssize_t len = readlink(path, exec_path, sizeof(exec_path) - 1);
    if(len != -1) {
        exec_path[len] = '\0';
        printf("Executable path: %s\n", exec_path);
    }
    else {
        if(selfpid != -1) printf("Executable path: ./a.out\n");
        else printf("ERROR: Unable to read executable path\n");
    }
    
    free(path);
    free(line);
    free(exec_path);
    printnl();
}