/* Funciones para manejar argumentos de línea de comando. */

#include <stdlib.h>
#include <stdio.h>


/* read_arguments
 * 
 * Lee los argumentos desde el arreglo de cadenas argv.
 * */

void read_arguments(int argc, char **argv, char **ppath, int *fromfiles,
											int *print_triangles)
{
	/* Chequear si hay suficientes argumentos. */
	if(argc < 4)
	{
		printf("Uso: %s [archivo puntos] ", argv[0]);
		printf("[threshold]\n");
		exit(EXIT_FAILURE);
	}
	
	/* Obtener argumentos de línea de comando. */
	*ppath = argv[1];
	*fromfiles = atoi(argv[2]);
	*print_triangles = atoi(argv[3]);
}
