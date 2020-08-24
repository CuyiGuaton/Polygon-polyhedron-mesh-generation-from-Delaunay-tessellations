/* Prototipos de funciones para manejar entrada y salida de datos. */

extern void read_qdelaunay_data(char *ppath, double **r, int **p, int **ady,
																double **a, int *pnumber, int *tnumuber, void *align_settings);
extern void write_classification(char *cpath_prefix, int *root_id, int *type, double *area,
																	int tnumber, int num_nonzone_triangs,
																	int num_ivoid_triangs, int num_bvoid_triangs, int num_wall_triangs);
extern void read_triangles(char *cpath, int **id, int *inumber);
extern void write_geomview(double *r, int *p,  int pnumber,
									int tnumber, int ind_poly, 
									int *polygon, int id_start_poly,int *start_polygon);
extern void read_fromfiles_data(char *ppath, double **r, int **p, int **adj,
													double **a, int *pnumber, int *tnumber, void **align_settings);