#include "functions.h"

char* log_file_name = "log.txt";

char** log_to_array_helper(char* log_file_path, int* log_line_count) {
    FILE* log_file = fopen(log_file_path, "r");
    if(log_file == NULL) {
        printe("ERROR: Failed to open log file for reading\n");
        return NULL;
    }
    
    char** log_file_text = (char**)malloc(MAX_LOG_SIZE * sizeof(char*));
    for(int i=0; i<MAX_LOG_SIZE; i++) {
        log_file_text[i] = (char*)malloc(MAX_LENGTH * sizeof(char));
    }

    while(fgets(log_file_text[*log_line_count], MAX_LENGTH, log_file) != NULL && *log_line_count < MAX_LOG_SIZE) {
        log_file_text[*log_line_count][strcspn(log_file_text[*log_line_count], "\n")] = 0;
        (*log_line_count)++;
    }
    fclose(log_file);

    return log_file_text;
}

void log_file_helper(char* command, char** log_file_text, int log_line_count, char* log_file_path) {
    if(log_line_count == 0 || strcmp(log_file_text[log_line_count - 1], command) != 0) {
        if(log_line_count >= MAX_LOG_SIZE) {
            for(int i = 1; i < MAX_LOG_SIZE; i++) {
                strcpy(log_file_text[i - 1], log_file_text[i]);
            }
            strcpy(log_file_text[MAX_LOG_SIZE - 1], command);
        }
        else {
            strcpy(log_file_text[log_line_count++], command);
        }

        FILE* log_file = fopen(log_file_path, "w");
        if(log_file == NULL) {
            printe("ERROR: Failed to open log file for writing\n");
            return;
        }

        for(int i = 0; i < log_line_count; i++) {
            fprintf(log_file, "%s\n", log_file_text[i]);
        }

        fclose(log_file);
    }
}

void print_log(char* log_file_path) {
    printnl();
    FILE* log_file = fopen(log_file_path, "r");

    char log_file_text[MAX_LOG_SIZE][MAX_LENGTH];
    int log_line_count = 0;

    while(fgets(log_file_text[log_line_count], sizeof(log_file_text[log_line_count]), log_file) != NULL && log_line_count < 15) {
        log_file_text[log_line_count][strcspn(log_file_text[log_line_count], "\n")] = 0;
        log_line_count++;
    }

    fclose(log_file);

    for(int i=0; i<log_line_count; i++) {
        printf("%i. %s\n", log_line_count - i, log_file_text[i]);
    }
    printnl();
}

void purge_log(char* log_file_path) {
    printnl();
    FILE* log_file = fopen(log_file_path, "w");
    if(log_file != NULL) {
        fclose(log_file);
        printf("Log Successfully Cleared\n");
        printnl();
        return;
    }
    printe("ERROR: Unable to open log file\n");
    printnl();
}

char* log_function(char* command, char* log_file_path, char** log_file_text, int log_line_count, bool* log_execute_flag) {
    char* token = strtok(command, " \t");
    token = strtok(NULL, " \t");

    if(token == NULL) {
        print_log(log_file_path);
    }

    else if(strcmp(token, "purge") == 0) {
        purge_log(log_file_path);
    }
    
    else if(strcmp(token, "execute") == 0) {
        token = strtok(NULL, " \t");

        if(token != NULL) {
            int num = atoi(token);
            if(num < 1 || num > 15) {
                printe("\nERROR: Invalid execute log line\n\n");
                return NULL;
            }
            if (log_line_count - num >= 0) {
                char* str = log_file_text[log_line_count - num];
                strcpy(command, str);
                *log_execute_flag = true;
            }
            else {
                printe("\nERROR: Log line number out of range\n\n");
            }
        }
        
        else {
            printe("\nERROR: Execute line not provided in command\n\n");
        }
    }

    else {
        printe("\nERROR: Invalid log command\n\n");
    }
        
    return command;
}