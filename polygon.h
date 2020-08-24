extern int save_to_mesh(int *mesh, int *poly, int i_mesh, int length_poly);
extern int generate_polygon(int * poly, int * triangles, int * adj, double *r, int * visited, int i, int adj_counter);
extern int count_FrontierEdges(int triangle, int *adj);
