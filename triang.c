/* Funciones para manejar triangulación. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "consts.h"



/* dist
 * 
 * Retorna la distancia que hay entre el punto (x0,y0)
 * y (x1,y1).
 * */

double dist(double x0, double y0, double x1, double y1)
{
	return sqrt(pow(x0 - x1, 2.0) + pow(y0 - y1, 2.0));
}



/* max_edge_index
 * 
 * Retorna el índice k de la arista máxima de un triángulo i, 
 * descrito por los puntos p0p1p2. Será 0 si p0p1 es máxima.
 * Será 1 si p1p2 lo es. Será 2 si p2p0 lo es.
 * */

int max_edge_index(int i, double *r, int *p)
{
	double l0;
	double l1;
	double l2;
	
	int p0;
	int p1;
	int p2;
	
	p0 = p[3*i + 0];
	p1 = p[3*i + 1];
	p2 = p[3*i + 2];
	
	l0 = dist(r[2*p0 + 0], r[2*p0 + 1], r[2*p1 + 0], r[2*p1 + 1]);
	l1 = dist(r[2*p1 + 0], r[2*p1 + 1], r[2*p2 + 0], r[2*p2 + 1]);
	l2 = dist(r[2*p2 + 0], r[2*p2 + 1], r[2*p0 + 0], r[2*p0 + 1]);
/*	
	if((l0 >= l1 && l1 >= l2) || (l0 >= l2 && l2 >= l1))
	{
		return 0;
	}
	else if((l1 >= l0 && l0 >= l2) || (l1 >= l2 && l2 >= l0))
	{
		return 1;
	}
	else
	{
		return 2;
	}
	*/

	
	if((l2 >= l0 && l0 >= l1) || (l2 >= l1 && l1 >= l0))
	{
		return 2;
	}
	else if((l0 >= l1 && l1 >= l2) || (l0 >= l2 && l2 >= l1))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



/* same_edge
 * 
 * Indica para las aristas {u,v} y {w,x} si son iguales o no.
 * */
 
int same_edge(int u, int v, int w, int x)
{
	return (u == w && v == x) || (u == x && v == w);
}



/* get_edge_index
 * 
 * Entrega el índice de la arista {u,v} respecto del triángulo i.
 * */

static int get_edge_index(int u, int v, int i, int *p)
{
	int p0;
	int p1;
	int p2;
	
	p0 = p[3*i + 0];
	p1 = p[3*i + 1];
	p2 = p[3*i + 2];
	
	if(same_edge(u, v, p0, p1))
	{
		return 0;
	}
	else if(same_edge(u, v, p1, p2))
	{
		return 1;
	}
	else if(same_edge(u, v, p2, p0))
	{
		return 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** get_edge_index: Arista {%d,%d} no pertenece al triángulo %d.\n", u, v, i);
		exit(EXIT_FAILURE);
	}
}



/* is_nomax_nomax
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es nomáx-nomáx.
 * */

int is_nomax_nomax(int i, int j, int *p, int *max)
{
	int p0i;
	int p1i;
	int p2i;
	int p0j;
	int p1j;
	int p2j;
	
	p0i = p[3*i + 0];
	p1i = p[3*i + 1];
	p2i = p[3*i + 2];
	
	p0j = p[3*j + 0];
	p1j = p[3*j + 1];
	p2j = p[3*j + 2];
	
	int ij;
	int ii;
	
	if(same_edge(p0i, p1i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** is_nomax_nomax: Problema insperado para triángulos %d y %d.\n", i, j);
		exit(EXIT_FAILURE);
	}
	
	return (ij != max[j]) && (ii != max[i]);
}



/* is_max_max
 * 
 * Indica si la arista compartida entre los triángulos i y j
 * es máx-máx.
 * */

int is_max_max(int i, int j, int *p, int *max)
{
	int p0i;
	int p1i;
	int p2i;
	
	int p0j;
	int p1j;
	int p2j;
	
	p0i = p[3*i + 0];
	p1i = p[3*i + 1];
	p2i = p[3*i + 2];
	
	p0j = p[3*j + 0];
	p1j = p[3*j + 1];
	p2j = p[3*j + 2];
	
	int ij;
	int ii;
	
	if(same_edge(p0i, p1i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p0j, p1j))
	{
		ij = get_edge_index(p0j, p1j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p1j, p2j))
	{
		ij = get_edge_index(p1j, p2j, j, p);
		ii = 2;
	}
	else if(same_edge(p0i, p1i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 0;
	}
	else if(same_edge(p1i, p2i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 1;
	}
	else if(same_edge(p2i, p0i, p2j, p0j))
	{
		ij = get_edge_index(p2j, p0j, j, p);
		ii = 2;
	}
	else
	{
		fprintf(stderr, "** ERROR ** is_max_max: Problema insperado para triángulos %d y %d.\n", i, j);
		exit(EXIT_FAILURE);
	}
	
	return (ij == max[j]) && (ii == max[i]);
}



/* edge_belongs_to
 * 
 * Indica si arista {k,l} pertenece al triángulo i.
 * */

static int edge_belongs_to(int k, int l, int i, int *p)
{
	return same_edge(k, l, p[3*i + 0], p[3*i + 1])
					|| same_edge(k, l, p[3*i + 1], p[3*i + 2])
					|| same_edge(k, l, p[3*i + 2], p[3*i + 0]);
}



/* get_adjacent_triangle
 * 
 * Retorna el identificador del triángulo que es adyacente al
 * triángulo i, mediante la arista {k,l}.
 * 
 * Si no hay triángulo, retorna NO_ADY (aún si es porque {k,l}
 * es de borde de triangulación).
 * */

int get_adjacent_triangle(int i, int k, int l, int *p, int *adj)
{
	int u;
	int v;
	int w;
	
	/* Comprobar que {k,l} pertenezca al triángulo i. */
	if(!edge_belongs_to(k, l, i, p))
	{
		fprintf(stderr, "** ERROR ** get_adjacent_triangle: Arista {%d,%d} no pertenece al triángulo %d.\n", k, l, i);
		exit(EXIT_FAILURE);
	}
	
	u = adj[3*i + 0];
	v = adj[3*i + 1];
	w = adj[3*i + 2];
	
	int index;
	
	if((u != NO_ADJ)  && (same_edge(k, l, p[3*u + 0], p[3*u + 1]) ||
			same_edge(k, l, p[3*u + 1], p[3*u + 2]) || same_edge(k, l, p[3*u + 2], p[3*u + 0])))
	{
		index = u;
	}
	else if((v != NO_ADJ)  && (same_edge(k, l, p[3*v + 0], p[3*v + 1]) ||
					same_edge(k, l, p[3*v + 1], p[3*v + 2]) || same_edge(k, l, p[3*v + 2], p[3*v + 0])))
	{
		index = v;
	}
	else if((w != NO_ADJ) && (same_edge(k, l, p[3*w + 0], p[3*w + 1]) ||
					same_edge(k, l, p[3*w + 1], p[3*w + 2]) || same_edge(k, l, p[3*w + 2], p[3*w + 0])))
	{
		index = w;
	}
	else
	{
		/* Ningún triángulo apareció como adyacente a {k,l} desde el i. */
		index = NO_ADJ;
	}
	
	return index;
}

int is_continuous(int i, int endpoint, int *p ){
	int p0 = p[3*i + 0];
	int p1 = p[3*i + 1];
	int p2 = p[3*i + 2];
	int end = endpoint;

	if(end == p0){
		return  0; /* indica que está en p0*/
	}else if (end == p1){
		return  1;  /* indica que está en p1*/
	}else if(end == p2){
		return 2;  /* indica que está en p2*/
	}
	return -1;
}


/* 
	Busca un triangulo adjacente que comparte el mismo endpoint.
	Origen es el triangulo de donde se viene, -1 si se quiere que se pueda devolver a triangulo anterior.
*/
int get_adjacent_triangle_share_endpoint(int i, int origen, int endpoint, int *p, int *adj){
	int p0 = p[3*i + 0];
	int p1 = p[3*i + 1];
	int p2 = p[3*i + 2];
	
	/* consigue los triangulos adyacentes */
	int i0 = get_adjacent_triangle(i, p0, p1, p, adj);
	int i1 = get_adjacent_triangle(i, p1, p2, p, adj);
	int i2 = get_adjacent_triangle(i, p2, p0, p, adj);

	/*verifica si los triangulos son continuos al endpoint */
	int ic0 = is_continuous(i0 ,endpoint, p);
	int ic1 = is_continuous(i1 ,endpoint, p);
	int ic2 = is_continuous(i2 ,endpoint, p);
	
	/*fprintf(stderr,"FUNCTION i0 ic0 %d %d   || i1 ic1 %d %d || i2 ic2 %d %d  \n", i0, ic0, i1,ic1,  i2,ic2);*/
	if(ic0 != -1 &&  i0 != origen && i0 != -1){ /*Si hay contuinidad y no retrocede al origen */
		return i0;
	}else if(ic1 != -1 && i1 != origen  && i1 != -1){
		return i1;
	}else if(ic2 != -1 &&   i2 != origen  && i2 != -1){
		return i2;
	}
	return -2;
}


int count_FrontierEdges(int triangle, int *adj){
    int adj_counter = 0;
    int j;
    for(j = 0; j < 3; j++){ 
        if(adj[3*triangle + j] == NO_ADJ){
            adj_counter++;
        }
    }
    return adj_counter;
}


int search_triangle_by_vertex_with_FrontierEdge(int v, int *triangles, int *adj, int tnumber){
	int i,j;
	for (i = 0; i < tnumber; i++)
		for (j = 0; j < 3; j++)
			if(triangles[3*i +j] == v  && ( adj[3*i + ((j + 1)%3)] == -1 || adj[3*i + ((j + 2)%3)] == -1 )){
				/*printf("\n%d | Triangles %d %d %d | ADJ  %d %d %d\n", i, triangles[3*i + 0], triangles[3*i + 1], triangles[3*i + 2], adj[3*i + 0], adj[3*i + 1], adj[3*i + 2] ); */
				return i;
			}
				
	return -1;
}

int search_prev_vertex_to_split(int i, int v, int origen, int *triangles, int *adj){
	int t0, t1,t2;
	int a0, a1, a2;

	t0 = triangles[3*i + 0];
	t1 = triangles[3*i + 1];
	t2 = triangles[3*i + 2];

	a0 = adj[3*i + 0];
	a1 = adj[3*i + 1];
	a2 = adj[3*i + 2];

	if(t1 == v && origen == a0)
			return t2;
	else  if(t2 == v && origen == a0)
			return t1;
	else  if(t0 == v && origen == a1)
			return t2;
	else  if(t2 == v && origen == a1)
			return t0;
	else  if(t0 == v && origen == a2)
			return t1;
	else  if(t1 == v && origen == a2)
			return t0;	
	return -1;
}

int search_next_vertex_to_split(int i, int v, int origen, int *triangles, int *adj){
	int t0, t1,t2;
	int a0, a1, a2;

	t0 = triangles[3*i + 0];
	t1 = triangles[3*i + 1];
	t2 = triangles[3*i + 2];

	a0 = adj[3*i + 0];
	a1 = adj[3*i + 1];
	a2 = adj[3*i + 2];

	/*printf("origen %d, actual %d \n", origen,i);*/
	if(a0 != NO_ADJ && t1 == v && origen != a0)
			return t2;
	else  if(a0 != NO_ADJ && t2 == v && origen != a0){
			return t1;		
	}
	else  if(a1 != NO_ADJ && t0 == v && origen != a1)
			return t2;
	else  if(a1 != NO_ADJ && t2 == v && origen != a1)
			return t0;
	else  if(a2 != NO_ADJ && t0 == v && origen != a2)
			return t1;
	else  if(a2 != NO_ADJ && t1 == v && origen != a2)
			return t0;

	return -1;
}

/*
7 | Triangles 34 19 49 | ADJ  10 8 -1

8 | Triangles 34 97 49 | ADJ  70 7 -1

10 | Triangles 84 19 49 | ADJ  7 33 11

32 | Triangles 32 88 49 | ADJ  71 -1 -1

33 | Triangles 32 84 49 | ADJ  10 -1 -1

70 | Triangles 82 97 49 | ADJ  8 71 -1

71 | Triangles 82 88 49 | ADJ  32 70 -1


*/