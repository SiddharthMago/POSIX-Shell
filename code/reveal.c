#include "functions.h"

int compare_filenames(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void print_with_color(char *filename, struct stat *file_stat) {
    if(S_ISDIR(file_stat->st_mode)) printf("\033[0;34m%s\033[0m\n", filename);  // Blue for directories
    else if(file_stat->st_mode & S_IXUSR) printf("\033[0;32m%s\033[0m\n", filename);  // Green for executables
    else printf("%s\n", filename);  // White for regular files
}

void print_file_permissions(mode_t mode) {
    // File Type
    char file_type = '-';
    if(S_ISDIR(mode)) file_type = 'd';
    else if(S_ISLNK(mode)) file_type = 'l';
    else if(S_ISCHR(mode)) file_type = 'c';
    else if(S_ISBLK(mode)) file_type = 'b';
    else if(S_ISFIFO(mode)) file_type = 'p';
    else if(S_ISSOCK(mode)) file_type = 's';
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

void print_file_details(char *filename, struct stat *file_stat) {
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

void reveal_function(char* command, char* homedir) {
    printnl();
    char* temp = strdup(command);
    char* token = strtok(temp, " \t");

    bool lflag = false;
    bool aflag = false;
    char path[MAX_LENGTH] = ".";

    token = strtok(NULL, " \t");
    while(token != NULL && token[0] == '-') {
        for(int i = 1; i < strlen(token); i++) {
            if(token[i] == 'a') aflag = true;
            else if(token[i] == 'l') lflag = true;
            else {
                printf("\033[0;31mERROR: Invalid flag: -%c\n\033[0m", token[i]);
                printnl();
                return;
            }
        }
        token = strtok(NULL, " \t");
    }

    if(token != NULL) {
        if(strcmp(token, "~") == 0) snprintf(path, sizeof(path), "%s", homedir);
        else snprintf(path, sizeof(path), "%s", token);
    }

    struct dirent *de;
    DIR *dr = opendir(path);
    if(dr == NULL) {
        printf("\033[0;31mERROR: Could not open directory %s\n\033[0m", path);
        printnl();
        return;
    }

    char **filenames = NULL;
    size_t filenames_count = 0;

    while((de = readdir(dr)) != NULL) {
        if(!aflag && de->d_name[0] == '.') continue;
        filenames = realloc(filenames, sizeof(char*) * (filenames_count + 1));
        filenames[filenames_count] = strdup(de->d_name);
        filenames_count++;
    }

    closedir(dr);

    qsort(filenames, filenames_count, sizeof(char*), compare_filenames);

    struct stat file_stat;
    for(size_t i=0; i<filenames_count; i++) {
        char full_path[MAX_LENGTH + 2];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, filenames[i]);

        if (stat(full_path, &file_stat) == 0) {
            if (lflag == true) print_file_details(filenames[i], &file_stat);
            else print_with_color(filenames[i], &file_stat);
        }
        else {
            printf("\033[0;31mERROR: Could not get file status for %s\n\033[0m", filenames[i]);
        }

        free(filenames[i]);
    }

    if(filenames != NULL) free(filenames);

    free(temp);
    printnl();
}