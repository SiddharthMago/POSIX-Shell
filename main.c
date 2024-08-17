#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include<sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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
            token = strtok(NULL, " \t");
            if(token == NULL) {
                struct dirent *de; 
                DIR *dr = opendir("."); 
    
                if (dr == NULL) { 
                    printf("Could not open current directory" ); 
                    continue;
                } 
            
                while ((de = readdir(dr)) != NULL) if(de->d_name[0] != '.') printf("%s\n", de->d_name); 
            
                closedir(dr);
            }
            
            while(token != NULL) {
                if(strcmp(token, "-a") == 0) {
                    struct dirent *de; 
                    DIR *dr = opendir("."); 
    
                    if (dr == NULL) { 
                        printf("Could not open current directory" ); 
                        continue;
                    } 
            
                    while ((de = readdir(dr)) != NULL) printf("%s\n", de->d_name); 
            
                    closedir(dr);
                }
                else if(strcmp(token, "-l") == 0) {
                    struct dirent *de; 
                    DIR *dr = opendir("."); 
    
                    if (dr == NULL) { 
                        printf("Could not open current directory" ); 
                        continue;
                    } 
            
                    while ((de = readdir(dr)) != NULL) {
                        if(de->d_name[0] != '.') {
                            char path[1024];
                            snprintf(path, sizeof(path), "%s/%s", curdir, de->d_name);
                            struct stat file_stat;
                            if (stat(path, &file_stat) == 0) {
                                printf("File: %s\n", de->d_name);
                                printf("Size: %lld bytes\n", file_stat.st_size);
                                printf("Permissions: %o\n", file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                                printf("Last modified: %s", ctime(&file_stat.st_mtime));
                                printf("\n");
                            }
                        }
                    }
            
                    closedir(dr);
                }
                else if(strcmp(token, "~") == 0) {
                    struct dirent *de; 
                    DIR *dr = opendir(homedir); 
    
                    if (dr == NULL) { 
                        printf("Could not open current directory" ); 
                        continue;
                    } 
            
                    while ((de = readdir(dr)) != NULL) if(de->d_name[0] != '.') printf("%s\n", de->d_name); 
            
                    closedir(dr);
                }
                else {
                    struct dirent *de; 
                    DIR *dr = opendir(token); 
    
                    if (dr == NULL) { 
                        printf("Could not open current directory" ); 
                        continue;
                    } 
            
                    while ((de = readdir(dr)) != NULL) if(de->d_name[0] != '.') printf("%s\n", de->d_name); 
            
                    closedir(dr);
                }
                token = strtok(NULL, " \t");
            }
        }    
    }
}