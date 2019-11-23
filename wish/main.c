#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void GetToken(char *line, char* tokens[], int *length);
void EliminateNewLineCharacter(char *line);
void ReportError(void);
void MakePath(char* dir, char* copy_from, char** copy_to);
void DoInteract(void);
void ExecuteCommand(char* args[]);

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
    char *WHITE_SPACE = " ", *TAB = "\t";
    int count = 0;
    while(1) {
        pos_white = strchr(line, ' ');
        pos_tab = strchr(line, '\t');

        if (line == NULL) {
            *length = count;
            return;
        }
        if ((pos_white == NULL) && (pos_tab == NULL)) {
            *length = count;
            tokens[count] = line;
            return;
        } else if ((pos_white != NULL) && (pos_tab == NULL)) {
            tokens[count] = strsep(&line, WHITE_SPACE);
        } else if ((pos_white == NULL) && (pos_tab != NULL)) {
            tokens[count] = strsep(&line, TAB);
        } else {
            if (pos_white < pos_tab) {
                tokens[count] = strsep(&line, WHITE_SPACE);
            } else {
                tokens[count] = strsep(&line, TAB);
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
 * @brief MakePath: make path from params.
 * @param dir: directory name which will be appended.
 * @param copy_from
 * @param copy_to: the content of copy_to is dir + copy_from
 */
void MakePath(char* dir, char* copy_from, char** copy_to)  {
    int size = sizeof (copy_from) + sizeof (copy_to);
    char tmp[size];
    strcat(tmp, dir);
    strcat(tmp, copy_from);
    copy_to[0] = tmp;
}

/**
 * @brief DoInteract execute in a interactive mode.
 */
void DoInteract() {
    // Todo: divide.

    while(1) {
        char *line = NULL;
        size_t line_size = 0;
        char *to_free;

        printf("wish> ");
        if (getline(&line, &line_size, stdin) == -1) {
            ReportError();
        }
        to_free = line;
        char **tokens = malloc(strlen(line));
        int last_index = 0;
        GetToken(line, tokens, &last_index);

        char **cmd = malloc(sizeof (tokens) + sizeof("/bin/"));

        //Todo MakePath("/bin/", tokens[0], *cmd);
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
        free(tokens);
        free(cmd);
        free(to_free);
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
            printf("The error is %s", strerror(errno));
            ReportError();
        }
    } else {
        if (wait(NULL) == -1)
            ReportError();
    }
}

