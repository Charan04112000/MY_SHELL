#ifndef PROG01_H
#define PROG01_H

#define MAX_INPUT 128
#define MAX_TOKENS 20
#define MAX_COMMANDS 10
#define MAX_PIPES 5
#define HIST_SIZE 10

typedef struct {
    char* cmd;
    int seq;
} History;

typedef struct {
    char* tokens[MAX_TOKENS + 1];
    int num_tokens;
    char* input_file;
    char* output_file;
    int num_pipes;
} Command;

typedef struct {
    Command commands[MAX_COMMANDS];
    int num_commands;
} CommandList;

void init_shell();
void add_to_history(char* cmd, int seq);
void display_history();
int parse_and_execute(char* input, int is_c_option);
void parse_input(char* input, CommandList* commands);
int is_builtin(char* cmd);
void execute_builtin(Command* cmd);
void execute_external(Command* cmd);
void free_commands(CommandList* commands);

#endif