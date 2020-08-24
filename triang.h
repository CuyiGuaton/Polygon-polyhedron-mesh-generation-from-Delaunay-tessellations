/* Prototipos de funciones para manejar triangulación. */

extern double dist(double x0, double y0, double x1, double y1);
extern int max_edge_index(int i, double *r, int *p);
extern int is_nomax_nomax(int i, int j, int *p, int *max);
extern int is_max_max(int i, int j, int *p, int *max);
extern int get_adjacent_triangle(int i, int k, int l, int *p, int *ady);
extern int same_edge(int u, int v, int w, int x);
extern int is_continuous(int i, int endpoint, int *p );
extern int get_adjacent_triangle_share_endpoint(int i, int origen, int endpoint, int *p, int *adj);
extern int count_FrontierEdges(int triangle, int *adj);
extern int search_triangle_by_vertex_with_FrontierEdge(int v, int *triangles, int *adj, int tnumber);
extern int search_another_vertex(int i, int v, int origen, int *triangles, int *adj);
