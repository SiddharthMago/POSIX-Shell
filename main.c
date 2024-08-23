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

void print_with_color(const char *filename, const struct stat *file_stat) {
    if (S_ISDIR(file_stat->st_mode)) printf("\033[0;34m%s\033[0m\n", filename);  // Blue for directories
    else if (file_stat->st_mode & S_IXUSR) printf("\033[0;32m%s\033[0m\n", filename);  // Green for executables
    else printf("%s\n", filename);  // White for regular files
}

void print_file_permissions(mode_t mode) {
    // File Type
    char file_type = '-';
    if (S_ISDIR(mode)) file_type = 'd';
    else if (S_ISLNK(mode)) file_type = 'l';
    else if (S_ISCHR(mode)) file_type = 'c';
    else if (S_ISBLK(mode)) file_type = 'b';
    else if (S_ISFIFO(mode)) file_type = 'p';
    else if (S_ISSOCK(mode)) file_type = 's';
    printf("%c", file_type);

    // Owner Permissions
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');

    // Group Permissions
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');

    // Other Permissions
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c ", (mode & S_IXOTH) ? 'x' : '-');
}

void print_file_details(const char *filename, const struct stat *file_stat) {
    // Print permissions
    print_file_permissions(file_stat->st_mode);

    // Print number of hard links
    printf("%ld ", (long)file_stat->st_nlink);

    // Print owner and group names
    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group  *gr = getgrgid(file_stat->st_gid);
    printf("%s %s ", pw->pw_name, gr->gr_name);

    // Print file size
    printf("%5lld ", (long long)file_stat->st_size);

    // Print last modified time
    char time_str[20];
    struct tm *time_info = localtime(&file_stat->st_mtime);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", time_info);
    printf("%s ", time_str);

    print_with_color(filename, file_stat);
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
            char path[1024];

            token = strtok(NULL, " \t");

            if(token == NULL) {
                struct stat file_stat;
                struct dirent *de; 
                DIR *dr = opendir("."); 
                strcpy(path, ".");
                
                if (dr == NULL) { 
                    printf("Could not open current directory\n"); 
                    break;
                } 
            
                while ((de = readdir(dr)) != NULL) {
                    if(de->d_name[0] != '.') {
                        char full_path[1024];
                        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);
                        if (stat(full_path, &file_stat) == 0) {
                            print_with_color(de->d_name, &file_stat);
                        }
                    }
                }
            
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
                    struct stat file_stat;
                    struct dirent *de;
                    DIR *dr;

                    token = strtok(NULL, " \t");

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

                    while ((de = readdir(dr)) != NULL) {
                        char full_path[1024];
                        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);

                        if (stat(full_path, &file_stat) == 0) {

                            if(aflag == true && lflag == true) {
                                print_file_details(de->d_name, &file_stat);
                            }

                            else if (lflag == true) {
                                if(de->d_name[0] != '.') {
                                    print_file_details(de->d_name, &file_stat);
                                }
                            }

                            else if (aflag == true) {
                                print_with_color(de->d_name, &file_stat);
                            }

                        }

                        else {
                            printf("Could not get file status for %s\n", de->d_name);
                        }
                    }
                }

                else {
                    struct stat file_stat;
                    struct dirent *de;
                    DIR *dr;

                    if(token == NULL) dr = opendir(".");
                    else if(strcmp(token, "~") == 0) dr = opendir(homedir);
                    else dr = opendir(token);
        
                    if (dr == NULL) { 
                        printf("Could not open current directory\n"); 
                        break;
                    } 
                    
                    while ((de = readdir(dr)) != NULL) {

                        if(de->d_name[0] != '.') {
                            char full_path[1024];
                            snprintf(full_path, sizeof(full_path), "%s/%s", token ? token : ".", de->d_name);

                            if (stat(full_path, &file_stat) == 0) {
                                print_with_color(de->d_name, &file_stat);
                            }
                        }

                    }
            
                    closedir(dr);
                }

                token = strtok(NULL, " \t");

            }    
        }
    }
}