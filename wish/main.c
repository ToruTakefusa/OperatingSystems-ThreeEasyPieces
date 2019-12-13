#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdnoreturn.h>
#include <fcntl.h>

void GetToken(char *line, char* tokens[], int *length);
void EliminateNewLineCharacter(char *line);
void ReportError(void);
noreturn void DoInteract(void);
void ExecuteCommand(char* args[], int size);
unsigned long CountSize(char **args);
void ChangeDirectory(char *tokens[], int last_index);
void SetPath(char *tokens[], int last_index);
static const int PATH_SIZE = 50;
char path[PATH_SIZE][PATH_SIZE];

int main(int argc, char* argv[]) {
    strcpy(path[0], "/bin/");
    if (argc > 2) {
        // todo:error
    } else if (argc == 1) {
        DoInteract();
    } else {
        // batch mode
    }
    return 0;
}


/**
 * @brief GetToken Get tokens from line.
 * Tokens are separated by white space or \t.
 * @param line
 * @param tokens
 */
void GetToken(char *line, char* tokens[], int *length) {
    EliminateNewLineCharacter(line);
    char *pos_white, *pos_tab;
    char *WHITE_SPACE = " ", *TAB = "\t", *EMPTY="";
    char *tmp;
    int count = 0;
    while(1) {
        pos_white = strchr(line, ' ');
        pos_tab = strchr(line, '\t');

        if (line == NULL) {
            return;
        }
        if ((pos_white == NULL) && (pos_tab == NULL)) {
            *length = count;
            tokens[count] = line;
            return;
        } else if ((pos_white != NULL) && (pos_tab == NULL)) {
            tmp = strsep(&line, WHITE_SPACE);
            if (!strcmp(tmp, EMPTY)) continue;
            tokens[count] = tmp;
        } else if ((pos_white == NULL) && (pos_tab != NULL)) {
            tmp = strsep(&line, TAB);
            if (!strcmp(tmp, EMPTY)) continue;
            tokens[count] = tmp;
        } else {
            if (pos_white < pos_tab) {
                tmp = strsep(&line, WHITE_SPACE);
                if (!strcmp(tmp, EMPTY)) continue;
                tokens[count] = tmp;
            } else {
                tmp = strsep(&line, TAB);
                if (!strcmp(tmp, EMPTY)) continue;
                tokens[count] = tmp;
            }
        }
        ++count;
        pos_white = NULL;
        pos_tab = NULL;
    }
}

/**
 * @brief EliminateNewLineCharacter: Elminate new line character from line.
 * @param line
 */
void EliminateNewLineCharacter(char *line) {
    char *pos_lf;
    pos_lf = strchr(line, '\n');
    if (pos_lf != NULL)
        *pos_lf = '\0';
}

/**
 * @brief ReportError output a error.
 */
void ReportError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/**
 * @brief CountSize: Get size of args. If there is null item, count will stop.
 * @param args
 * @return
 */
unsigned long CountSize(char **args) {
    unsigned long size = 0;
    for (int i = 0; args[i] != NULL; ++i) {
        size += sizeof (args[i]);
    }

    return size;
}

/**
 * @brief DoInteract execute in a interactive mode.
 */
noreturn void DoInteract() {
    while(1) {
        char *line = NULL;
        size_t line_size = 0;
        char *to_free1;

        printf("wish> ");
        if (getline(&line, &line_size, stdin) == -1) ReportError();

        to_free1 = line;

        char **tokens = malloc(strlen(line));
        int last_index = 0;
        GetToken(line, tokens, &last_index);
        // Todo: erase.
        for (int i = 0; i <= last_index; ++i) {
            printf("toknes[%d] is %s\n", i, tokens[i]);
        }

        // exit build-in commands
        if ((last_index == 0) && (strcmp(tokens[0], "exit") == 0)) exit(1);

        // cd build-in commands
        if (strcmp(tokens[0], "cd") == 0) {
            ChangeDirectory(tokens, last_index);
            continue;
        }

        // path build-in commands
        if (strcmp(tokens[0], "path") == 0) {
            SetPath(tokens, last_index);
            continue;
        }

        for (int i = 0; i < PATH_SIZE; ++i) {
            char **cmd = malloc(CountSize(tokens) + sizeof (path[i]));
            int size = sizeof (path[i]) + sizeof (tokens[0]) + 1;
            char tmp[size];
            strcat(tmp, path[i]);
            strcat(tmp, tokens[0]);
            cmd[0] = tmp;

            for (int i = 1; i <= last_index; ++i) {
                cmd[i] = tokens[i];
            }
            cmd[last_index + 1] = NULL;
            printf("tokens created\n");

            if (access(cmd[0], X_OK) == -1) {
                ReportError();
                free(cmd);
                continue;
            }

            ExecuteCommand(cmd, last_index + 1);
            free(cmd);
            free(tokens);
            free(to_free1);
            break;
        }
    }
}

/**
 * @brief execute_command execute commmands.
 * @param args args[0] must be a command that will be executed.
 * The last element of args should be NULL.
 */
void ExecuteCommand(char* args[], int size) {
    char *file_name;
    int redirect_symbol_exists = 0;
    // check > exists
    for (int i = 0; i < size; ++i) {
        if (strcmp(args[i], ">") == 0) {
            // > can used only once
            if (redirect_symbol_exists == 1) ReportError();
            // only one filename is allowed
            // todo fix when parallel
            if ((i + 1) != (size - 1)) ReportError();
            file_name = args[i + 1];
            redirect_symbol_exists = 1;
            args[i] = NULL;
            break;
        }
    }
    // todo parallel
    int rc = fork();
    if (rc == -1) {
        ReportError();
        exit(0);
    } else if (rc == 0){
        FILE *f = NULL;
        if (redirect_symbol_exists == 1) {
            creat(file_name, O_CREAT);
            f = fopen(file_name, "w");
            if (dup2(fileno(f), 1) == -1) ReportError();
            if (dup2(fileno(f), 2) == -1) ReportError();
            redirect_symbol_exists = 0;
        }
        if (execv(args[0], args) == -1) {
            printf("Creating child missed\n");
            printf("The error is %s\n", strerror(errno));
            ReportError();
        }
        if (redirect_symbol_exists) {
            close(fileno(f));
        }
    } else {
        if (wait(NULL) == -1)
            ReportError();
    }
}

void ChangeDirectory(char *tokens[], int last_index) {
    if (last_index != 1) ReportError();
    chdir(tokens[1]);
}

void SetPath(char *tokens[], int last_index) {
    if (last_index == 0) {
        for (int i = 0; i < PATH_SIZE; ++i) {
            strcpy(path[i], "");
        }
    } else {
        for (int i = 0; i < PATH_SIZE; ++i) {
            // last_index - 1
            if (i <= (last_index - 1)) {
                strcpy(path[i], tokens[i + 1]);
            } else {
                strcpy(path[i], "");
            }
        }
    }

}
