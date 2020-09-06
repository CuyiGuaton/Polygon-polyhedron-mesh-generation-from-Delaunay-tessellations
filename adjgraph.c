/* Funciones para examinar grafos de adyacencia. */

#include <stdlib.h>
#include <stdio.h>
#include "consts.h"



/* adj_graph_DFS
 * 
 * Propaga el identificador i de triángulo raíz hacia toda la componente conexa.
 * Aprovecha las visitas para averiguar:
 * - Si hay un triángulo semilla.
 * - Si hay un triángulo que toque el borde de la triangulación.
 * - Área total de los triángulos en la componente conexa examinada.
 * */

void adj_graph_DFS(int i, int j, int *adj, int *visited, int *root_id)
{
	int t0;
	int t1;
	int t2;
	
	
	root_id[j] = i;
	
	visited[j] = TRUE;
	
	t0 = adj[3*j + 0];
	t1 = adj[3*j + 1];
	t2 = adj[3*j + 2];
	
	/*printf("#%d-%d, %d %d %d \n",i, j, t0, t1, t2);*/

	if((t0 != NO_ADJ) && (t0 != TRIANG_BORDER) && !visited[t0])
	{
		
		adj_graph_DFS(i, t0, adj,  visited, root_id);
	}

	if((t1 != NO_ADJ) && (t1 != TRIANG_BORDER) && !visited[t1])
	{
		
		adj_graph_DFS(i, t1, adj,  visited, root_id);
	}


	if((t2 != NO_ADJ) && (t2 != TRIANG_BORDER) && !visited[t2])
	{
		adj_graph_DFS(i, t2, adj,  visited, root_id);
	}
	
}
