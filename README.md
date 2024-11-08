## Makefile :

### make - to create the executable file

### ./a.out - to run the program executable


## Header File :

### functions.h - Has all the used libraries, global variables and function defintions. Header file for all c files.


## Functions :

### misc.c :

#### 1) void printCLI(int checkdir, char* username, char* hostname, char* homedir, char* curdir) - function that prints the CLI before every command.

#### 2) int printCLI_with_comtime(int checkdir, char* username, char* hostname, char* homedir, char* curdir, char* command_temp, long elapsed_time_temp) - funtion that prints the CLI when elapsed time of a function > 2s.


### hop.c :

#### 1) int ishomedir(char* curdir, char* homedir) - returns 1 if curdir = homedir, 2 if curdir = homedir + /path and -1 if curdir != homedir.

#### 2) void hop_function(char* command, char* homedir, char* curdir, char* prevdir) - the function that handles the hop command.


### reveal.c :

#### 1) int compare_filenames(const void *a, const void *b) - the compare function for qsort.

#### 2) void print_with_color(char *filename, struct stat *file_stat) - the function to print files/directories based on color.

#### 3) void print_file_permissions(mode_t mode) - the function for reveal -l, used to print permissions of files/directory

#### 4) void print_file_details(char *filename, struct stat *file_stat) - the function for the final print of all details of a file/directory.

#### 5) void reveal_function(char* command, char* homedir) - the function that handles the reveal command.


### log.c :

#### 1) char** log_to_array_helper(char* log_file_path, int* log_line_count) - the function to convert the text in the log file to an array for further manipulation.

#### 2) void log_file_helper(char* command, char** log_file_text, int log_line_count, char* log_file_path) - the function that handles the printing/manipulation of the text printed to log file.

#### 3) void print_log(char* log_file_path) - the function that prints the log.

#### 4) void purge_log(char* log_file_path) - the function that clears the log.

#### 5) char* log_function(char* command, char* log_file_path, char** log_file_text, int log_line_count, bool* log_execute_flag) - the function that handles the log commands.


### syscom.c :

#### 1) void syscom_function(char* command, bool background, bool* print_CLI_flag, char** command_temp, long* elapsed_time_temp) - the function that handles system commands like sleep, echo, etc for the terminal using exec().

#### 2) void handle_sigchld(int sig) - the function that handles the background process exit status signals.

### proclore.c :

#### 1) void proclore_function(char *pid) - the function that handles proclore commands.


### seek.c :

#### 1) int display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) - the function that displays the files/directories we have seeked.

#### 2) void seek_function(char *command, char *homedir) - the function that handles seek command.


### alias.c :

#### 1) void print_aliases(char** alias_file_text, int alias_line_count) - prints all aliases along with their commands.

#### 2) char** alias_to_array_helper(char* alias_file_path, int* alias_line_count) - loads all aliases from .myshrc to a 2D character array.

#### 3) char* search_alias_file_text(char* command, char** alias_file_text, int alias_line_count) - searches the 2d array for existence of alias for every command entered.

#### 4) void alias_function(char* command, char* alias_file_path) - the function that handles the entire alias logic.

#### NOTE : Alias functionality supports dynamic aliasing.


### redirection-piping.c : file with logic for redirection and piping.

#### 1) char* redirection_function(char* command) - function that is called if there is a '>' in the command and further handles the redirection logic

#### 2) void handle_pipe(char* input, char** alias_file_text, char** log_file_text) - handles the piping logic and the flow of control for pipes.


### activities-ping.c : file with logic for activities and ping functionality.

#### 1) struct process_struct : the struct that stores all the info about a process. Used for activities and other functions that require shell-created processes list.

#### 2) void setup_signal_handling() : main signal handling function, captures and handles background signals, ctrlc and ctrlz.

#### 3) void bg_process_signal(int sig), void ctrlc(int sig), void ctrlz(int sig) : the functions with signal handling logic for the above cases when signal is captured by setup_signal_handling().

#### 4) void kill_all_processes() : function to kill all processes incase of ctrlD which is captured by input of shell = EOF.

#### 5) void add_process(pid_t pid, const char* command) : function to add processes to the global processes array.

#### 6) void update_process_states() and char process_state(pid_t pid) : functions that handle the updation of states of all processes in global processes array.

#### 7) void activities() : function that handles and calls all other relevant functions to display the activities.

#### 8) void ping_function(char* command) : function that handles all pings.


### neonate.c : BONUS

#### 1) int get_latest_PID() : function that gets the latest PID from proc file using opendir and pid logic.

#### 2) int kbhit() : function that handles input of 'x' and hence stops the neonate process.

#### 3) void make_non_blocking(int fd) and void make_blocking(int fd) : make shell invulnerable and vulnerable respectively to signals like ctrlc, ctrlz and ctrld.

#### 4) void neonate_function(char* command) : function that handles the entire process of neonate.


### networking.c :

#### 1) void iMan_function(char* command) : the function that creates the socket, makes the command, sends it as a request and then recieves the response from the man webpage and finally displays the content in a formatted manner using some basic string manipulation and slicing.


### main-logic.c : shifted the inner loop logic from main.c to main-logic.c to make it more modular and easier to pipe and handle returned commands.


## Implementations :

### 1) If input is interrupted by background process exit message, command is left there, enter can be pressed to refresh the CLI and input is given again.

### 2) Erroneous commands are stored in log. Any commands with log in it are not stored in log.

### 3) In case of multiple sleep commands, only latest one is appended to CLI.

### 4) Aliases are maintained for every run. Alias allows dynamic aliasing. Alias commands are of form "Alias name = command". Log stores the alias name along with the actual command.

### 5) If hop command is erroneous, we stay in home directory and path of homedir is printed.

### 6) In case of Redirection in self-implemented command, error message is redirected to file.

### 7) Activities are qsorted and displayed based on their PIDs and not lexicographically on names.

### 8) ctrl+D is only input when there is no fg process running. ctrl+D can be associated with EOF of input and there will be no other such case where EOF will be input.
