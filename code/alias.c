#include "functions.h"

bool alias_function_flag = false;

void print_aliases(char** alias_file_text, int alias_line_count) {
    printnl();
    for(int i=0; i<alias_line_count; i++) {
        printf("%s\n", alias_file_text[i]);
    }
    printnl();
}

char** alias_to_array_helper(char* alias_file_path, int* alias_line_count) {
    FILE* alias_file = fopen(alias_file_path, "r");
    if(alias_file == NULL) {
        printf("\nERROR: Alias file not found\n\n");
        return NULL;
    }

    char** alias_file_text = (char**)malloc(MAX_ALIAS_SIZE * sizeof(char*));
    for(int i=0; i<MAX_ALIAS_SIZE; i++) {
        alias_file_text[i] = (char*)malloc(MAX_LENGTH * sizeof(char));
    }

    char* buffer = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* buffer_temp = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* alias_command = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* alias_name = (char*)malloc(MAX_LENGTH * sizeof(char));
    bool alias_function = false;
    bool iter = false;

    while(fgets(buffer, MAX_LENGTH, alias_file) != NULL && *alias_line_count < MAX_ALIAS_SIZE) {
        strcspn(buffer, "\n");
        strcpy(buffer_temp, buffer);

        if(strstr(buffer, "()")) {
            char* token = strtok(buffer_temp, " \t");
            strcpy(alias_name, token);
            alias_name = trim(alias_name);
            alias_function = true;
            continue;
        }
        if(alias_function == true) {
            if(strchr(buffer, '{')) continue;
            if(strchr(buffer, '}')) {
                alias_function = false;
                continue;
            }

            if(iter == false) {
                strcspn(buffer, "\n");
                buffer = trim(buffer);
                snprintf(buffer_temp, MAX_LENGTH, "%s=%s", alias_name, buffer);
                strcpy(alias_command, buffer_temp);
                strcspn(alias_command, "\n");
                iter = true;
                continue;
            }

            if(iter == true) {
                buffer = trim(buffer);
                snprintf(buffer_temp, MAX_LENGTH, "%s ; %s", alias_command, buffer);
                strcpy(alias_command, buffer_temp);

                strcpy(alias_file_text[*alias_line_count], alias_command);
                alias_file_text[*alias_line_count][strcspn(alias_file_text[*alias_line_count], "\n")] = 0;
                // printf("alias command : %s\n", alias_file_text[*alias_line_count]);
                (*alias_line_count)++;

                iter = false;
                continue;
            }
        }

        strcpy(alias_file_text[*alias_line_count], buffer);
        alias_file_text[*alias_line_count][strcspn(alias_file_text[*alias_line_count], "\n")] = 0;
        (*alias_line_count)++;
    }
    fclose(alias_file);

    // print_aliases(alias_file_text, *alias_line_count);
    return alias_file_text;
}

char* search_alias_file_text(char* command, char** alias_file_text, int alias_line_count) {
    for(int i=0; i<alias_line_count; i++) {
        char* temp_command = strdup(command);
        char* token2 = strtok(temp_command, " \t");
        if(token2 == NULL) break;

        char temp_alias[MAX_LENGTH];
        strcpy(temp_alias, alias_file_text[i]);

        char* token = strtok(temp_alias, "=");
        if(token == NULL) break;

        if(strcmp(token, token2) == 0) {
            token = strtok(NULL, "=");
            alias_function_flag = true;
            return token;
        }
        free(temp_command);
    }
    return command;
}

void alias_function(char* command, char* alias_file_path) {
    printnl();
    if(command == NULL) return;

    char* alias_name = (char*)malloc(MAX_LENGTH * sizeof(char));
    char* alias_command = (char*)malloc(MAX_LENGTH * sizeof(char));

    char* token = strtok(command, "=");

    char* prefix = (char*)malloc(MAX_LENGTH * sizeof(char));
    strcpy(prefix, token);

    token = strtok(NULL, "=");
    if(token == NULL) {
        printf("ERROR: Invalid alias command\n");
        printnl();
        return;
    }

    strcpy(alias_command, token);
    alias_command = trim(alias_command);

    char* token2 = strtok(prefix, " \t");
    token2 = strtok(NULL, " \t");
    strcpy(alias_name, token2);
    alias_name = trim(alias_name);

    FILE* alias_file = fopen(alias_file_path, "a+");
    if(alias_file == NULL) {
        printf("ERROR: Alias file not found\n");
        printnl();
        return;
    }

    fprintf(alias_file, "%s=%s\n", alias_name, alias_command);
    fclose(alias_file);

    if(prefix != NULL) free(prefix);
    if(alias_name != NULL) free(alias_name);
    printnl();
}