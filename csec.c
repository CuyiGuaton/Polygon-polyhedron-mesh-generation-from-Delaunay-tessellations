/* Programa principal para O(n) secuencial. 

Conjunto de puntos aleatorios:

rbox 100 D2 z > data.dat && make && ./uwu data.dat 0 dat

$ rbox 1000 D2 z > data.dat && make && ./uwu data.dat 0 dat > a.off && geomview a.off

Guitarra:

make && ./uwu guitar 1 dat > a.off && geomview a.off 

SE HA CAMBIADO EL INPUT, qdelaunay cambia de i a Fv

DEBUG:
rbox 100 D2 z > data.dat && make CFLAGS=-DDEBUG  && ./uwu data.dat 0 dat
*/

/*
Errores cococidos

1.- rbox 16987 D2 z > data.dat &&  make CFLAGS=-DDEBUG  && ./uwu data.dat 0 dat clear  

Tiene un BE que es otro poly

3.- rbox 15486 D2 z > data.dat &&  make CFLAGS=-DDEBUG  && ./uwu data.dat 0 dat


Error critico, se genera un poligono inicial con 4 edges de area 0 (4) 10066 13263 7810 13263, se detectan 3 BEg
Causa, El poligono inicia en el inicio de un Barrier edge, da una vuelta en u sobre este y cree que se termino de formar el poligono
*/



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "io.h"
#include "args.h"
#include "consts.h"
#include "timestamp.h"
#include "triang.h"
#include "polygon.h"
#include <time.h>


#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define debug_block(fmt) do { if (DEBUG_TEST){ fmt }} while (0)
#define debug_print(fmt, ...) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#define debug_msg(fmt) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__,  __LINE__, __func__); } while (0)

int main(int argc, char **argv)
{

	debug_msg("RUNNING DEBUG BUILD\n\n\n");

	int pnumber;
	int tnumber;
	double *r;
	int *triangles;
	int *adj;

	double *area_poly;

	char *ppath;
	double threshold;
	char *cpath_prefix;
	struct timeval t;
	
	start_time_measure(&t);
	
	read_arguments(argc, argv, &ppath, &threshold, &cpath_prefix);
	

	/* print_timestamp("Ejecutando qdelaunay...\n", t); */
	
	if(threshold == 1)
		read_fromfiles_data(ppath, &r, &triangles, &adj, &pnumber, &tnumber, NULL);
	else
		read_qdelaunay_data(ppath, &r, &triangles, &adj, &pnumber, &tnumber, NULL);
	

	int *max;
	int *visited;
	max = (int *)malloc(tnumber*sizeof(int));
	visited = (int *)malloc(tnumber*sizeof(int));
	
	
	area_poly = (double *)malloc(tnumber*sizeof(double));

	int i;
	int j;

	
	/* Inicializar arreglos adicionales. */
	for(i = 0; i < tnumber; i++)
	{
		visited[i] = FALSE;
		
	
	}
	
	/* Etapa 1: Encontrar aristas máximas. */
	for(i = 0; i < tnumber; i++)
	{
		max[i] = max_edge_index(i, r, triangles);
	}
	
	/* Etapa 2: Desconectar arcos asociados a aristas nomáx-nomáx. */
	for(i = 0; i < tnumber; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(adj[3*i +j] == TRIANG_BORDER || is_nomax_nomax(i, adj[3*i + j], triangles, max))
			{
				adj[3*i + j] = NO_ADJ;
			}
			
		}
	}


	
	/* Se crean los poligonos */
	
	
	int i_mesh = 0;
	int num_fe;
	int *poly = (int *)malloc(tnumber*sizeof(int));

	int *pos_poly = (int *)malloc(tnumber*sizeof(int));
	int id_pos_poly = 0;

	int *mesh;
	mesh = (int *)malloc(3*tnumber*sizeof(int));

	int length_poly = 0;
	int num_BE;

	for(i = 0; i < tnumber; i++)
	{
		/*busca fronter edge en un triangulo, hacer función está wea*/
		num_fe = count_FrontierEdges(i, adj);

		/* si tiene 2-3 froint edge y no ha sido visitado*/
		if(num_fe > 0 && !visited[i]){ 

			debug_msg("Generando polinomio\n");
			length_poly = generate_polygon(poly, triangles, adj, r, visited, i, num_fe);
			num_BE = count_BarrierEdges(poly, length_poly);
			
			
			//save_to_mesh(mesh, poly, &i_mesh, length_poly, pos_poly, &id_pos_poly);	
			
			debug_msg("Poly: "); debug_block(print_poly(poly, length_poly); printf("\n"););
			if( num_BE > 0){
				debug_print("Se dectecto %d BE\n", num_BE);
				remove_BarrierEdge(poly, length_poly, num_BE, triangles, adj, r, tnumber, mesh, &i_mesh, pos_poly, &id_pos_poly);
			}else{
				debug_msg("Guardando poly\n");
				save_to_mesh(mesh, poly, &i_mesh, length_poly, pos_poly, &id_pos_poly);	
			}
			
			
		}
	}
	
	for (i = 0; i < tnumber; i++) 
	{
		if(visited[i] == FALSE){
			printf("hmnito mira, el triangulo %d no se visito ", i);
			return 0;
		}
	}
	
	
	write_geomview(r,triangles, pnumber, tnumber,i_mesh, mesh, id_pos_poly, pos_poly);


	free(r);
	free(triangles);
	free(adj);
	free(max );
	free(visited );

	free(area_poly);
	free(mesh );
	free(poly);
	free(pos_poly);
	/*print_timestamp("Fin.\n", t);*/
	return EXIT_SUCCESS;
}
