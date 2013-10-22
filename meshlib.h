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
#define MESH_ORIGIN_TYPE_PLY 5


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

    INTDATA num_vertices;
    INTDATA num_faces;

    mesh_vertex *vertices;
    mesh_face *faces;
    mesh_normal *vnormals;
    mesh_normal *fnormals;
    mesh_color *vcolors;
    mesh_color *fcolors;

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


int mesh_calculate_vertex_normals(MESH m);


int mesh_isnumeric(FILEPOINTER fp);
int mesh_go_next_word(FILEPOINTER fp);
int mesh_read_word(FILEPOINTER fp, char *c_word, int sz);
int mesh_count_words_in_line(FILEPOINTER fp, int *count);

void __mesh_calculate_vertex_normal(MESH_VERTEX v1, MESH_VERTEX v2, MESH_VERTEX v3, MESH_NORMAL n);
void mesh_draw_mesh(MESH m);

#ifdef __cplusplus
}
#endif

#endif
