/**
 * @file meshlib.h
 * @author Sk. Mohammadul Haque
 * @version 1.3.0.0
 * @copyright
 * Copyright (c) 2013, 2014, 2015 Sk. Mohammadul Haque.
 * @brief This header file contains declarations of all functions of meshlib.
 */

#ifndef __MESHLIB__
#define __MESHLIB__

#define _CRT_SECURE_NO_DEPRECATE

/*! \mainpage Meshlib
 *
 * \section intro_sec Introduction
 * Meshlib is a simple mesh library written in C.
 *
 * \section build_sec Build
 * To build the whole project, Code::blocks is required.
 *
 * \section content_sec Contents
 * Load/Write PLY, OFF, ASC files.
 *
 * Basic Vertex Manipulations.
 *
 * Basic Vertex Transformations.
 *
 * Basic Face Manipulations.
 *
 * Bilateral Filtering.
 *
 * Laplacian Filtering.
 *
 * Mesh Cleaning Algorithms.
 *
 *
 */



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
typedef FILE *FILEPOINTER; /**< File pointer */
#else
typedef struct _iobuf *FILEPOINTER; /**< File pointer */
#endif


#define MESH_INTDATA_TYPE 0 /**< Integer datatype selector */
#define MESH_FLOATDATA_TYPE 0 /**< Float datatype selector */

#if MESH_INTDATA_TYPE==0
#define INTDATA int32_t /* do not change this, careful see meshload fscanf and other functions */ /**< Integer datatype */
#else
#define INTDATA int64_t /* do not change this, careful see meshload fscanf and other functions */ /**< Integer datatype */
#endif

#if MESH_FLOATDATA_TYPE==0
#define FLOATDATA float /* do not change this, careful see meshload fscanf and other functions */ /**< Float datatype */
#else
#define FLOATDATA double /* do not change this, careful see meshload fscanf and other functions */ /**< Float datatype */
#endif

#define MESH_ORIGIN_TYPE_BUILD 00 /**< Mesh origin type - create new */

#define MESH_ORIGIN_TYPE_OFF 11 /**< Mesh origin type - OFF file */
#define MESH_ORIGIN_TYPE_NOFF 12 /**< Mesh origin type - NOFF file */
#define MESH_ORIGIN_TYPE_COFF 13 /**< Mesh origin type - COFF file */
#define MESH_ORIGIN_TYPE_NCOFF 14 /**< Mesh origin type - NCOFF file */

#define MESH_ORIGIN_TYPE_XYZ 20 /**< Mesh origin type - XYZ file */

#define MESH_ORIGIN_TYPE_PLY_ASCII 30 /**< Mesh origin type - PLY ascii file */
#define MESH_ORIGIN_TYPE_PLY_BINARY_LITTLE_ENDIAN 31 /**< Mesh origin type - PLY binary LE file */
#define MESH_ORIGIN_TYPE_PLY_BINARY_BIG_ENDIAN 32 /**< Mesh origin type - PLY binary BE file */

#define MESH_ERR_MALLOC 0 /**< Mesh error type - allocation */
#define MESH_ERR_SIZE_MISMATCH 1 /**< Mesh error type - size mismatch */
#define MESH_ERR_FNOTOPEN 2 /**< Mesh error type - file open */
#define MESH_ERR_UNKNOWN 3 /**< Mesh error type - unknown */

#define MESH_PI (3.14159265359) /**< \f$ \pi \f$ */

typedef INTDATA INTDATA2[2]; /**< 2- element INTDATA */

typedef struct mesh_vector3
{
    FLOATDATA x; /**< x co-ordinate */
    FLOATDATA y; /**< y co-ordinate */
    FLOATDATA z; /**< z co-ordinate */
} mesh_vector3; /**< Generic 3-d vector */
typedef mesh_vector3* MESH_VECTOR3; /**< Generic 3-d vector pointer */


typedef mesh_vector3 mesh_vertex; /**< Vertex */
typedef mesh_vertex* MESH_VERTEX; /**< Vertex pointer */

typedef mesh_vector3 mesh_normal; /**< Normal */
typedef mesh_normal* MESH_NORMAL; /**< Normal pointer */

typedef struct mesh_color
{
    FLOATDATA r; /**< Red channel */
    FLOATDATA g; /**< Blue channel */
    FLOATDATA b; /**< Green channel */
    FLOATDATA a; /**< Alpha channel */
} mesh_color;
typedef mesh_color* MESH_COLOR; /**< Color */

typedef struct mesh_struct
{
    INTDATA num_items; /**< Number of items */
    INTDATA *items; /**< Pointer to INTDATA items */
} mesh_struct; /**< INTDATA Structure */
typedef mesh_struct* MESH_STRUCT; /**< INTDATA Structure pointer */

typedef struct mesh_struct2
{
    INTDATA num_items; /**< Number of items */
    INTDATA2 *items; /**< Pointer to INTDATA2 items */
} mesh_struct2; /**< INTDATA2 Structure */
typedef mesh_struct2* MESH_STRUCT2; /**< INTDATA2 Structure pointer */


typedef struct mesh_face
{
    INTDATA num_vertices; /**< Number of vertices */
    INTDATA* vertices; /**< Pointer to vertex indices */
} mesh_face; /**< Face */
typedef mesh_face* MESH_FACE; /**< Pointer to face */


typedef struct mesh_vface
{
    INTDATA num_faces; /**< Number of adjacent faces */
    INTDATA *faces; /**< Pointer to adjacent face indices */
} mesh_vface; /**< Vertex adjacent faces */
typedef mesh_vface* MESH_VFACE; /**< Pointer to vertex adjacent faces */


typedef struct mesh_rotation
{
    FLOATDATA data[9]; /**< Matrix data */
} mesh_rotation; /**< Rotation */
typedef mesh_rotation* MESH_ROTATION; /**< Pointer to rotation */

typedef struct mesh_transform
{
    FLOATDATA *data; /**< Matrix data */
} mesh_transform; /**< Transformation */
typedef mesh_transform* MESH_TRANSFORM; /**< Pointer to transformation */

typedef struct mesh
{
    uint8_t origin_type; /**< Origin type */
    uint8_t is_loaded; /**< Is loaded? */
    uint8_t is_vertices; /**< Has vertices? */
    uint8_t is_faces; /**< Has faces? */

    uint8_t is_vnormals; /**< Has vertex normals? */
    uint8_t is_fnormals; /**< Has face normals? */
    uint8_t is_vcolors; /**< Has vertex colors? */
    uint8_t is_fcolors; /**< Has face colors? */
    uint8_t is_vfaces; /**< Has vertex adjacent faces? */

    INTDATA num_vertices; /**< Number of vertices */
    INTDATA num_faces; /**< Number of faces */

    MESH_VERTEX vertices; /**< Pointer to vertices */
    MESH_FACE faces; /**< Pointer to faces */
    MESH_NORMAL vnormals; /**< Pointer to vertex normals */
    MESH_NORMAL fnormals; /**< Pointer to face normals */
    MESH_COLOR vcolors; /**< Pointer to vertex colors */
    MESH_COLOR fcolors; /**< Pointer to face colors */

    MESH_VFACE vfaces; /**< Pointer to vertex adjacency faces */

    uint8_t is_trimesh; /**< Is trimesh? */
    uint8_t dummy;
} mesh; /**< Mesh */
typedef mesh* MESH; /**< Pointer to mesh */

void mesh_error(int type);

MESH mesh_create_mesh_new();
void mesh_free_mesh(MESH m);

MESH mesh_load_file(const char* fname);

MESH mesh_load_off(const char* fname);
/** \cond HIDDEN_SYMBOLS */
MESH __mesh_parse_off_header(MESH m, FILEPOINTER fp);
MESH __mesh_parse_off_vertices(MESH m, FILEPOINTER fp);
MESH __mesh_parse_off_faces(MESH m, FILEPOINTER fp);
/** \endcond */

MESH mesh_load_xyz(const char* fname);
/** \cond HIDDEN_SYMBOLS */
MESH __mesh_parse_xyz_data(MESH m, FILEPOINTER fp);
/** \endcond */

MESH mesh_load_ply(const char* fname);
/** \cond HIDDEN_SYMBOLS */
MESH __mesh_parse_ply_header(MESH m, FILEPOINTER fp);
MESH __mesh_parse_ply_body(MESH m, FILEPOINTER fp);
MESH __mesh_parse_ply_vertices(MESH m, FILEPOINTER fp);
MESH __mesh_parse_ply_faces(MESH m, FILEPOINTER fp);
/** \endcond */

int mesh_write_file(MESH m, const char* fname);

int mesh_write_off(MESH m, const char* fname);
int mesh_write_xyz(MESH m, const char* fname);
int mesh_write_ply(MESH m, const char* fname);

int mesh_calc_vertex_normals(MESH m);
int mesh_calc_face_normals(MESH m);
int mesh_calc_vertex_adjacency(MESH m);
int mesh_upsample(MESH m, int iters);
void mesh_cross_vector3(MESH_VECTOR3 x, MESH_VECTOR3 y, MESH_VECTOR3 z);
void mesh_cross_normal(MESH_NORMAL x, MESH_NORMAL y, MESH_NORMAL z);
FLOATDATA mesh_calc_triangle_area(MESH_VERTEX a, MESH_VERTEX b, MESH_VERTEX c);
void mesh_calc_face_normal(MESH_VERTEX v1, MESH_VERTEX v2, MESH_VERTEX v3, MESH_NORMAL n);

INTDATA mesh_find(MESH_STRUCT s, INTDATA q);
INTDATA mesh_find2(MESH_STRUCT2 s, INTDATA q);

int mesh_remove_boundary_vertices(MESH m, int iters);
int mesh_remove_boundary_faces(MESH m, int iters);
int mesh_remove_triangles_with_small_area(MESH m, FLOATDATA area);
int mesh_remove_unreferenced_vertices(MESH m);
int mesh_remove_zero_area_faces(MESH m);
int mesh_remove_close_vertices(MESH m, FLOATDATA r);
int mesh_remove_ear_faces(MESH m, int niters);

int mesh_isnumeric(FILEPOINTER fp);
int mesh_go_next_word(FILEPOINTER fp);
int mesh_read_word(FILEPOINTER fp, char *c_word, int sz);
int mesh_count_words_in_line(FILEPOINTER fp, int *count);
int mesh_skip_line(FILEPOINTER fp);

int mesh_bilateral_filter(MESH m, FLOATDATA sigma_c, FLOATDATA sigma_s, int niters);
int mesh_laplacian_filter(MESH m, FLOATDATA r);
int mesh_restricted_laplacian_filter(MESH m, FLOATDATA r, FLOATDATA ang);

MESH_ROTATION mesh_rotation_create();
void mesh_rotation_free(MESH_ROTATION r);
MESH_ROTATION mesh_rotation_set_matrix(FLOATDATA *mat, MESH_ROTATION r);
MESH_ROTATION mesh_rotation_set_angleaxis(FLOATDATA ang, MESH_NORMAL axis, MESH_ROTATION r);

int mesh_translate(MESH m, FLOATDATA x, FLOATDATA y, FLOATDATA z);
int mesh_translate_vector(MESH m, MESH_VERTEX v);
int mesh_scale(MESH m, FLOATDATA sx, FLOATDATA sy, FLOATDATA sz);

MESH_VERTEX mesh_vertex_rotate(MESH_VERTEX v, MESH_ROTATION r);
int mesh_rotate(MESH m, MESH_ROTATION r);

void mesh_draw_mesh(MESH m);

#ifdef __cplusplus
}
#endif

#endif
