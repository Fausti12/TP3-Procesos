#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*Como warm up para este primer ejercicio, el objetivo es implementar un esquema
de comunicación en forma de anillo para interconectar los procesos. En un esquema
de anillo se da con al menos tres procesos están conectados formando un bucle
cerrado. Cada proceso está comunicado exactamente con dos procesos: su
predecesor y su sucesor. Recibe un mensaje del predecesor y lo envía al sucesor.
En este caso, la comunicación se llevará a cabo a través de pipes, las cuales deben
ser implementadas.

Al inicio, alguno de los procesos del anillo recibirá un número entero como mensaje
a transmitir. Este mensaje será enviado al siguiente proceso en el anillo, quien, tras
recibirlo, lo incrementará en uno y luego lo enviará al siguiente proceso en el anillo.
Este proceso continuará hasta que el proceso que inició la comunicación reciba, del
último proceso, el resultado del mensaje inicialmente enviado.

Se sugiere que el programa inicial cree un conjunto de procesos hijos, que deben
ser organizados para formar un anillo. Por ejemplo, el hijo 1 recibe el mensaje, lo
incrementa y lo envía al hijo 2. Este último lo incrementa nuevamente y lo pasa al
hijo 3, y así sucesivamente, hasta llegar al último hijo, que incrementa el valor por
última vez y lo envía de vuelta al proceso padre. Este último debe mostrar el
resultado final del proceso de comunicación en la salida estándar.*/

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
			exit(1);
		}
	}

	for (i = 0; i < n; i++){  // Create n processes
		pid = fork();
		if (pid == -1){
			perror("Error creating process");
			exit(1);
		} else if (pid == 0) {
            // Estamos en el proceso hijo
            printf("Soy el proceso hijo %d con PID %d\n", i+1, getpid());
            break; // Salimos del proceso hijo
        } else {
            // Estamos en el proceso padre
            printf("Proceso padre creó al hijo %d con PID %d\n", i+1, pid);
		}
	}

	return 0;
}
