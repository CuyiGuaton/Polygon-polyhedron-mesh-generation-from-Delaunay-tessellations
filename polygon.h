extern void save_to_mesh(int *mesh, int *poly, int *i_mesh, int length_poly, int *pos_poly, int *id_pos_poly);
extern int generate_polygon(int * poly, int * triangles, int * adj, double *r, int * visited, int i, int adj_counter);
extern int count_BarrierEdges(int *poly, int length_poly);
extern int get_vertex_BarrierEdge(int *poly, int length_poly);
extern double get_area_poly(int *poly, int length_poly, double *r);
extern void split_poly(int *original_poly, int length_poly, int *poly1, int *length_poly1, int *poly2, int *length_poly2, int e1, int e2);
extern void print_poly(int *poly, int length_poly);
extern int copy_poly(int *in, int *out, int len);