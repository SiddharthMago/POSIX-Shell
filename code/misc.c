#include "functions.h"

extern bool redirection_flag;

void printCLI(int checkdir, char* username, char* hostname, char* homedir, char* curdir) {
    if(checkdir == 1) {
        // printf("<%s@%s:~>", username, hostname);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:~>", hostname);
    }
    else if(checkdir == 2){
        char mydir[MAX_LENGTH];
        for(int i = strlen(homedir); i <= strlen(curdir); i++) {
            mydir[i-strlen(homedir)] = curdir[i];
        }
        // printf("<%s@%s:~/%s>", username, hostname, mydir);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:~%s>", hostname, mydir);
    }
    else {
        // printf("<%s@%s:%s>", username, hostname, curdir);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:%s>", hostname, curdir);
    }
}

int printCLI_with_comtime(int checkdir, char* username, char* hostname, char* homedir, char* curdir, char* command_temp, long elapsed_time_temp) {
    command_temp = trim(command_temp);
    char* token = strtok(command_temp, " \t");
    if(token == NULL) return -1;
    if(checkdir == 1) {
        // printf("<%s@%s:~>", username, hostname);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:~ %s : %lis >", hostname, token, elapsed_time_temp);
    }
    else if(checkdir == 2){
        char mydir[MAX_LENGTH];
        for(int i = strlen(homedir); i <= strlen(curdir); i++) {
            mydir[i-strlen(homedir)] = curdir[i];
        }
        // printf("<%s@%s:~/%s>", username, hostname, mydir);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:~%s %s : %lis >", hostname, mydir, token, elapsed_time_temp);
    }
    else {
        // printf("<%s@%s:%s>", username, hostname, curdir);
        printf("<");
        printf("\033[0;34m");
        printf("%s", username);
        printf("\033[0m");
        printf("@%s:%s %s : %lis >", hostname, curdir, token, elapsed_time_temp);
    }
    return 0;
}

char* ftrim(char* str) {
    if(!str || !*str) return str;
    while(isspace((unsigned char)*str)) str++;
    return str;
}

char* trim(char* str) {
    if(!str || !*str) return str;
    while(isspace((unsigned char)*str)) str++;
    char *ptr;
    for(ptr = str + strlen(str) - 1; (ptr >= str) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return str;
}

void printnl() {
    if(redirection_flag == false) printf("\n");
}

void printe(char* error_message) {
    if(redirection_flag == false) {
        printf("\033[0;31m%s\033[0m", error_message);
    }
}