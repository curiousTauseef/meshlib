#ifndef __MESHLIB__
#define __MESHLIB__

#define _CRT_SECURE_NO_DEPRECATE

#ifdef __cplusplus
#define __MESH__CPP__
extern "C"
{
#endif


#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#if defined (GCC) || defined (__GNUC__)
typedef FILE *FILEPOINTER;
#else
typedef struct _iobuf *FILEPOINTER;
#endif


#define MESH_INTDATA_TYPE 0

#if MESH_INTDATA_TYPE == 0
#define INTDATA int32_t /* do not change this, careful see meshload fscanf and other functions */
#else
#define INTDATA int64_t /* do not change this, careful see meshload fscanf and other functions */
#endif
#define FLOATDATA double /* do not change this, careful see meshload fscanf and other functions */

#define MESH_ORIGIN_TYPE_BUILD 0
#define MESH_ORIGIN_TYPE_OFF 1
#define MESH_ORIGIN_TYPE_NOFF 2
#define MESH_ORIGIN_TYPE_COFF 3
#define MESH_ORIGIN_TYPE_XYZ 4
#define MESH_ORIGIN_TYPE_PLY_ASCII 5
#define MESH_ORIGIN_TYPE_PLY_BINARY_LITTLE_ENDIAN 6
#define MESH_ORIGIN_TYPE_PLY_BINARY_BIG_ENDIAN 7

#define MESH_ERR_MALLOC 0
#define MESH_ERR_SIZE_MISMATCH 1
#define MESH_ERR_FNOTOPEN 2
#define MESH_ERR_UNKNOWN 3

struct __mesh_vertex
{
    FLOATDATA x;
    FLOATDATA y;
    FLOATDATA z;
};
typedef struct __mesh_vertex mesh_vertex;
typedef struct __mesh_vertex* MESH_VERTEX;

struct __mesh_face
{
    INTDATA num_vertices;
    INTDATA* vertices;
};
typedef struct __mesh_face mesh_face;
typedef struct __mesh_face* MESH_FACE;

struct __mesh_normal
{
    FLOATDATA x;
    FLOATDATA y;
    FLOATDATA z;
};
typedef struct __mesh_normal mesh_normal;
typedef struct __mesh_normal* MESH_NORMAL;

struct __mesh_color
{
    FLOATDATA r;
    FLOATDATA g;
    FLOATDATA b;
    FLOATDATA a;
};
typedef struct __mesh_color mesh_color;
typedef struct __mesh_color* MESH_COLOR;


struct __mesh_vface
{
    INTDATA num_faces;
    INTDATA *faces;
};
typedef struct __mesh_vface mesh_vface;
typedef struct __mesh_vface* MESH_VFACE;

struct __mesh_struct
{
    INTDATA num_items;
    INTDATA *items;
};
typedef struct __mesh_struct mesh_struct;
typedef struct __mesh_struct * MESH_STRUCT;

struct __mesh
{
    uint8_t origin_type;
    uint8_t is_loaded;
    uint8_t is_vertices;
    uint8_t is_faces;

    uint8_t is_vnormals;
    uint8_t is_fnormals;
    uint8_t is_vcolors;
    uint8_t is_fcolors;
    uint8_t is_vfaces;

    INTDATA num_vertices;
    INTDATA num_faces;

    MESH_VERTEX vertices;
    MESH_FACE faces;
    MESH_NORMAL vnormals;
    MESH_NORMAL fnormals;
    MESH_COLOR vcolors;
    MESH_COLOR fcolors;

    MESH_VFACE vfaces;

    uint8_t is_trimesh;
    uint8_t dummy;
};
typedef struct __mesh mesh;
typedef struct __mesh* MESH;

void mesh_error(int type);

MESH mesh_create_mesh_new();
void mesh_free_mesh(MESH m);


MESH mesh_load_off(const char* fname);
MESH __mesh_parse_off_header(MESH m, FILEPOINTER fp);
MESH __mesh_parse_off_vertices(MESH m, FILEPOINTER fp);
MESH __mesh_parse_off_faces(MESH m, FILEPOINTER fp);

MESH mesh_load_xyz(const char* fname);
MESH __mesh_parse_xyz_data(MESH m, FILEPOINTER fp);

MESH mesh_load_ply(const char* fname);
MESH __mesh_parse_ply_header(MESH m, FILEPOINTER fp);
MESH __mesh_parse_ply_body(MESH m, FILEPOINTER fp);

int mesh_write_off(MESH m, const char* fname);
int mesh_write_xyz(MESH m, const char* fname);
int mesh_write_ply(MESH m, const char* fname);

int mesh_calc_vertex_normals(MESH m);
int mesh_calc_vertex_adjacency(MESH m);
int mesh_upsample(MESH m, int iters);
void mesh_cross_vertex(MESH_VERTEX x, MESH_VERTEX y, MESH_VERTEX z);
void mesh_cross_normal(MESH_NORMAL x, MESH_NORMAL y, MESH_NORMAL z);
INTDATA mesh_find(MESH_STRUCT s, INTDATA q);
FLOATDATA mesh_calc_triangle_area(MESH_VERTEX a, MESH_VERTEX b, MESH_VERTEX c);
void mesh_calc_vertex_normal(MESH_VERTEX v1, MESH_VERTEX v2, MESH_VERTEX v3, MESH_NORMAL n);
FLOATDATA mesh_calc_triangle_area(MESH_VERTEX a, MESH_VERTEX b, MESH_VERTEX c);
int mesh_remove_triangles_with_small_area(MESH m, FLOATDATA area);

int mesh_isnumeric(FILEPOINTER fp);
int mesh_go_next_word(FILEPOINTER fp);
int mesh_read_word(FILEPOINTER fp, char *c_word, int sz);
int mesh_count_words_in_line(FILEPOINTER fp, int *count);
int mesh_skip_line(FILEPOINTER fp);

int mesh_bilateral_filter(MESH m, FLOATDATA sigma_c, FLOATDATA sigma_s, int niters);

void mesh_draw_mesh(MESH m);

#ifdef __cplusplus
}
#endif

#endif
