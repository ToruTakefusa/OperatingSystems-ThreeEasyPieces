#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdnoreturn.h>

void GetToken(char *line, char* tokens[], int *length);
void EliminateNewLineCharacter(char *line);
void ReportError(void);
noreturn void DoInteract(void);
void ExecuteCommand(char* args[]);
unsigned long CountSize(char **args);

int main(int argc, char* argv[]) {

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
    // Todo: multiple whitespace.
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
    // Todo: divide.

    while(1) {
        char *line = NULL;
        size_t line_size = 0;
        char *to_free1;

        printf("wish> ");
        if (getline(&line, &line_size, stdin) == -1) {
            ReportError();
        }

        to_free1 = line;
        char **tokens = malloc(strlen(line));
        int last_index = 0;
        GetToken(line, tokens, &last_index);
        // Todo: erase.
        for (int i = 0; i <= last_index; ++i) {
            printf("toknes[%d] is %s\n", i, tokens[i]);
        }

        if ((last_index == 0) && (strcmp(tokens[0], "exit") == 0)) exit(1);

        char **cmd = malloc(CountSize(tokens) + sizeof ("/bin/"));
        int size = sizeof ("/bin/") + sizeof (tokens[0]);
        char tmp[size];
        strcat(tmp, "/bin/");
        strcat(tmp, tokens[0]);
        cmd[0] = tmp;

        for (int i = 1; i <= last_index; ++i) {
            cmd[i] = tokens[i];
        }
        cmd[last_index + 1] = NULL;

        if (access(cmd[0], X_OK) == -1) {
            ReportError();
        }

        ExecuteCommand(cmd);
        free(cmd);
        free(tokens);
        free(to_free1);
    }
}

/**
 * @brief execute_command execute commmands.
 * @param args args[0] must be a command that will be executed.
 * The last element of args should be NULL.
 */
void ExecuteCommand(char* args[]) {
    int rc = fork();
    if (rc == -1) {
        ReportError();
        exit(0);
    } else if (rc == 0){
        if (execv(args[0], args) == -1) {
            printf("Creating child missed\n");
            printf("The error is %s\n", strerror(errno));
            ReportError();
        }
    } else {
        if (wait(NULL) == -1)
            ReportError();
    }
}

