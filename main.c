#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

void shell_loop(void);
char *shell_read_line(void);
char **shell_split_line(char *line);
int shell_launch(char **args);
int shell_execute(char **pString);
int shell_num_builtins();

int shell_cd(char **args);
int shell_help(char **args);
int shell_pwd();
int shell_exit(char **args);



char *builtin_str[] = {
        "cd",
        "help",
        "pwd",
        "exit"
};

int (*builtin_func[]) (char **) = {
        &shell_cd,
        &shell_help,
        &shell_pwd,
        &shell_exit
};


int main(int argc, char **argv) {

    shell_loop();

    return EXIT_SUCCESS;
}




void shell_loop(void){
    char *line;
    char **args;
    int status;

    do{
        printf("Teddy shell ʕ•́ᴥ•̀ʔっ -> ");
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);
    } while(status);
}



char *shell_read_line(void){
    int bufsize = SHELL_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof (char) * bufsize);
    int c;

    if(!buffer){
        fprintf(stderr, "Teddy says: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        c = getchar();

        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }else{
            buffer[position++] = c;
        }

        if(position >= bufsize){
            bufsize += SHELL_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);
            if(!buffer){
                fprintf(stderr, "Teddy says: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **shell_split_line(char *line){
    int bufsize = SHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof (char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "Teddy says: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += SHELL_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Teddy says: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}



int shell_execute(char **args)
{
    int i;

    if (args[0] == NULL) {

        return 1;
    }

    for (i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return shell_launch(args);
}



int shell_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Teddy says:");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Teddy says:");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


int shell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int shell_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("Teddy says:");
        }
    }
    return 1;
}


int shell_pwd()
{
    system("pwd");
    return 1;
}


int shell_help(char **args)
{
    int i;
    printf("Welcome to the Teddy shell ʕ•́ᴥ•̀ʔっ \n");
    printf("These are the available commands:\n");

    for (i = 0; i < shell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit(char **args)
{
    return 0;
}

