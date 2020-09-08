/* Funciones para manejar entrada y salida de datos. */

#define _POSIX_C_SOURCE 200112L /* Para popen, pclose y posix_memalign. */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "consts.h"
#include "triang.h"

/*#define cmdpath "/home/jojeda/qhull-2015.2/bin/qdelaunay"*/
#define filespath "input/"
#define filespathoutput "output/"
#define cmdname "qdelaunay"

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define debug_block(fmt) do { if (DEBUG_TEST){ fmt }} while (0)
#define debug_print(fmt, ...) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#define debug_msg(fmt) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__,  __LINE__, __func__); } while (0)

/*read files*/
/*Lee tres archivos, uno de nodes, otro de triangulos y otro de t adj*/
void read_fromfiles_data(char *ppath, double **r, int **p, int **adj, int *pnumber, int *tnumber, void **align_settings)
{
	
	int i;
	int tmp_int_id;
	int tmp_int0;
	int tmp_int1;
	int tmp_int2;
	double tmp_dbl0;
	double tmp_dbl1;
	int flag = FALSE;
	int alignment;
	int (*new_aligned_mem_size)(int);

	if(align_settings != NULL)
	{
		/* Obtener alineamiento y función para calcular el tamaño
		 * de la memoria alineada. */
		alignment = *((int *)(align_settings[0]));
		new_aligned_mem_size = align_settings[1];
	}
	
	/* guardar puntos */
	char cmd[1024] = "\0";
	strcat(cmd, filespath);
	strcat(cmd, ppath);
	strcat(cmd,".node");
	
	

	FILE *nodes = fopen(cmd, "r");

	if(nodes == NULL)
	{
		printf("** ERROR ** read_fromfiles_data: No se pudo abrir el archivo %s\n %s",cmd, strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* Leer número de puntos. */
	fscanf(nodes, "%d %d %d %d", pnumber, &tmp_int0, &tmp_int1, &tmp_int2);

	if(align_settings == NULL)
	{
		*r = (double *)malloc(2*(*pnumber)*sizeof(double));
	}
	else
	{
		posix_memalign((void **)r, alignment, new_aligned_mem_size(2*(*tnumber)*sizeof(double)));
	}

	/*verifica si los indices comienzan con 1*/
	fscanf(nodes, "%d %lf %lf %d", &tmp_int_id, &tmp_dbl0, &tmp_dbl1, &tmp_int0);
	(*r)[0] = tmp_dbl0;
	(*r)[1] = tmp_dbl1;
	
	if(tmp_int_id == 1){
		flag = TRUE;
		debug_msg("El indice del archivo node es 1\n");
	}else
	{
		debug_msg("El indice del archivo node es 0\n");
	}

	for(i = 1; i < *pnumber; i++)
	{
		fscanf(nodes, "%d %lf %lf %d", &tmp_int_id, &tmp_dbl0, &tmp_dbl1, &tmp_int0);
		
		/* Inicializar campos. */
		(*r)[2*i + 0] = tmp_dbl0;
		(*r)[2*i + 1] = tmp_dbl1;
		/* printf("%d %lf %lf\n", i, tmp_dbl0, tmp_dbl1); */
	}
	
	fclose(nodes);

	

	/*triangulos */
	cmd[0] = '\0';
	strcat(cmd, filespath);
	strcat(cmd, ppath);
	strcat(cmd,".ele");
	FILE *triangles = fopen(cmd, "r");
	if(triangles == NULL)
	{
		printf("** ERROR ** read_fromfiles_data: No se pudo abrir el archivo %s\n %s",cmd, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fscanf(triangles, "%d 3 0\n", tnumber);
	
	if(align_settings == NULL)
	{
		*p = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)p, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	

	if(flag == FALSE){
		/* Leer puntos de cada triángulo y verterlos en arreglo. */
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(triangles, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			
			/* Inicializar campos. */
			(*p)[3*i + 0] = tmp_int0;
			(*p)[3*i + 1] = tmp_int1;
			(*p)[3*i + 2] = tmp_int2;
			
		}
	}else
	{
		/* Leer puntos de cada triángulo y verterlos en arreglo. */
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(triangles, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			
			/* Inicializar campos. */
			(*p)[3*i + 0] = tmp_int0-1;
			(*p)[3*i + 1] = tmp_int1-1;
			(*p)[3*i + 2] = tmp_int2-1;
			
		}
	}
	

	
	fclose(triangles);


	/*vecindad */
	cmd[0] = '\0';
	strcat(cmd, filespath);
	strcat(cmd, ppath);
	strcat(cmd,".neigh");
	FILE *tadj = fopen(cmd, "r");

	if(tadj == NULL)
	{
		printf("** ERROR ** read_fromfiles_data: No se pudo abrir el archivo %s\n %s",cmd, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fscanf(tadj, "%d 3\n", tnumber);
	
	if(align_settings == NULL)
	{
		*adj = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)adj, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	
	if(flag == FALSE){
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(tadj, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			//debug_print("%d %d %d %d\n", tmp_int_id, tmp_int0, tmp_int1, tmp_int2);
			/* Inicializar campos. Si no hay adyacencia, colocar  NO_ADJ. */
			(*adj)[3*i + 0] = (tmp_int0 < 0) ? TRIANG_BORDER : tmp_int0;
			(*adj)[3*i + 1] = (tmp_int1 < 0) ? TRIANG_BORDER : tmp_int1;
			(*adj)[3*i + 2] = (tmp_int2 < 0) ? TRIANG_BORDER : tmp_int2;
			
		}
	}else
	{
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(tadj, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			
			/* Inicializar campos. Si no hay adyacencia, colocar  NO_ADJ. */
			(*adj)[3*i + 0] = (tmp_int0 < 0) ? TRIANG_BORDER : tmp_int0-1;
			(*adj)[3*i + 1] = (tmp_int1 < 0) ? TRIANG_BORDER : tmp_int1-1;
			(*adj)[3*i + 2] = (tmp_int2 < 0) ? TRIANG_BORDER : tmp_int2-1;
			
		}
	}
	
	

	
	fclose(tadj);
}




/* read_qdelaunay_data
 * 
 * Introduce el archivo de puntos ppath a qdelaunay y luego introduce
 * los datos que genera a r, p, adj, a. A pnumber y tnumber le introduce
 * el número de puntos y triángulos, correspondientemente.
 * 
 * El puntero a áreas puede ser NULL.
 * 
 * También recibe align_settings que es un arreglo de 2 componentes:
 * un puntero a entero que indica alineación y un puntero a función que
 * toma el tamaño de la memoria a reservar y retorna un entero.
 * */

void read_qdelaunay_data(char *ppath, double **r, int **p, int **adj, int *pnumber, int *tnumber, void **align_settings)
{
	FILE *qdp;
	
	char cmd[1024];
	int alignment;
	int (*new_aligned_mem_size)(int);
	
	if(align_settings != NULL)
	{
		/* Obtener alineamiento y función para calcular el tamaño
		 * de la memoria alineada. */
		alignment = *((int *)(align_settings[0]));
		new_aligned_mem_size = align_settings[1];
	}
	
	/* Construir comando a ejecutar. */
	strcpy(cmd,  cmdname);
	strcat(cmd, " Fv Fn Fa QJ < "); /* QJ para que no hayan triángulos iguales!! */
	strcat(cmd, ppath);
	qdp = popen(cmd, "r");
	
	if(qdp == NULL)
	{
		printf("** ERROR ** read_qdelaunay_data: No se pudo ejecutar qdelaunay.\n" );
		exit(EXIT_FAILURE);
	}
	
	int i;
	int tmp_int0;
	int tmp_int1;
	int tmp_int2;
	double tmp_dbl0;
	double tmp_dbl1;
	
	/* Leer datos de hijo, a través de tubería.
	 * Leer número de triángulos. */
	fscanf(qdp, "%d\n", tnumber);
	
	if(align_settings == NULL)
	{
		*p = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)p, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	
	/* Leer puntos de cada triángulo y verterlos en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "3 %d %d %d\n", &tmp_int0, &tmp_int1, &tmp_int2);
		
		/* Inicializar campos. */
		(*p)[3*i + 0] = tmp_int0;
		(*p)[3*i + 1] = tmp_int1;
		(*p)[3*i + 2] = tmp_int2;
	}
  
	/* Ignorar línea */
	fscanf(qdp, "%*[^\n]\n");
	
	if(align_settings == NULL)
	{
		*adj = (int *)malloc(3*(*tnumber)*sizeof(int));
	}
	else
	{
		posix_memalign((void **)adj, alignment, new_aligned_mem_size(3*(*tnumber)*sizeof(int)));
	}
	
	/* Leer identificadores de triaǵnulos adyacentes a
	 * cada triángulo y verterlos en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "3 %d %d %d\n", &tmp_int0, &tmp_int1, &tmp_int2);
		
		/* Inicializar campos. Si no hay adyacencia, colocar  NO_ADJ. */
		(*adj)[3*i + 0] = (tmp_int0 < 0) ? TRIANG_BORDER : tmp_int0;
		(*adj)[3*i + 1] = (tmp_int1 < 0) ? TRIANG_BORDER : tmp_int1;
		(*adj)[3*i + 2] = (tmp_int2 < 0) ? TRIANG_BORDER : tmp_int2;
	}
	
	/* Ignorar línea */
	fscanf(qdp, "%*[^\n]\n");
	
	
	/* Pudiera quedar algo por leer, pero no interesa. */
	while(fscanf(qdp, "%*[^\n]\n") > 0)
	{
		/* Descartar. */
	}
	
	pclose(qdp);
	
	FILE *pf = fopen(ppath, "r");
	
	/* Ignorar primera línea. */
	fscanf(pf, "%*[^\n]\n");
	
	/* Leer número de puntos. */
	fscanf(pf, "%d", pnumber);
	
	if(align_settings == NULL)
	{
		*r = (double *)malloc(2*(*tnumber)*sizeof(double));
	}
	else
	{
		posix_memalign((void **)r, alignment, new_aligned_mem_size(2*(*tnumber)*sizeof(double)));
	}
	
	for(i = 0; i < *pnumber; i++)
	{
		fscanf(pf, "%lf %lf", &tmp_dbl0, &tmp_dbl1);
		
		/* Inicializar campos. */
		(*r)[2*i + 0] = tmp_dbl0;
		(*r)[2*i + 1] = tmp_dbl1;
	}
	
	fclose(pf);
}


/*geomview output*/
void write_geomview(double *r, int *p,  int pnumber,
									int tnumber, int ind_poly, 
									int *polygon, int id_start_poly,int *start_polygon, int print_triangles, char *ppath,
									int *chose_seed_triangle, int id_chose_seed_triangle){

	int i,j;
	
	char cmd[1024] = "\0";
	strcat(cmd, filespathoutput);
	strcat(cmd, ppath);
	strcat(cmd,".off");
	debug_print("Imprimiendo archivo en %s\n", cmd);
	FILE *fptr;
    fptr = fopen(cmd, "w");
	if(fptr == NULL){
		printf("Hmnito, no abrio el archivo :C\n");
		perror("fopen");
		exit(0);
	}
	
	if (print_triangles)
	{
		debug_print("Imprimiendo %d triangulos semilla", id_chose_seed_triangle);

		fprintf(fptr, "{ appearance  {-edge +face linewidth 2} LIST\n");
		for(j = 0; j < id_chose_seed_triangle; j++){

			i = chose_seed_triangle[j];
			fprintf(fptr,"{ OFF 3 1 1\n");
			fprintf(fptr,"%.3f %.3f 0\n%.3f %.3f 0\n%.3f %.3f 0\n",   r[2*p[3*i + 0] + 0], r[2*p[3*i + 0] + 1], 
																r[2*p[3*i + 1] + 0], r[2*p[3*i + 1] + 1], 
																r[2*p[3*i + 2] + 0], r[2*p[3*i + 2] + 1] );
			
			fprintf(fptr,"3 0 1 2  1.0 0.0 1.0 1 }\n");
		}
	}
	debug_print("Imprimiendo %d poligonos\n", id_start_poly);
	fprintf(fptr, "{ appearance  {+edge +face linewidth 2} LIST\n");
	fprintf(fptr, "OFF\n");
	fprintf(fptr,"%d %d 0\n", pnumber, id_start_poly);
	for(i = 0; i < pnumber; i++)
		fprintf(fptr,"%.3f %.3f 0\n", r[2*i + 0], r[2*i + 1]);
	
	int anterior = 0;
	for(j = 0,i = 0; i< id_start_poly; i ++){
		fprintf(fptr,"%d ", start_polygon[i] - anterior);
		for(; j <  start_polygon[i] ; j = j + 1 ){
				fprintf(fptr,"%d ", polygon[j]);
				
		}
		anterior = start_polygon[i] ;
		fprintf(fptr,"\n");
	}

	
	if(print_triangles){
		debug_msg("Imprimiendo malla delaunay\n");
		fprintf(fptr, "{ appearance  {+edge -face linewidth 2} LIST\n");
		int p0, p1,p2;
		for(i = 0; i < tnumber; i++){
			p0 = 3*i + 0;
			p1 = 3*i + 1;
			p2 = 3*i + 2;
			fprintf(fptr,"# %d %d\n", p0, p1);
			fprintf(fptr,"VECT 1 2 1 2 1\n");
			fprintf(fptr,"%.3f %.3f 0\n%.3f %.3f 0\n",	r[2*p[p0]+0], r[2*p[p0]+1], 
													r[2*p[p1]+0], r[2*p[p1]+1]);
			fprintf(fptr,"0 1 1 1\n");
			
			fprintf(fptr,"# %d %d\n", p1, p2);
			fprintf(fptr,"VECT 1 2 1 2 1\n");
			fprintf(fptr,"%.3f %.3f 0\n%.3f %.3f 0\n",	r[2*p[p1]+0], r[2*p[p1]+1], 
													r[2*p[p2]+0], r[2*p[p2]+1]);
			fprintf(fptr,"0 1 1 1\n");

			fprintf(fptr,"# %d %d\n", p0, p2);
			fprintf(fptr,"VECT 1 2 1 2 1\n");
			fprintf(fptr,"%.3f %.3f 0\n%.3f %.3f 0\n",	r[2*p[p0]+0], r[2*p[p0]+1], 
													r[2*p[p2]+0], r[2*p[p2]+1]);
			fprintf(fptr,"0 1 1 1\n");
		}
	}

	fprintf(fptr," }\n");
	if(print_triangles){
		fprintf(fptr," }\n");
		fprintf(fptr," }\n");
	}
    fclose(fptr);

}


/* read_triangles
 * 
 * Lee los identificadores de los triángulos contenidos en el archivo cpath.
 * */

void read_triangles(char *cpath, int **id, int *inumber)
{
	FILE *pf;
	
	int tmp_int0;
	int tmp_int1;
	double tmp_dbl0;
	int i;
	
	pf = fopen(cpath, "r");
	
	if(pf == NULL)
	{
		printf("** ERROR ** read_classification: No se pudo abrir '%s'.\n", cpath);
		exit(EXIT_FAILURE);
	}
	
	/* Leer número de regiones (zonas + no-zonas). */
	fscanf(pf, "%d", inumber);
	
	*id = (int *)malloc((*inumber)*sizeof(int));
	
	for(i = 0; i < *inumber; i++)
	{
		fscanf(pf, "%d %d %lf", &tmp_int0, &tmp_int1, &tmp_dbl0);
		
		/* Inicializar campos. */
		(*id)[i] = tmp_int1;
	}
	
	fclose(pf);
}
