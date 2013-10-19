
#include "meshlib.h"



MESH mesh_create_mesh_new()
{
    MESH m = NULL;
    if((m = (MESH)malloc(sizeof(mesh)))==NULL) mesh_error(MESH_ERR_MALLOC);
    m->is_vertices = 0;
    m->is_faces = 0;
    m->is_vcolors = 0;
    m->is_fcolors = 0;
    m->is_vnormals = 0;
    m->is_fnormals = 0;

    m->num_vertices = 0;
    m->num_faces = 0;

    m->vertices = NULL;
    m->faces = NULL;

    m->vcolors = NULL;
    m->fcolors = NULL;

    m->vnormals = NULL;
    m->fnormals = NULL;

    m->is_trimesh = 0;

    return m;
}


void mesh_free_mesh(MESH m)
{
    int k;
    if(m->is_vertices) free(m->vertices);
    if(m->is_faces)
    {
        for(k=0; k<m->num_faces; ++k) free(m->faces[k].vertices);
        free(m->faces);
    }
    if(m->is_vcolors) free(m->vcolors);
    if(m->is_fcolors) free(m->fcolors);
    if(m->is_vnormals) free(m->vnormals);
    if(m->is_fnormals) free(m->fnormals);
    free(m);
}


