/* Funciones para manejar poligonos. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "consts.h"
#include "triang.h"

/* guarda un poly en la malla
    retorna el nuevo indice de la malla */
void save_to_mesh(int *mesh, int *poly, int *i_mesh, int length_poly, int *pos_poly, int *id_pos_poly){
    int i;
    for (i = 0; i < length_poly; i++){
        
        mesh[*i_mesh + i] = poly[i];
    }
    *i_mesh += length_poly;
    pos_poly[*id_pos_poly] = *i_mesh;
	*id_pos_poly = *id_pos_poly + 1;
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

int count_BarrierEdges(int *poly, int length_poly){
    int count = 0;
    int x, y,i;
    for (i = 0; i < length_poly + 2; i++)
    {
        x = i % length_poly;
        y = (i+2) % length_poly;
        if (poly[x] == poly[y])
            count++;
    }
    return count;
}

int get_vertex_BarrierEdge(int *poly, int length_poly){
    int x, y,i;
    for (i = 0; i < length_poly + 2; i++)
    {
        x = i % length_poly;
        y = (i+2) % length_poly;
        if (poly[x] == poly[y])
            return (i+1) %length_poly;
    }
    return -1;
}

int get_area_poly(int *poly, int length_poly, int *r){
    int area = 0;
    int i;
    int x1,y1,x2,y2;
    for (i = 0; i < length_poly-1; i++)
    {
        x1=r[2*poly[i] + 0];
        y1=r[2*poly[i] + 1];
        x2=r[2*poly[i+1] + 0];
        y2=r[2*poly[i+1] + 1];
        area += (x1*y2 - y1*x2);
    }
    return abs(area/2);
}

void split_poly(int *original_poly, int length_poly, int *poly1, int *length_poly1, int *poly2, int *length_poly2, int e1, int e2){
    int pos1, pos2,i;
    for(i =0; i< length_poly; i++){
        if(original_poly[i] == e1)
            pos1 = i;
        if(original_poly[i] == e2)
            pos2 = i;
    }
    *length_poly1 = abs(pos1-pos2);
    *length_poly2 = length_poly - *length_poly1;
    for (i = 0; i < *length_poly1; i++)
        poly1[i] = original_poly[(pos1 + i) %length_poly];
    for (i = 0; i < *length_poly2; i++)
        poly2[i] = original_poly[(pos2 + i) %length_poly];
}




int generate_polygon(int * poly, int * triangles, int * adj, double *r, int * visited, int i, int adj_counter) {
    int ind_poly = 0;
	
	int initial_point = 0;
	int end_point = 0;
	
	int t0;
	int t1;	
	int t2;
	int continuous;
	int k, j, aux;
	int origen;
    /*si tiene 3 se agregan y se corta el ciclo*/
    if (adj_counter == 3) {
        poly[ind_poly] = triangles[3 * i + 0];
        ind_poly++;
        poly[ind_poly] = triangles[3 * i + 1];
        ind_poly++;
        poly[ind_poly] = triangles[3 * i + 2];
        ind_poly++;

        visited[i] = TRUE;

    } else {
        /*si tiene dos FE se agregan y se empieza el ciclo*/
        t0 = adj[3 * i + 0];
        t1 = adj[3 * i + 1];
        t2 = adj[3 * i + 2];
        /*fprintf(stderr,"\n \n \ni %d t %d %d %d \n", i, t0, t1, t2);*/
        if (t0 == NO_ADJ && t1 == NO_ADJ) {

            poly[ind_poly] = triangles[3 * i + 1];
            ind_poly++;
            poly[ind_poly] = triangles[3 * i + 2];
            ind_poly++;

            initial_point = 3 * i + 1;
            end_point = 3 * i + 0;

        } else if (t2 == NO_ADJ && t0 == NO_ADJ) {

            poly[ind_poly] = triangles[3 * i + 0];
            ind_poly++;
            poly[ind_poly] = triangles[3 * i + 1];
            ind_poly++;

            initial_point = 3 * i + 0;
            end_point = 3 * i + 2;

        } else if (t1 == NO_ADJ && t2 == NO_ADJ) {
            poly[ind_poly] = triangles[3 * i + 2];
            ind_poly++;
            poly[ind_poly] = triangles[3 * i + 0];
            ind_poly++;

            end_point = 3 * i + 1;
            initial_point = 3 * i + 2;
        } else {
            fprintf(stderr, "** ERROR ** Choosing initial point and endpoint\n");
        }

        /*se marca como visitado */
        
        visited[i] = TRUE;
        adj_counter = 0;
        k = i;
        aux = k;
        k = get_adjacent_triangle_share_endpoint(k, k, end_point, triangles, adj); /* cambia el indice */
        continuous = is_continuous(k, end_point, triangles);
        origen = aux;
        /*fprintf(stderr,"k %d origen %d, conti %d\n", k, origen, continuous);*/
        while (triangles[initial_point] != triangles[end_point]) {

            /*se marca el triangulo visto como visitado y se suma al area del poligono */
            
            visited[k] = TRUE;
            t0 = adj[3 * k + 0];
            t1 = adj[3 * k + 1];
            t2 = adj[3 * k + 2];

            adj_counter = 0;
            for (j = 0; j < 3; j++) {
                /*cuenta los fronter edge*/
                if (adj[3 * k + j] == NO_ADJ) {
                    adj_counter++;
                }
            }

            if (adj_counter == 2 && continuous != -1) {
                /* ///////////////////si tiene 2 frontier edge se agregan a poly //////////////////////////////////// */

                if (t0 == NO_ADJ && t1 == NO_ADJ) {
                    /*si endpoint es continua a t0  y t0-t1 son fe*/
                    if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = 3 * k + 0;

                    } else if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = 3 * k + 1;
                    }
                } else if (t2 == NO_ADJ && t0 == NO_ADJ) {
                    /*si endpoint es continua a t2  y t2-t0 son fe*/
                    if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = 3 * k + 2;

                    } else if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = 3 * k + 0;

                    }
                } else if (t1 == NO_ADJ && t2 == NO_ADJ) {
                    /*si endpoint es continua a t1 y t1-t2 son fe*/
                    if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = 3 * k + 1;

                    } else if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = 3 * k + 2;

                    }
                } else {
                    fprintf(stderr, "** ERROR ** Adding 2 fronter edges\n");
                    fprintf(stderr, "** ERROR ** k %d t %d %d %d ini %d end %d \n", k, t0, t1, t2, initial_point, end_point);
                }

                aux = k;
                k = get_adjacent_triangle_share_endpoint(k, -1, end_point, triangles, adj); /* se le permite volver al triangulo anterior */
                continuous = is_continuous(k, end_point, triangles);
                origen = aux;

            } else if (adj_counter == 1 && continuous != -1) {
                /* ///////////////////si solo se tiene 1 frontier edge //////////////////////////////////// */
                if (t0 == NO_ADJ) {
                    /*si endpoint es continua a t0  y t0 es fe*/
                    if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = 3 * k + 2;

                    } else if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = 3 * k + 1;

                    }
                } else if (t2 == NO_ADJ) {
                    /*si endpoint es continua a t2  y t2 es fe*/
                    if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = 3 * k + 1;

                    } else if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = 3 * k + 0;

                    }
                } else if (t1 == NO_ADJ) {
                    /*si endpoint es continua a t1  y t1 es fe*/
                    if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = 3 * k + 0;

                    } else if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = 3 * k + 2;

                    }
                } else {
                    fprintf(stderr, "** ERROR ** Adding 1 fronter edges\n");
                    fprintf(stderr, "** ERROR ** k %d t %d %d %d ini %d end %d conti %d\n", k, t0, t1, t2, initial_point, end_point, continuous);
                }
                /*si es continuo y tiene 1 fe no puede volver, ind si se guarda  o no*/
                aux = k;
                k = get_adjacent_triangle_share_endpoint(k, origen, end_point, triangles, adj); /* cambia el indice */
                continuous = is_continuous(k, end_point, triangles);
                origen = aux;
            } else {
                /*si no es continuo no puede regresar de donde venía*/
                aux = k;
                k = get_adjacent_triangle_share_endpoint(k, origen, end_point, triangles, adj); /* cambia el indice */
                continuous = is_continuous(k, end_point, triangles);
                origen = aux;
            }

        }
    }
    return ind_poly;
}