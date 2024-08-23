#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>

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
            // printf("<%s@%s:~>", username, hostname);
            printf("<");
            printf("\033[0;34m");
            printf("%s", username);
            printf("\033[0m");
            printf("@%s:~>", hostname);
        }
        else if(checkdir == 2){
            char mydir[1024];
            for(int i = strlen(homedir); i <= strlen(curdir); i++) {
                mydir[i-strlen(homedir)] = curdir[i];
            }
            // printf("<%s@%s:~/%s>", username, hostname, mydir);
            printf("<");
            printf("\033[0;34m");
            printf("%s", username);
            printf("\033[0m");
            printf("@%s:%s>", hostname, mydir);
        }
        else {
            // printf("<%s@%s:%s>", username, hostname, curdir);
            printf("<");
            printf("\033[0;34m");
            printf("%s", username);
            printf("\033[0m");
            printf("@%s:%s>", hostname, curdir);
        }

        char command[1024];
        scanf("%[^\n]", command);
        getchar();

        char* token = strtok(command, " \t");
        if(strcmp(token, "exit") == 0) {
            break;
        }
        else if(strcmp(token, "hop") == 0) {
            token = strtok(NULL, " \t");
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
                token = strtok(NULL, " \t");
            }
            printf("\n%s\n\n", getcwd(curdir, sizeof(curdir)));
        }
        
        else if(strcmp(token, "reveal") == 0) {
            bool lflag = false;
            bool aflag = false;
            
            token = strtok(NULL, " \t");

            if(token == NULL) {
                struct dirent *de; 
                DIR *dr = opendir("."); 
    
                if (dr == NULL) { 
                    printf("Could not open current directory\n"); 
                    break;
                } 
            
                while ((de = readdir(dr)) != NULL) if(de->d_name[0] != '.') printf("%s\n", de->d_name); 
            
                closedir(dr);
            }

            while(token != NULL) {
                
                if(token[0] == '-') {
                    for(int i = 1; i < strlen(token); i++) {
                        if(token[i] != 'a' && token[i] != 'l') {
                            aflag = false;
                            lflag = false;
                            break;
                        }
                        if(token[i] == 'a') aflag = true;
                        if(token[i] == 'l') lflag = true;
                    }
                }

                if(aflag == true || lflag == true) {
                    struct dirent *de;
                    DIR *dr;

                    token = strtok(NULL, " \t");
                    
                    char path[1024];

                    if(token != NULL && token[0] == '-') continue;
                    if(token == NULL) {
                        dr = opendir(".");
                        strcpy(path, ".");
                    }
                    else if(strcmp(token, "~") == 0) {
                        dr = opendir(homedir);
                        snprintf(path, sizeof(path), "%s", homedir);
                    }
                    else {
                        dr = opendir(token);
                        snprintf(path, sizeof(path), "%s", token);
                    }
    
                    if (dr == NULL) { 
                        printf("Could not open current directory\n");
                        break;
                    }

                    if(aflag == true && lflag == true) {
                        printf("Both a and l flags are present\n");
                        while ((de = readdir(dr)) != NULL) {
                            char full_path[1024];
                            snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);
                            struct stat file_stat;
                            if (stat(full_path, &file_stat) == 0) {
                                printf("File: %s\n", de->d_name);
                                printf("Size: %lld bytes\n", (long long)file_stat.st_size);
                                printf("Permissions: %o\n", file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                                printf("Last modified: %s", ctime(&file_stat.st_mtime));
                                printf("\n");
                            } 
                            else {
                                printf("Could not get file status\n");
                            }
                        }
                        closedir(dr);
                    }

                    else if(aflag == true) {
                        printf("a flag is present\n");
                        while ((de = readdir(dr)) != NULL) printf("%s\n", de->d_name);
                        closedir(dr);
                    }

                    else if(lflag == true) {
                        printf("l flag is present\n");
                        while ((de = readdir(dr)) != NULL) {
                            if (de->d_name[0] != '.') {
                                char full_path[1024];
                                snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);
                                struct stat file_stat;
                                if (stat(full_path, &file_stat) == 0) {
                                    printf("File: %s\n", de->d_name);
                                    printf("Size: %lld bytes\n", (long long)file_stat.st_size);
                                    printf("Permissions: %o\n", file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                                    printf("Last modified: %s", ctime(&file_stat.st_mtime));
                                    printf("\n");
                                } 
                                else {
                                    printf("Could not get file status\n");
                                }
                            }
                        }
                        closedir(dr);
                    }
                }

                else {
                    struct dirent *de; 
                    DIR *dr;

                    if(token == NULL) dr = opendir(".");
                    else if(strcmp(token, "~") == 0) dr = opendir(homedir);
                    else dr = opendir(token);
        
                    if (dr == NULL) { 
                        printf("Could not open current directory\n"); 
                        break;
                    } 
            
                    while ((de = readdir(dr)) != NULL) if(de->d_name[0] != '.') printf("%s\n", de->d_name); 
            
                    closedir(dr);
                }
                token = strtok(NULL, " \t");
            }    
        }
    }
}