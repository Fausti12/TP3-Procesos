#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 100

char* quit_quotes(char *str) {
    int longitud = strlen(str);
    if ((str[0] == '\"' || str[0] == '\'') && (str[longitud - 1] == '\"' || str[longitud - 1] == '\'') && longitud > 1) {
        memmove(str, str + 1, longitud - 1);
        str[longitud - 2] = '\0';
    }
    return str;
}

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */ 

        /*create pipes to connect the output of one command to the input of the next command*/
        int pipefd[command_count - 1][2];
        for (int i = 0; i < command_count - 1; i++) 
        {
            if (pipe(pipefd[i]) == -1) 
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        /*create child processes to execute the commands*/
        for (int i = 0; i < command_count; i++) 
        {
            int pid = fork();
            if (pid == -1) 
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) 
            {
                if (i == 0) 
                {   /*first command*/
                    for (int j = 0; j < command_count - 1; j++) // iteratively close all the pipes that are not needed
                    {
                        if (j != 0){
                            if (close(pipefd[j][0]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                            if (close(pipefd[j][1]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        }
                        else{
                            if (close(pipefd[j][0]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        }
                        
                    }
                    if (dup2(pipefd[i][1], STDOUT_FILENO) == -1) { perror("dup2"); exit(EXIT_FAILURE);}
                } 
                else if (i == command_count - 1) 
                {   /*last command*/
                    for (int j = 0; j < command_count - 1; j++) // iteratively close all the pipes that are not needed
                    {
                        if (j != command_count - 2){
                            if (close(pipefd[j][0]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                            if (close(pipefd[j][1]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        }
                        else{
                            if (close(pipefd[j][1]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        }
                    }
                    if (dup2(pipefd[i - 1][0], STDIN_FILENO) == -1) { perror("dup2"); exit(EXIT_FAILURE);}
                } 
                else 
                {   /*middle commands*/
                    for (int j = 0; j < command_count - 1; j++) // iteratively close all the pipes that are not needed
                    {
                        if (j != i - 1 && j != i){
                            if (close(pipefd[j][0]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                            if (close(pipefd[j][1]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        } else if (j == i - 1){
                            if (close(pipefd[j][1]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        } else if (j == i){
                            if (close(pipefd[j][0]) == -1) { perror("close"); exit(EXIT_FAILURE);}
                        }
                    }
                    if (dup2(pipefd[i - 1][0], STDIN_FILENO) == -1) { perror("dup2"); exit(EXIT_FAILURE);}
                    if (dup2(pipefd[i][1], STDOUT_FILENO) == -1) { perror("dup2"); exit(EXIT_FAILURE);}
                }


                char *args[MAX_ARGS];
                char *token = strtok(commands[i], " ");
                int j = 0;
                while (token != NULL) // iteratively tokenize the arguments of the command
                {
                    args[j++] = quit_quotes(token);
                    token = strtok(NULL, " ");
                }
                args[j] = NULL;

                
                if (execvp(args[0], args) == -1) 
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            }
        }
        

        for (int i = 0; i < command_count - 1; i++) 
        {
            if (close(pipefd[i][0]) == -1) 
            {
                perror("close");
                exit(EXIT_FAILURE);
            }
            if (close(pipefd[i][1]) == -1) 
            {
                perror("close");
                exit(EXIT_FAILURE);
            }
        }

               
        for (int i = 0; i < command_count; i++) 
        {
            wait(NULL);
        }

        command_count = 0;
    }
    return 0;
}