#define _GNU_SOURCE
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<errno.h>
#include<math.h>
#include<signal.h>
#include<ftw.h>
#include<libgen.h>
#include<time.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<termios.h>

#define MAX_LOG_SIZE 15
#define MAX_LENGTH 4097
#define MAX_PROCESSES 4097
#define MAX_ALIAS_SIZE 4097
#define MAX_SIGNALS 32
#define MAX_PIPE_COMMANDS 15

int ishomedir(char* curdir, char* homedir);
void printCLI(int checkdir, char* username, char* hostname, char* homedir, char* curdir);
char* ftrim(char* str);
char* trim(char* str);
void hop_function(char* command, char* homedir, char* curdir, char* prevdir);
void print_with_color(char* filename, struct stat* file_stat);
void print_file_permissions(mode_t mode);
void print_file_details(char* filename, struct stat* file_stat);
void reveal_function(char* command, char* homedir);
char** log_to_array_helper(char* log_file_path, int* log_line_count);
void log_file_helper(char* command, char** log_file_text, int log_line_count, char* log_file_path);
char* log_function(char* command, char* log_file_path, char** log_file_text, int log_line_count, bool* log_execute_flag);
void print_log(char* log_file_path);
void purge_log(char* log_file_path);
int printCLI_with_comtime(int checkdir, char* username, char* hostname, char* homedir, char* curdir, char* command_temp, long elapsed_time_temp);
void syscom_function(char* command, bool background, bool* print_CLI_flag, char** command_temp, long* elapsed_time_temp);
void bg_process_signal(int sig);
void seek_function(char *command);
int display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
void proclore_function(char *pid, bool background);
char** alias_to_array_helper(char* alias_file_path, int* alias_line_count);
char* search_alias_file_text(char* command, char** alias_file_text, int alias_line_count);
void alias_function(char* command, char* alias_file_path);
void add_process(pid_t pid, const char* command);
void update_process_states();
int compare_processes(const void* a, const void* b);
void activities();
void ping_function(char* command);
void setup_signal_handling();
void ctrlc(int sig);
void ctrlz(int sig);
void kill_all_processes();
char* redirection_function(char* command);
void printnl();
void print_aliases(char** alias_file_text, int alias_line_count);
void handle_pipe(char* input, char** alias_file_text, char** log_file_text);
void command_handler(char* input, char** alias_file_text, char** log_file_text);
void make_non_blocking(int fd);
void make_blocking(int fd);
int kbhit();
int get_latest_PID();
void neonate_function(char* command);
void iMan_function(char* command);

typedef struct {
    pid_t pid;
    char command[MAX_LENGTH];
    char state;
} process_struct;

#endif