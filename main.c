#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>

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

int main() {
    char* username = getenv("USER");
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));

    char homedir[1024];
    getcwd(homedir, sizeof(homedir));

    char prevdir[1024];
    getcwd(prevdir, sizeof(prevdir));

    while(1) {

        char curdir[1024];
        getcwd(curdir, sizeof(curdir));
        int checkdir = ishomedir(curdir, homedir);

        if(checkdir == 1) {
            printf("<%s@%s:~>", username, hostname);
        }
        else if(checkdir == 2){
            char mydir[1024];
            for(int i = strlen(homedir); i <= strlen(curdir); i++) {
                mydir[i-strlen(homedir)] = curdir[i];
            }
            printf("<%s@%s:%s>", username, hostname, mydir);
        }
        else {
            printf("<%s@%s:%s>", username, hostname, curdir);
        }

        char command[1024];
        scanf("%[^\n]", command);
        getchar();

        char* token = strtok(command, " ");
        if(strcmp(token, "exit") == 0) {
            break;
        }
        else if(strcmp(token, "hop") == 0) {
            token = strtok(NULL, " ");
            if(token == NULL) {
                strcpy(prevdir, curdir);
                chdir(homedir);
            }
            while(token != NULL) {
                if(strcmp(token, "~") == 0) {
                    strcpy(prevdir, curdir);
                    chdir(homedir);
                }
                else if(strcmp(token, "-") == 0) {
                    char temp[1024];
                    strcpy(temp, curdir);
                    chdir(prevdir);
                    strcpy(prevdir, temp);
                }
                else {
                    strcpy(prevdir, curdir);
                    int i=0;
                    if(token[0] == '~' || token[0] == '/') { // handles the case where the user enters ~ or / in the start new hop dir
                        while(token[i] == '~' || token[i] == '/') i++;
                        char temp[1024];
                        int j = 0;
                        while(j < strlen(token)) {
                            temp[j] = token[i + j];
                            j++;
                        }
                        temp[j] = '\0';

                        strcpy(token, temp);
                    }
                    chdir(token);
                }
                token = strtok(NULL, " ");
            }
            printf("\n%s\n\n", getcwd(curdir, sizeof(curdir)));
        }
    }    
    return 0;
}