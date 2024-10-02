#include "functions.h"

int ishomedir(char* curdir, char* homedir) {
    if(strlen(curdir) < strlen(homedir)) {
        return -1;
    }
    for(int i = 0; i < strlen(homedir); i++) {
        if(curdir[i] != homedir[i]) {
            return -1;
        }
    }
    
    if(strlen(curdir) == strlen(homedir)) {
        return 1;
    }
    else return 2;
}

void hop_function(char* command, char* homedir, char* curdir, char* prevdir) {
    printnl();
    char* token = strtok(command, " \t");
    token = strtok(NULL, " \t");

    char tempdir[MAX_LENGTH];

    if(token == NULL) {
        strcpy(prevdir, curdir);
        chdir(homedir);
        printf("%s\n", getcwd(tempdir, sizeof(tempdir)));
        strcpy(curdir, tempdir);
        printnl();
        return;
    }
    while(token != NULL) {
        if(strcmp(token, "~") == 0) {
            strcpy(prevdir, curdir);
            chdir(homedir);
        }
        else if(strcmp(token, "-") == 0) {
            char temp[MAX_LENGTH];
            strcpy(temp, curdir);
            chdir(prevdir);
            strcpy(prevdir, temp);
        }
        else {
            strcpy(prevdir, curdir);
            int i=0;
            if(token[0] == '~' && token[1] == '/') { // handles the case where the user enters ~ && / in the start new hop dir
                while(token[i] == '~' || token[i] == '/') i++;
                char temp[MAX_LENGTH];
                int j = 0;
                while(j < strlen(token)) {
                    temp[j] = token[i + j];
                    j++;
                }
                temp[j] = '\0';

                strcpy(token, temp);
            }
            if(chdir(token) == -1) {
                printf("ERROR: invalid hop command\n");
            }
        }
        printf("%s\n", getcwd(tempdir, sizeof(tempdir)));
        strcpy(curdir, tempdir);
        token = strtok(NULL, " \t");
        printnl();
    }
}