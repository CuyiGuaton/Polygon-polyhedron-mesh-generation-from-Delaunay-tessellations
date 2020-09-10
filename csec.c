/* Programa principal para O(n) secuencial. 

Uso ej:

rbox 186 D2 z > data.dat &&  make  && ./uwu data.dat 0 0 && geomview output/data.dat.off

make && ./uwu feamy_parabolic_main_norms_t3_hsize_01 1 1 && geomview output/feamy_parabolic_main_norms_t3_hsize_01.off

argumentos:

arg[1] = Nombre archivo en input/
arg[2] = 1 si viene un .poly, 0 si son puntos aleatorios
arg[3] = 1 para imprimir triangulos  y malla poly, 0 para imprimir solo malla poly

DEBUG:
rbox 186 D2 z > data &&  make CFLAGS=-DDEBUG  && ./uwu data.dat 0 1    
*/

/*
Errores cococidos

1.- rbox 10000 D2 z > data.dat &&  make  && ./uwu data.dat 0 dat > a.off && geomview a.off


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
#include "adjgraph.h"
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

	debug_msg("RUNNING DEBUG BUILD\n");
	freopen( "output/stderr.log", "w", stderr );

	int pnumber;
	int tnumber;
	double *r;
	int *triangles;
	
	int *adj;

	double *area_poly;

	char *ppath;
	int fromfiles;
	int print_triangles;
	struct timeval t;
	
	start_time_measure(&t);
	
	read_arguments(argc, argv, &ppath, &fromfiles, &print_triangles);
	

	/* print_timestamp("Ejecutando qdelaunay...\n", t); */
	
	if(fromfiles == 1)
		read_fromfiles_data(ppath, &r, &triangles, &adj, &pnumber, &tnumber, NULL);
	else
		read_qdelaunay_data(ppath, &r, &triangles, &adj, &pnumber, &tnumber, NULL);
	

	int *max;
	int *visited;
	int *is_seed;
	int *root_id;
	int *adj_copy;
	adj_copy = (int *)malloc(3*tnumber*sizeof(int));
	max = (int *)malloc(tnumber*sizeof(int));
	is_seed = (int *)malloc(tnumber*sizeof(int));
	visited = (int *)malloc(tnumber*sizeof(int));
	root_id = (int *)malloc(tnumber*sizeof(int));
	
	area_poly = (double *)malloc(tnumber*sizeof(double));

	int i;
	int j;
	debug_print("Numero triangulos %d\n", tnumber);
	debug_msg("Inicializando arreglos adionales\n");
	/* Inicializar arreglos adicionales. */
	for(i = 0; i < tnumber; i++)
	{
		visited[i] = FALSE;
		is_seed[i] = FALSE;
		for (j = 0; j < 3; j++)
		{
			adj_copy[3*i + j] = adj[3*i + j];
		}
		
		adj_copy[i] = adj[i];
	}
	
	/* Etapa 1: Encontrar aristas máximas. */
	debug_msg("Etapa 1: Encontrar aristas máximas. \n");
	for(i = 0; i < tnumber; i++)
	{
		max[i] = max_edge_index(i, r, triangles);
	}
	
	/* Etapa 2: Desconectar arcos asociados a aristas nomáx-nomáx. */
	debug_msg("Etapa 2: Desconectar arcos asociados a aristas nomáx-nomáx. \n");
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

	/* Etapa 3: Marcar triángulos semilla. */
	debug_msg("Etapa 3: Marcar triángulos semilla.\n");
	for(i = 0; i < tnumber; i++)
	{
		for(j = 0; j < 3; j++)
		{
			int k;
			
			k = adj[3*i + j];
			
			if((k != NO_ADJ) && (k != TRIANG_BORDER) && is_max_max(i, adj[3*i + j], triangles, max) && (k < i))
			{
				is_seed[i] = TRUE;
			}
		}
	}

	
	/* Etapa 4: Generar regiones */
	debug_msg("Etapa 4: Generar regiones\n");
	int num_regs = 0;
	
	for(i = 0; i < tnumber; i++)
	{
		if(!visited[i])
		{
			/* Hacer DFS */
			adj_graph_DFS(num_regs, i, adj, visited, root_id);		
			num_regs++;
			
		}
		
	}

	debug_block(print_poly(root_id, tnumber););
	

	/* Se crean los poligonos */

	
	int *poly = (int *)malloc(tnumber*sizeof(int));
	int length_poly = 0;
	int *pos_poly = (int *)malloc(tnumber*sizeof(int));
	int id_pos_poly = 0;

	int *mesh;
	mesh = (int *)malloc(3*tnumber*sizeof(int));
	int i_mesh = 0;	
	
	int *chose_seed_triangle;
	chose_seed_triangle = (int *)malloc(num_regs*sizeof(int));
	int id_chose_seed_triangle = 0;
	int num_BE;

	for(i = 0; i < tnumber; i++){
		visited[i] = FALSE;
	}

	debug_msg("Etapa 5: Generar poligonos\n");
	for(i = 0; i < tnumber; i++)
	{
		/*busca fronter edge en un triangulo, hacer función está wea*/
		
		

		/* si tiene 2-3 froint edge y no ha sido visitado*/
		//AGREGAR LA CONDICION DE QUE SI ROOT_ID = -1 ENTONCES NO GENERA POLY, MARCAR_ID[ALGO] == -1 DESPUES DE GENERAR C/POLY!!!
		if(is_BarrierEdge(i, adj,adj_copy,root_id) && !visited[i]){

				chose_seed_triangle[id_chose_seed_triangle] = i;
				id_chose_seed_triangle++;

				debug_msg("Generando polinomio\n");
				length_poly = generate_polygon(poly, triangles, adj, r, visited, i);
				num_BE = count_BarrierEdges(poly, length_poly);
				
				
				//save_to_mesh(mesh, poly, &i_mesh, length_poly, pos_poly, &id_pos_poly);	
				
				debug_msg("Poly: "); debug_block(print_poly(poly, length_poly););
				if( num_BE > 0){
					debug_print("Se dectecto %d BE\n", num_BE);
					remove_BarrierEdge(poly, length_poly, num_BE, triangles, adj, r, tnumber, mesh, &i_mesh, pos_poly, &id_pos_poly, visited);
				}else{
					debug_msg("Guardando poly\n");
					save_to_mesh(mesh, poly, &i_mesh, length_poly, pos_poly, &id_pos_poly);	
				}
				
				
			}
	}
	
	
	for (i = 0; i < tnumber; i++) 
	{
		if(visited[i] == FALSE){
			fprintf(stderr,"ERROR hmnito mira, el triangulo %d no se visito ", i);
		}
	}
	
	
	write_geomview(r,triangles, pnumber, tnumber,i_mesh, mesh, id_pos_poly, pos_poly, print_triangles, ppath, chose_seed_triangle, id_chose_seed_triangle);


	free(r);
	free(triangles);
	free(adj);
	free(max );
	free(visited );
	free(chose_seed_triangle);
	free(root_id);
	free(adj_copy);
	free(area_poly);
	free(mesh );
	free(poly);
	free(pos_poly);
	/*print_timestamp("Fin.\n", t);*/
	return EXIT_SUCCESS;
}
