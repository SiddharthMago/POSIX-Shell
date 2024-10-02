#include "functions.h"

char* target_name;
char* homedir;
bool match_file = false;
bool match_dir = false;
bool singlefd = false;
int match_count = 0;

int display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *filename = fpath + ftwbuf->base;

    if(((match_file && typeflag == FTW_F) || (match_dir && typeflag == FTW_D) || (!match_file && !match_dir)) &&
        strcmp(target_name, filename) == 0) {
        match_count++;

        if(singlefd) {
            if(match_count > 1) {
                return 1;
            }

            if(match_count == 1) {
                if(typeflag == FTW_F) {
                    // Check if the directory has read permissions
                    if(access(fpath, R_OK) != 0) {
                        printe("ERROR : Missing permissions for task!\n");
                        return 1;
                    }
                    else {
                        FILE *file = fopen(fpath, "r");
                        if(file != NULL) {
                            char ch;
                            while((ch = fgetc(file)) != EOF) {
                                putchar(ch);
                            }
                            fclose(file);
                        }
                        printnl();
                        return 1;
                    }
                }
                else if(typeflag == FTW_D) {
                    // Check if the directory has execute permissions
                    if(access(fpath, X_OK) != 0) {
                        printe("ERROR : Missing permissions for task!\n");
                        return 1;
                    }
                    else {
                        // chdir(fpath);
                        if(chdir(fpath) == 0) printf("Changed directory to: %s\n", fpath);
                        else printe("ERROR : chdir");
                        return 1;
                    }
                }
            }
        }
        else {
            if(typeflag == FTW_F) printf("\033[0;34m%s\033[0m\n", fpath);
            else if(typeflag == FTW_D) printf("\033[0;32m%s\033[0m\n", fpath);
        }
    }

    return 0;
}

void seek_function(char *command) {
    printnl();
    match_count = 0;
    char *token = strtok(command, " \t");
    bool dflag = false;
    bool fflag = false;
    bool eflag = false;
    homedir = homedir;

    token = strtok(NULL, " \t");
    
    while(token != NULL && token[0] == '-') {
        for(int i = 1; i < strlen(token); i++) {
            if(token[i] == 'd') dflag = true;
            else if(token[i] == 'f') fflag = true;
            else if(token[i] == 'e') eflag = true;
            else {
                printf("\033[0;31mERROR : Invalid flag: -%c\n\033[0m", token[i]);
                printnl();
                return;
            }
        }
        token = strtok(NULL, " \t");
    }

    if(token == NULL) {
        printe("ERROR: No search target provided\n");
        printnl();
        return;
    }

    if(fflag && dflag) {
        printe("ERROR : Invalid flags!\n");
        printnl();
        return;
    }

    target_name = strdup(token);
    token = strtok(NULL, " \t");

    char* searchdir = (char*)malloc(sizeof(char) * MAX_LENGTH);
    if(token == NULL) strcpy(searchdir, ".");
    else strcpy(searchdir, token);

    match_file = fflag;
    match_dir = dflag;
    singlefd = eflag;

    if (nftw(searchdir, display_info, 20, FTW_PHYS) == -1) {
        printe("ERROR : nftw error\n");
        exit(EXIT_FAILURE);
    }

    if (match_count == 0) {
        printe("ERROR : No match found!\n");
    }
    printnl();
}