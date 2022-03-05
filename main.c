#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_split_line(char *line);
int lsh_launch(char **args);
int lsh_execute(char **pString);
int lsh_num_builtins();

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_pwd();
int lsh_exit(char **args);



char *builtin_str[] = {
        "cd",
        "help",
        "pwd",
        "exit"
};

int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_csgo,
        &lsh_help,
        &lsh_pwd,
        &lsh_exit
};


int main(int argc, char **argv) {

    lsh_loop();

    return EXIT_SUCCESS;
}




void lsh_loop(void){
    char *line;
    char **args;
    int status;

    do{
        printf("Teddy shell ʕ•́ᴥ•̀ʔっ -> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while(status);
}



char *lsh_read_line(void){
    int bufsize = LSH_RL_BUFSIZE;
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
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);
            if(!buffer){
                fprintf(stderr, "Teddy says: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **lsh_split_line(char *line){
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof (char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "Teddy says: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line,LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Teddy says: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}



int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {

        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}



int lsh_launch(char **args)
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


int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args)
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


int lsh_pwd()
{
    system("pwd");
    return 1;
}


int lsh_help(char **args)
{
    int i;
    printf("Welcome to the Teddy shell ʕ•́ᴥ•̀ʔっ \n");
    printf("These are the available commands:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

