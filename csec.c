/*falta:

- optimizar memoria
- generar funciones y ponerlas en otro archivo
- optimizar  eliminación de barrier edges
*/

/* Programa principal para O(n) secuencial. 

Conjunto de puntos aleatorios:

rbox 100 D2 z > data.dat && make && ./uwu data.dat 0 dat

$ rbox 1000 D2 z > data.dat && make && ./uwu data.dat 0 dat > a.off && geomview a.off

Guitarra:

make && ./uwu guitar 1 dat > a.off && geomview a.off 

SE HA CAMBIADO EL INPUT, qdelaunay cambia de i a Fv

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "io.h"
#include "args.h"
#include "consts.h"
#include "adjgraph.h"
#include "timestamp.h"
#include "triang.h"
#include "polygon.h"
#include <time.h>




int main(int argc, char **argv)
{
	int pnumber;
	int tnumber;
	double *r;
	int *triangles;
	int *adj;

	double *area;
	double *area_poly;

	char *ppath;
	double threshold;
	char *cpath_prefix;
	struct timeval t;
	
	start_time_measure(&t);
	
	read_arguments(argc, argv, &ppath, &threshold, &cpath_prefix);
	

	/* print_timestamp("Ejecutando qdelaunay...\n", t); */
	
	if(threshold == 1)
		read_fromfiles_data(ppath, &r, &triangles, &adj, &area, &pnumber, &tnumber, NULL);
	else
		read_qdelaunay_data(ppath, &r, &triangles, &adj, &area, &pnumber, &tnumber, NULL);
	

	int *max;
	int *visited;
	max = (int *)malloc(tnumber*sizeof(int));
	visited = (int *)malloc(tnumber*sizeof(int));
	
	area = (double *)malloc(tnumber*sizeof(double));
	area_poly = (double *)malloc(tnumber*sizeof(double));
	
	

	int i;
	int j;

	double Ax, Ay, Bx, By, Cx, Cy;
	/* Inicializar arreglos adicionales. */
	for(i = 0; i < tnumber; i++)
	{
		visited[i] = FALSE;
		
		/*Calcular el area */
		area_poly[i] = 0;
		Ax = r[triangles[3*i + 0]*2 + 0];
		Ay = r[triangles[3*i + 0]*2 + 1];
		Bx = r[triangles[3*i + 1]*2 + 0];
		By = r[triangles[3*i + 1]*2 + 1];
		Cx = r[triangles[3*i + 2]*2 + 0];
		Cy = r[triangles[3*i + 2]*2 + 1];
		area[i] = abs((Ax*(By - Cy) + Bx*(Cy - Ay) + Cx*(Ay - By))/2);
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
			if(adj[3*i +j] == NO_ADJ || is_nomax_nomax(i, adj[3*i + j], triangles, max))
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

	int *poly1 = (int *)malloc(tnumber*sizeof(int));
	int *poly2 = (int *)malloc(tnumber*sizeof(int));
	int length_poly = 0;
	int length_poly1 = 0;
	int length_poly2 = 0;
	for(i = 0; i < tnumber; i++)
	{
		/*busca fronter edge en un triangulo, hacer función está wea*/
		num_fe = count_FrontierEdges(i, adj);

		/* si tiene 2-3 froint edge y no ha sido visitado*/
		if(num_fe >= 2 && !visited[i]){ 

			length_poly = generate_polygon(poly, triangles, adj, r, visited, i, num_fe);
			
			int num_BE = count_BarrierEdges(poly, length_poly);
			if( num_BE > 0){
				
				double A_poly = get_area_poly(poly, length_poly,r);
				double opt = A_poly/(num_BE+1);

				printf("A: %.2lf, opt = %.2lf, numBE = %d\n", A_poly, opt, num_BE);
				print_poly(poly, length_poly);
				int v_be = get_vertex_BarrierEdge(poly, length_poly);
				int t = search_triangle_by_vertex_with_FrontierEdge(v_be, triangles, adj, tnumber);
				int v_other = search_next_vertex_to_split(t, v_be, -2, triangles, adj);
				printf("%d-%d ",v_be, v_other);
				printf("%d\n",t);

				split_poly(poly, length_poly, poly1, &length_poly1, poly2, &length_poly2, v_be, v_other);
				print_poly(poly1, length_poly1);
				print_poly(poly2, length_poly2);

				double A1 =get_area_poly(poly1, length_poly1,r);
				double A2 = get_area_poly(poly2, length_poly2,r);
				
				double r_prev = fabs(fmin(A1, A2) - opt);
				double r_act = 0.0;
				printf("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
				
				int aux, origen = t;
				printf("\n");
				while (1){
					aux = t;
					t = get_adjacent_triangle_share_endpoint(t, origen, v_be, triangles, adj);
					origen = aux;
					v_other = search_next_vertex_to_split(t, v_be, origen, triangles, adj);
					

					printf("%d-%d ",v_be, v_other);
					printf(" | nuevo t = %d | origen = %d\n",t, origen);

					split_poly(poly, length_poly, poly1, &length_poly1, poly2, &length_poly2, v_be, v_other);
					print_poly(poly1, length_poly1);
					print_poly(poly2, length_poly2);
					A1 =get_area_poly(poly1, length_poly1,r);
					A2 = get_area_poly(poly2, length_poly2,r);
					
					r_act = fabs(fmin(A1, A2) - opt);
					printf("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
					if (r_act <= r_prev)
						r_prev = r_act;
					else{
						printf("SALI!!!!\n");
						v_other = search_prev_vertex_to_split(t, v_be, origen, triangles, adj);
						split_poly(poly, length_poly, poly1, &length_poly1, poly2, &length_poly2, v_be, v_other);
						break;
					}
					printf("\n\n");
				}
				printf("%d-%d ",v_be, v_other);
				printf("%d\n",t);
				print_poly(poly1, length_poly1);
				print_poly(poly2, length_poly2);
				A1 =get_area_poly(poly1, length_poly1,r);
				A2 = get_area_poly(poly2, length_poly2,r);
				printf("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
				return 0;
			}

			save_to_mesh(mesh, poly, &i_mesh, length_poly, pos_poly, &id_pos_poly);
			
		}

	}
	
	for (i = 0; i < tnumber; i++) 
	{
		if(visited[i] == FALSE){
			printf("hmnito mira, el triangulo %d no se visito ", i);
			return 0;
		}
	}
	
	/*
	write_geomview(r,triangles, pnumber, tnumber,i_mesh, mesh, id_pos_poly, pos_poly);
*/

	free(r);
	free(triangles);
	free(adj);
	free(max );
	free(visited );
	free(area);
	free(area_poly);
	free(mesh );
	free(poly);
	free(pos_poly);
	/*print_timestamp("Fin.\n", t);*/
	return EXIT_SUCCESS;
}
