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
#define cmdname "qdelaunay"

#define NZ_FILE_SUFFIX "_nz.dat"
#define IV_FILE_SUFFIX "_iv.dat"
#define BV_FILE_SUFFIX "_bv.dat"
#define W_FILE_SUFFIX "_w.dat"


/*read files*/
/*Lee tres archivos, uno de nodes, otro de triangulos y otro de t adj*/
void read_fromfiles_data(char *ppath, double **r, int **p, int **adj,
													double **a, int *pnumber, int *tnumber, void **align_settings)
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
	/*printf("%d %lf %lf\n", i, tmp_dbl0, tmp_dbl1);*/
	if(tmp_int_id == 1) 
		flag = TRUE;

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
	
	if(!flag){
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


	/*triangulos */
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
	
	if(!flag){
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(tadj, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			
			/* Inicializar campos. Si no hay adyacencia, colocar  NO_ADJ. */
			(*adj)[3*i + 0] = (tmp_int0 < 0) ? NO_ADJ : tmp_int0;
			(*adj)[3*i + 1] = (tmp_int1 < 0) ? NO_ADJ : tmp_int1;
			(*adj)[3*i + 2] = (tmp_int2 < 0) ? NO_ADJ : tmp_int2;
			
		}
	}else
	{
		for(i = 0; i < *tnumber; i++)
		{
			fscanf(tadj, "%d %d %d %d\n", &tmp_int_id, &tmp_int0, &tmp_int1, &tmp_int2);
			
			/* Inicializar campos. Si no hay adyacencia, colocar  NO_ADJ. */
			(*adj)[3*i + 0] = (tmp_int0 < 0) ? NO_ADJ : tmp_int0-1;
			(*adj)[3*i + 1] = (tmp_int1 < 0) ? NO_ADJ : tmp_int1-1;
			(*adj)[3*i + 2] = (tmp_int2 < 0) ? NO_ADJ : tmp_int2-1;
			
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

void read_qdelaunay_data(char *ppath, double **r, int **p, int **adj,
													double **a, int *pnumber, int *tnumber, void **align_settings)
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
		(*adj)[3*i + 0] = (tmp_int0 < 0) ? NO_ADJ : tmp_int0;
		(*adj)[3*i + 1] = (tmp_int1 < 0) ? NO_ADJ : tmp_int1;
		(*adj)[3*i + 2] = (tmp_int2 < 0) ? NO_ADJ : tmp_int2;
	}
	
	/* Ignorar línea */
	fscanf(qdp, "%*[^\n]\n");
	
	if(a != NULL)
	{
		if(align_settings == NULL)
		{
			*a = (double *)malloc((*tnumber)*sizeof(double));
		}
		else
		{
			posix_memalign((void **)a, alignment, new_aligned_mem_size((*tnumber)*sizeof(double)));
		}
	}
	
	/* Leer áreas de cada triángulo y verterlas en arreglo. */
	for(i = 0; i < *tnumber; i++)
	{
		fscanf(qdp, "%lf\n", &tmp_dbl0);
		
		if(a != NULL)
		{
			/* Inicializar campos. */
			(*a)[i] = tmp_dbl0;
		}
	}
	
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
									int *polygon, int id_start_poly,int *start_polygon){

	int i,j;
	
	srand(138); 
	printf("{appearance  {+edge +face linewidth 2} LIST {\n OFF\n");


	printf("%d %d 0\n", pnumber, id_start_poly);
	for(i = 0; i < pnumber; i++)
		printf("%.3f %.3f 0\n", r[2*i + 0], r[2*i + 1]);
	
	int anterior = 0;
	for(j = 0,i = 0; i< id_start_poly; i ++){
		printf("%d ", start_polygon[i] - anterior);
		for(; j <  start_polygon[i] ; j = j + 1 ){
				printf("%d ", polygon[j]);
				
		}

		printf("\n");
		anterior = start_polygon[i] ;
	}
	
	printf("} }\n");
}

/* write_classification printf("%.2f %.2f %.2f 0.75\n", ((float)rand()/(float)(RAND_MAX)), ((float)rand()/(float)(RAND_MAX)), ((float)rand()/(float)(RAND_MAX)));
 * 
 * Escribe la clasificación de los triángulos en archivos con prefijo cpath_prefix.
 * */

void write_classification(char *cpath_prefix, int *root_id, int *type, double *area,
													int tnumber, int num_nonzone_triangs,
													int num_ivoid_triangs, int num_bvoid_triangs, int num_wall_triangs)
{
	FILE *fp_nz;
	FILE *fp_iv;
	FILE *fp_bv;
	FILE *fp_w;
	FILE *fp_tmp;
	int i;
	
	int cpath_prefix_length;
	
	cpath_prefix_length = strlen(cpath_prefix);
	
	char nz_path[cpath_prefix_length + strlen(NZ_FILE_SUFFIX)];
	char iv_path[cpath_prefix_length + strlen(IV_FILE_SUFFIX)];
	char bv_path[cpath_prefix_length + strlen(BV_FILE_SUFFIX)];
	char w_path[cpath_prefix_length + strlen(W_FILE_SUFFIX)];
	
	/* Construir nombres de archivo. */
	strcpy(nz_path, cpath_prefix);
	strcpy(iv_path, cpath_prefix);
	strcpy(bv_path, cpath_prefix);
	strcpy(w_path, cpath_prefix);
	
	strcat(nz_path, NZ_FILE_SUFFIX);
	strcat(iv_path, IV_FILE_SUFFIX);
	strcat(bv_path, BV_FILE_SUFFIX);
	strcat(w_path, W_FILE_SUFFIX);
	
	/* Abrir archivos. */
	fp_nz = fopen(nz_path, "w+");
	fp_iv = fopen(iv_path, "w+");
	fp_bv = fopen(bv_path, "w+");
	fp_w = fopen(w_path, "w+");
	
	/* Escribir número de triángulos. */
	fprintf(fp_nz, "%d\n", num_nonzone_triangs);
	fprintf(fp_iv, "%d\n", num_ivoid_triangs);
	fprintf(fp_bv, "%d\n", num_bvoid_triangs);
	fprintf(fp_w, "%d\n", num_wall_triangs);

	/* Escribir datos. */
	for(i = 0; i < tnumber; i++)
	{
		/* Dependiendo del tipo, se escribirá en uno y otro archivo. */
		if(type[i] == NONZONE)
		{
			fp_tmp = fp_nz;
		}
		else if(type[i] == INNER_VOID)
		{
			fp_tmp = fp_iv;
		}
		else if(type[i] == BORDER_VOID)
		{
			fp_tmp = fp_bv;
		}
		else if(type[i] == WALL)
		{
			fp_tmp = fp_w;
		}
		else
		{
			printf("** ERROR ** write_classification: Índice de clasificación de triángulo desconocido.\n" );
			exit(EXIT_FAILURE);
		}
		fprintf(fp_tmp, "%d %d %lf\n", root_id[i], i, area[i]);
	}
	
	/* Cerrar archivos. */
	fclose(fp_nz);
	fclose(fp_iv);
	fclose(fp_bv);
	fclose(fp_w);
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
