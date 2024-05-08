#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{	
	int start, status, pid, n;
	int buffer[1];

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    /* Parsing of arguments */
	n = atoi(argv[1]); // Number of processes
	buffer[0] = atoi(argv[2]); // Initial message
	start = atoi(argv[3]); // Start process 	

    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
   
   	/* You should start programming from here... */
	
	int fd[n][2]; // File descriptors for pipes
	int i;
	for (i = 0; i < n; i++){
		if (pipe(fd[i]) == -1){
			perror("Error creating pipe");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < n; i++){  // Create n processes
		pid = fork();
		if (pid == -1){
			perror("Error creating process");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
            // We are in the child process
            //printf("Soy el proceso hijo %d con PID %d\n", i, getpid());
            break;
        } else {
            // Estamos en el proceso padre
            //printf("Proceso padre creó al hijo %d con PID %d\n", i, pid);
			continue;
		}
	}

	if (pid == 0){
		// We are in the child process
		if (i == start){ // If it is the start process
			if (write(fd[i][1], buffer, sizeof(buffer)) == -1){
				perror("Error writing to pipe");
				exit(EXIT_FAILURE);
			}
			
			printf("Proceso %d recibió el mensaje %d desde el padre\n\n", i, buffer[0]);
		}
		if (read(fd[i][0], buffer, sizeof(buffer)) == -1){ // Check if there is an error reading from the pipe
			perror("Error reading from pipe");
			exit(EXIT_FAILURE);
		}
		
		printf("Proceso %d recibió el mensaje %d\n", i, buffer[0]);

		buffer[0]++;
		
		if (write(fd[(i+1)%n][1], buffer, sizeof(buffer)) == -1){ // Check if there is an error writing to the pipe
			perror("Error writing to pipe");
			exit(EXIT_FAILURE);
		}
		printf("Proceso %d envió el mensaje %d\n\n", i, buffer[0]);

		// close all file descriptors
		for (int j = 0; j < n; j++){
			if (close(fd[j][0]) == -1){ perror("close"); exit(EXIT_FAILURE);}
			if (close(fd[j][1]) == -1){ perror("close"); exit(EXIT_FAILURE);}
		}
	}
	else {
		// We are in the parent process
		wait(&status);
		if (read(fd[start][0], buffer, sizeof(buffer)) == -1){ // Check if there is an error reading from the pipe
			perror("Error reading from pipe");
			exit(EXIT_FAILURE);
		}
		// close all file descriptors
		for (int j = 0; j < n; j++){
			if (close(fd[j][0]) == -1){ perror("close"); exit(EXIT_FAILURE);}
			if (close(fd[j][1]) == -1){ perror("close"); exit(EXIT_FAILURE);}
		}
		printf("El mensaje final es: %d\n", buffer[0]);
	}
	
	return 0;
}