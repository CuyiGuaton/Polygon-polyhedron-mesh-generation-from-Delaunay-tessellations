/* Funciones para manejar poligonos. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "consts.h"
#include "triang.h"
#include "polygon.h"

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define debug_block(fmt) do { if (DEBUG_TEST){ fmt }} while (0)
#define debug_print(fmt, ...) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#define debug_msg(fmt) do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__,  __LINE__, __func__); } while (0)


/* Dado un poly con barrier edges
Optimiza la división de este y devuelve poly1 y poly2*/
void remove_BarrierEdge_from_polygon(int *poly, int length_poly, int *poly1, int *length_poly1, int *poly2, int *length_poly2, int num_BE, int *triangles, int *adj, double *r, int tnumber){
    double A_poly, A1, A2, opt, r_prev, r_act;
    int v_be, t, v_other, aux, origen;

    /*se calculca el valor optimo para el poligono */
    A_poly = get_area_poly(poly, length_poly,r);
    opt = A_poly/(num_BE+1);

    debug_print("Area poly: %.2lf, opt = %.2lf\n", A_poly, opt);

    /* se calcula el vertice a insertar en el polygono */
    v_be = get_vertex_BarrierEdge(poly, length_poly);
    t = search_triangle_by_vertex_with_FrontierEdge(v_be, triangles, adj, tnumber);
    v_other = search_next_vertex_to_split(t, v_be, -2, triangles, adj);

    debug_print("Agregar edge %d - %d del Triangulo %d \n", v_be, v_other,t);
    
    if(v_other == -1 || v_other == -2){
        printf("No se puede agregar el edge %d - %d del Triangulo %d\n", v_be, v_other, t);
        exit(0);
    }

    debug_msg("Dividiendo poligono\n");
    /* Se divide el polygono en dos */
    split_poly(poly, length_poly, poly1, &(*length_poly1), poly2, &(*length_poly2), v_be, v_other);

    debug_msg("poly1: "); debug_block(print_poly(poly1, *length_poly1); printf("\n"););
	debug_msg("poly2: "); debug_block( print_poly(poly2, *length_poly2); printf("\n"););


    A1 =get_area_poly(poly1, *length_poly1,r);
    A2 = get_area_poly(poly2, *length_poly2,r);

    /* se calcula el r */
    r_prev = fabs(fmin(A1, A2) - opt);
    r_act = 0.0;

    debug_print("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
    origen = t;
    while (1){
        
        aux = t;
        t = get_adjacent_triangle_share_endpoint(t, origen, v_be, triangles, adj);
        origen = aux;
        v_other = search_next_vertex_to_split(t, v_be, origen, triangles, adj);
        

        debug_print("Agregar edge %d - %d del nuevo triangulo %d | origen = %d \n", v_be, v_other,t, origen); 
        
        if(v_other == -1 || v_other == -2){
            printf("No se puede agregar el edge %d - %d del Triangulo %d\n", v_be, v_other, t);
            exit(0);
        }

        debug_msg("Dividiendo poligono de nuevo\n");

        split_poly(poly, length_poly, poly1, &(*length_poly1), poly2, &(*length_poly2), v_be, v_other);
        A1 =get_area_poly(poly1, *length_poly1,r);
        A2 = get_area_poly(poly2,*length_poly2,r);
        debug_msg("poly1: "); debug_block(print_poly(poly1, *length_poly1); printf("\n"););
	    debug_msg("poly2: "); debug_block( print_poly(poly2, *length_poly2); printf("\n"););
        
        r_act = fabs(fmin(A1, A2) - opt);
        debug_print("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
  
        if (r_act <= r_prev)
            r_prev = r_act;
        else{
            debug_print("Se encontro la optimización con r_act %.2lf", r_act);
            v_other = search_prev_vertex_to_split(t, v_be, origen, triangles, adj);
            debug_print("Agregar edge %d - %d del nuevo triangulo %d | origen = %d \n", v_be, v_other,t, origen);
            split_poly(poly, length_poly, poly1, &(*length_poly1), poly2, &(*length_poly2), v_be, v_other);
            debug_msg("Poligonos generados\n");
			debug_msg("poly1: "); debug_block(print_poly(poly1, *length_poly1); printf("\n"););
			debug_msg("poly2: "); debug_block( print_poly(poly2, *length_poly2); printf("\n"););
            debug_block(
            A1 =get_area_poly(poly1, *length_poly1,r);
            A2 = get_area_poly(poly2,*length_poly2,r););
            debug_print("A: %.2lf, A1: %.2lf, A2:  %.2lf, A1/A = %.2lf, A2/A = %.2lf, r_prev = %.2lf, r_act = %.2lf\n", A_poly, A1 , A2,  A1/A_poly, A2/A_poly, r_prev, r_act);
            debug_msg("División optima terminada\n");
            break;
        }
        
    }
    
}


/* Divide un poly dado un vertice e1-e2
    resultados poly1 y poly*/

void split_poly(int *original_poly, int length_poly, int *poly1, int *length_poly1, int *poly2, int *length_poly2, int e1, int e2){
    int pos1, pos2,i;
    for(i =0; i< length_poly; i++)
        if(original_poly[i] == e1 || original_poly[i] == e2){
            pos1 = i;
            break;
        }
    for(i =pos1 + 1; i< length_poly; i++)
        if(original_poly[i] == e1  || original_poly[i] == e2){
            pos2 = i;
            break;
        }

    debug_print("Divide pos1: %d, pos2: %d \n", pos1, pos2);
    
    *length_poly1 = abs(pos1-pos2) +1;
    *length_poly2 = length_poly - *length_poly1 +2;

    for (i = 0; i < *length_poly1 ; i++)
        poly1[i] = original_poly[(pos1 + i) %length_poly];

    for (i = 0; i < *length_poly2 ; i++)
        poly2[i] = original_poly[(pos2 + i) %length_poly];
}

int copy_poly(int *in, int *out, int len){
    int i;
    for (i = 0; i < len; i++)
        out[i] = in[i];
    return len;
}

void print_poly(int *poly, int length_poly){
    int i;
    printf("(%d)", length_poly);
    for (i = 0; i < length_poly; i++)
        printf(" %d", poly[i]);   
}

double get_area_poly(int *poly, int length_poly, double *r){
    double area = 0.0;
    double x1,y1,x2,y2;
    int i,j;
    for (i = 0; i < length_poly; i++)
    {
        j = (i+1) %length_poly;
        x1=r[2*poly[i] + 0];
        y1=r[2*poly[i] + 1];
        x2=r[2*poly[j] + 0];
        y2=r[2*poly[j] + 1];
        area += (x1 + x2)*(y2 - y1);
    }
    return fabs(area/2);
}


void save_to_mesh(int *mesh, int *poly, int *i_mesh, int length_poly, int *pos_poly, int *id_pos_poly){
    
    int i;
    for (i = 0; i < length_poly; i++){        
        mesh[*i_mesh + i] = poly[i];
    }
    *i_mesh += length_poly;
    pos_poly[*id_pos_poly] = *i_mesh;
	*id_pos_poly = *id_pos_poly + 1;
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
            return poly[(i+1) %length_poly];
    }
    printf("num_BE %d\n", count_BarrierEdges(poly, length_poly));
    printf("%s:%d:%s(): No se encontro vertice BarrierEdge\n",__FILE__,  __LINE__, __func__);
    exit(0);
    return -1;
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

            initial_point = triangles[3 * i + 1];
            end_point = triangles[3 * i + 0];

        } else if (t2 == NO_ADJ && t0 == NO_ADJ) {

            poly[ind_poly] = triangles[3 * i + 0];
            ind_poly++;
            poly[ind_poly] = triangles[3 * i + 1];
            ind_poly++;

            initial_point = triangles[3 * i + 0];
            end_point = triangles[3 * i + 2];

        } else if (t1 == NO_ADJ && t2 == NO_ADJ) {
            poly[ind_poly] = triangles[3 * i + 2];
            ind_poly++;
            poly[ind_poly] = triangles[3 * i + 0];
            ind_poly++;

            end_point = triangles[3 * i + 1];
            initial_point = triangles[3 * i + 2];
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
        while (initial_point != end_point) {

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

                        end_point = triangles[3 * k + 0];

                    } else if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = triangles[3 * k + 1];
                    }
                } else if (t2 == NO_ADJ && t0 == NO_ADJ) {
                    /*si endpoint es continua a t2  y t2-t0 son fe*/
                    if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = triangles[3 * k + 2];

                    } else if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = triangles[3 * k + 0];

                    }
                } else if (t1 == NO_ADJ && t2 == NO_ADJ) {
                    /*si endpoint es continua a t1 y t1-t2 son fe*/
                    if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = triangles[3 * k + 1];

                    } else if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = triangles[3 * k + 2];

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

                        end_point = triangles[3 * k + 2];

                    } else if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = triangles[3 * k + 1];

                    }
                } else if (t2 == NO_ADJ) {
                    /*si endpoint es continua a t2  y t2 es fe*/
                    if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = triangles[3 * k + 1];

                    } else if (continuous == 1) {
                        poly[ind_poly] = triangles[3 * k + 1];
                        ind_poly++;

                        end_point = triangles[3 * k + 0];

                    }
                } else if (t1 == NO_ADJ) {
                    /*si endpoint es continua a t1  y t1 es fe*/
                    if (continuous == 2) {
                        poly[ind_poly] = triangles[3 * k + 2];
                        ind_poly++;

                        end_point = triangles[3 * k + 0];

                    } else if (continuous == 0) {
                        poly[ind_poly] = triangles[3 * k + 0];
                        ind_poly++;

                        end_point = triangles[3 * k + 2];

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