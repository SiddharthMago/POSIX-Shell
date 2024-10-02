#include "functions.h"

void make_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void make_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);  // Remove O_NONBLOCK flag
}

int get_latest_PID() {
    struct dirent *entry;
    DIR *proc_dir = opendir("/proc");

    if(proc_dir == NULL) return -1;

    int latest_pid = -1;

    while((entry = readdir(proc_dir)) != NULL) {
        // Check if the directory name is numeric (i.e., a PID)
        if(isdigit(entry->d_name[0])) {
            pid_t pid = atoi(entry->d_name);
            if(pid > latest_pid) latest_pid = pid;  // Keep track of the largest PID = latest PID
        }
    }

    closedir(proc_dir);

    return latest_pid;
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void neonate_function(char* command) {
    printnl();
    char* token = strtok(command, " \t");
    token = strtok(NULL, " \t");
    if(token == NULL) {
        printf("ERROR: missing command argument\n");
        printnl();
        return;
    }
    token = strtok(NULL, " \t");
    if(token == NULL) {
        printf("ERROR: missing flag argument\n");
        printnl();
        return;
    }
    int interval = atoi(token);
    if(interval == 0) {
        printf("ERROR: invalid interval argument\n");
        printnl();
        return;
    }
    
    make_non_blocking(STDIN_FILENO);

    while(1) {
        if(kbhit()) {
            char ch = getchar();
            if(ch == 'x') break;
        }

        int pid = get_latest_PID();
        if(pid > 0) printf("latest pid : %i\n", pid);
        else printf("ERROR: Could not get latest PID\n");
        sleep(interval);
    }

    make_blocking(STDIN_FILENO);
    printnl();
}