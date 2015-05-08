/**
 * @file meshcreate.c
 * @author Sk. Mohammadul Haque
 * @version 1.3.0.0
 * @copyright
 * Copyright (c) 2013, 2014, 2015 Sk. Mohammadul Haque.
 * @brief This file contains functions pertaining to mesh creation and freeing.
 */

#include "../include/meshlib.h"

/** \brief Creates a new mesh
 *
 * \return Output mesh
 *
 */

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
    m->is_vfaces = 0;

    m->is_fareas = 0;

    m->num_vertices = 0;
    m->num_faces = 0;

    m->vertices = NULL;
    m->faces = NULL;

    m->vcolors = NULL;
    m->fcolors = NULL;

    m->vnormals = NULL;
    m->fnormals = NULL;

    m->vfaces = NULL;
    m->fareas = NULL;


    m->is_trimesh = 0;

    return m;
}

/** \brief Frees a mesh
 *
 * \param[in] m Input mesh
 * \return NULL
 *
 */

void mesh_free_mesh(MESH m)
{
    INTDATA i;
    if(m->is_vertices) free(m->vertices);
    if(m->is_faces)
    {
        for(i=0; i<m->num_faces; ++i) free(m->faces[i].vertices);
        free(m->faces);
    }
    if(m->is_vcolors) free(m->vcolors);
    if(m->is_fcolors) free(m->fcolors);
    if(m->is_vnormals) free(m->vnormals);
    if(m->is_fnormals) free(m->fnormals);
    if(m->is_vfaces)
    {
        for(i=0; i<m->num_vertices; ++i)
        {
            if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
        }
        free(m->vfaces);
    }
    if(m->is_fareas) free(m->fareas);

    free(m);
}

