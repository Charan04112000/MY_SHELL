#include "prog01.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

History history[HIST_SIZE];
int cmd_sequence = 1;

void init_shell() {
    for (int i = 0; i < HIST_SIZE; i++) {
        history[i].cmd = NULL;
        history[i].seq = 0;
    }
}

void add_to_history(char* cmd, int seq) {
    static int hist_index = 0;
    if (strlen(cmd) > 0) {
        if (history[hist_index].cmd) free(history[hist_index].cmd);
        history[hist_index].cmd = strdup(cmd);
        history[hist_index].seq = seq;
        hist_index = (hist_index + 1) % HIST_SIZE;
    }
}

void display_history() {
    for (int i = 0; i < HIST_SIZE; i++) {
        if (history[i].cmd && history[i].seq > 0) {
            printf("%d %s\n", history[i].seq, history[i].cmd);
        }
    }
}

int parse_and_execute(char* input, int is_c_option) {
    CommandList commands = {0};
    parse_input(input, &commands);

    if (commands.num_commands == 0) return 0;

    for (int i = 0; i < commands.num_commands; i++) {
        Command* cmd = &commands.commands[i];

        if (is_builtin(cmd->tokens[0])) {
            if (is_c_option) {
                fprintf(stderr, "Built-in commands not allowed with -c option\n");
                return 1;
            }
            execute_builtin(cmd);
        } else {
            execute_external(cmd);
        }
    }

    free_commands(&commands);
    return 0;
}

void parse_input(char* input, CommandList* commands) {
    char* token = strtok(input, ";");
    while (token && commands->num_commands < MAX_COMMANDS) {
        Command* cmd = &commands->commands[commands->num_commands];
        cmd->num_tokens = 0;
        cmd->input_file = NULL;
        cmd->output_file = NULL;

        char* pipe_token = strtok(token, "|");
        while (pipe_token && cmd->num_pipes < MAX_PIPES) {
            char* word = strtok(pipe_token, " \t");
            while (word && cmd->num_tokens < MAX_TOKENS) {
                if (strcmp(word, "<") == 0) {
                    word = strtok(NULL, " \t");
                    if (word) cmd->input_file = strdup(word);
                } else if (strcmp(word, ">") == 0) {
                    word = strtok(NULL, " \t");
                    if (word) cmd->output_file = strdup(word);
                } else {
                    cmd->tokens[cmd->num_tokens++] = strdup(word);
                }
                word = strtok(NULL, " \t");
            }
            cmd->tokens[cmd->num_tokens] = NULL;
            cmd->num_pipes++;
            pipe_token = strtok(NULL, "|");
        }
        commands->num_commands++;
        token = strtok(NULL, ";");
    }
}

int is_builtin(char* cmd) {
    if (!cmd) return 0;
    return (strcmp(cmd, "hist") == 0 || strcmp(cmd, "curPid") == 0 ||
            strcmp(cmd, "pPid") == 0 || strcmp(cmd, "cd") == 0 ||
            strcmp(cmd, "quit") == 0);
}

void execute_builtin(Command* cmd) {
    if (strcmp(cmd->tokens[0], "hist") == 0) {
        display_history();
    } else if (strcmp(cmd->tokens[0], "curPid") == 0) {
        printf("%d\n", getpid());
    } else if (strcmp(cmd->tokens[0], "pPid") == 0) {
        printf("%d\n", getppid());
    } else if (strcmp(cmd->tokens[0], "cd") == 0) {
        if (cmd->num_tokens > 1) {
            if (chdir(cmd->tokens[1]) != 0) {
                perror("cd failed");
            }
        }
    } else if (strcmp(cmd->tokens[0], "quit") == 0) {
        exit(0);
    }
}

void execute_external(Command* cmd) {
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES];

    for (int i = 0; i < cmd->num_pipes - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return;
        }
    }

    for (int i = 0; i < cmd->num_pipes; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            return;
        }
        if (pids[i] == 0) { // Child process
            if (i > 0) { // Not first command
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
                close(pipes[i-1][1]);
            }
            if (i < cmd->num_pipes - 1) { // Not last command
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            if (i == 0 && cmd->input_file) {
                int fd = open(cmd->input_file, O_RDONLY);
                if (fd == -1) {
                    perror("input redirection failed");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (i == cmd->num_pipes - 1 && cmd->output_file) {
                int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("output redirection failed");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(cmd->tokens[0], cmd->tokens);
            perror("execvp failed");
            exit(1);
        }
    }

    for (int i = 0; i < cmd->num_pipes - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd->num_pipes; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

void free_commands(CommandList* commands) {
    for (int i = 0; i < commands->num_commands; i++) {
        Command* cmd = &commands->commands[i];
        for (int j = 0; j < cmd->num_tokens; j++) {
            free(cmd->tokens[j]);
        }
        if (cmd->input_file) free(cmd->input_file);
        if (cmd->output_file) free(cmd->output_file);
    }
}

int main(int argc, char* argv[]) {
    char input[MAX_INPUT];
    char hostname[256];
    init_shell();

    gethostname(hostname, sizeof(hostname));

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        add_to_history(argv[2], cmd_sequence++);
        parse_and_execute(argv[2], 1);
        return 0;
    }

    while (1) {
        printf("<%d %s>", cmd_sequence, hostname);
        if (!fgets(input, MAX_INPUT, stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;

        add_to_history(input, cmd_sequence++);
        parse_and_execute(input, 0);
    }

    return 0;
}
~                                          