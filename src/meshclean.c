/**
 * @file meshclean.c
 * @author Sk. Mohammadul Haque
 * @version 1.3.0.0
 * @copyright
 * Copyright (c) 2013, 2014, 2015 Sk. Mohammadul Haque.
 * @brief This file contains functions pertaining to different mesh cleaning algorithms.
 */

#include <string.h>
#include "../include/meshlib.h"

/** \cond HIDDEN_SYMBOLS */

__inline FLOATDATA __mesh_calc_triangle_area(MESH_VERTEX a, MESH_VERTEX b, MESH_VERTEX c)
{
    static mesh_vertex p, q, r;
    static FLOATDATA area;
    p.x = a->x - b->x;
    p.y = a->y - b->y;
    p.z = a->z - b->z;
    q.x = a->x - c->x;
    q.y = a->y - c->y;
    q.z = a->z - c->z;
    mesh_cross_vector3(&p, &q, &r);
    area = 0.5*sqrt(r.x*r.x+r.y*r.y+r.z*r.z);
    return area;
}

__inline FLOATDATA __mesh_calc_vertex_distance_squared(MESH_VERTEX a, MESH_VERTEX b)
{
    static FLOATDATA dx, dy, dz;
    dx = a->x-b->x;
    dy = a->y-b->y;
    dz = a->z-b->z;
    return (dx*dx+dy*dy+dz*dz);
}

__inline INTDATA __mesh_find_parent(INTDATA i, INTDATA* parents)
{
    if(parents[i]!=i) parents[i] = __mesh_find_parent(parents[i], parents);
    return parents[i];
}

__inline void __mesh_union(INTDATA i, INTDATA j, INTDATA* parents, INTDATA* ranks)
{
    static INTDATA ir, jr;
    ir = __mesh_find_parent(i, parents);
    jr = __mesh_find_parent(j, parents);
    if(ir==jr) return;
    if(ranks[i]<ranks[j]) parents[i] = j;
    else if(ranks[i]>ranks[j]) parents[j] = i;
    else
    {
        parents[j] = i;
        ++ranks[i];
    }
}

__inline INTDATA __mesh_find2(MESH_STRUCT2 s, INTDATA q)
{
    INTDATA k;
    for(k=0; k<s->num_items; ++k)
    {
        if(s->items[k][0]==q) return k;
    }
    return -1;
}

#define __mesh_rm_vertices (0)
#define __mesh_rm_faces (1)

int __mesh_remove_boundary_elements(MESH m, int iters, int type)
{
    MESH_STRUCT2 e_table = NULL;
    MESH_FACE new_faces = NULL;
    MESH_COLOR new_fcolors = NULL;
    MESH_NORMAL new_fnormals = NULL;
    char *fflags = NULL;
    INTDATA i, j, k, s, num_deleted;
    INTDATA i_01, i_12, i_02;
    INTDATA v0, v1, v2, v_tmp;


    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    if(m->is_trimesh==0) return 3;

    for(s=0; s<iters; ++s)
    {
        num_deleted = 0;
        if(m->is_vfaces!=1) mesh_calc_vertex_adjacency(m);

        e_table = (MESH_STRUCT2)malloc(m->num_vertices*sizeof(mesh_struct2));
        if((fflags = (char *)malloc(sizeof(char)*(m->num_faces)))==NULL) mesh_error(MESH_ERR_MALLOC);
        memset(fflags, 0, sizeof(char)*(m->num_faces));
        if(e_table==NULL) mesh_error(MESH_ERR_MALLOC);
        for(i=0; i<m->num_vertices; ++i)
        {
            e_table[i].num_items = 0;
            e_table[i].items = NULL;
        }
        for(i=0; i<m->num_faces; ++i)
        {
            v0 = m->faces[i].vertices[0];
            v1 = m->faces[i].vertices[1];
            v2 = m->faces[i].vertices[2];
            if(v0>v1)
            {
                v_tmp = v0;
                v0 = v1;
                v1 = v_tmp;
            }
            if(v0>v2)
            {
                v_tmp = v0;
                v0 = v2;
                v2 = v_tmp;
            }
            if(v1>v2)
            {
                v_tmp = v1;    /* v0<v1<v2 */
                v1 = v2;
                v2 = v_tmp;
            }

            i_01 = __mesh_find2(&e_table[v0], v1);

            if(i_01<0)
            {
                if((e_table[v0].items = (INTDATA2*)realloc(e_table[v0].items, sizeof(INTDATA2)*(e_table[v0].num_items+1)))==NULL) mesh_error(MESH_ERR_MALLOC);
                e_table[v0].num_items += 1;
                e_table[v0].items[e_table[v0].num_items-1][0] = v1;

                e_table[v0].items[e_table[v0].num_items-1][1] = 1;
            }
            else
            {
                ++e_table[v0].items[i_01][1];
            }

            i_12 = __mesh_find2(&e_table[v1], v2);

            if(i_12<0)
            {
                if((e_table[v1].items = (INTDATA2*)realloc(e_table[v1].items, sizeof(INTDATA2)*(e_table[v1].num_items+1)))==NULL) mesh_error(MESH_ERR_MALLOC);
                e_table[v1].num_items += 1;
                e_table[v1].items[e_table[v1].num_items-1][0] = v2;

                e_table[v1].items[e_table[v1].num_items-1][1] = 1;
            }
            else
            {
                ++e_table[v1].items[i_12][1];
            }

            i_02 = __mesh_find2(&e_table[v0], v2);

            if(i_02<0)
            {
                if((e_table[v0].items = (INTDATA2*)realloc(e_table[v0].items, sizeof(INTDATA2)*(e_table[v0].num_items+1)))==NULL) mesh_error(MESH_ERR_MALLOC);
                e_table[v0].num_items += 1;
                e_table[v0].items[e_table[v0].num_items-1][0] = v2;

                e_table[v0].items[e_table[v0].num_items-1][1] = 1;
            }
            else
            {
                ++e_table[v0].items[i_02][1];
            }


        }

        if(type==__mesh_rm_vertices)
        {
            for(i=0; i<m->num_vertices; ++i)
            {
                for(j=0; j<e_table[i].num_items; ++j)
                {
                    if(e_table[i].items[j][1]<=1)
                    {
                        for(k=0; k<m->vfaces[i].num_faces; ++k)
                        {
                            fflags[m->vfaces[i].faces[k]]= 1;
                        }
                        for(k=0; k<m->vfaces[e_table[i].items[j][0]].num_faces; ++k)
                        {
                            fflags[m->vfaces[e_table[i].items[j][0]].faces[k]]= 1;
                        }
                    }
                }
            }
        }
        else
        {
            for(i=0; i<m->num_vertices; ++i)
            {
                for(j=0; j<e_table[i].num_items; ++j)
                {
                    if(e_table[i].items[j][1]<=1)
                    {
                        MESH_FACE curr_face;
                        for(k=0; k<m->vfaces[i].num_faces; ++k)
                        {
                            curr_face = &(m->faces[m->vfaces[i].faces[k]]);
                            if(curr_face->vertices[0]==e_table[i].items[j][0]||curr_face->vertices[1]==e_table[i].items[j][0]||curr_face->vertices[2]==e_table[i].items[j][0])
                            {
                                fflags[m->vfaces[i].faces[k]]= 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        for(i=0; i<m->num_faces; ++i) if(fflags[i]==1) ++num_deleted;
        for(i=0; i<m->num_vertices; ++i) free(e_table[i].items);
        free(e_table);
        if(num_deleted>0)
        {
            if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
            j = 0;
            for(i=0; i<m->num_faces; ++i)
            {
                if(fflags[i]!=1)
                {
                    new_faces[j].num_vertices = 3;
                    if((new_faces[j].vertices = (INTDATA *)malloc(sizeof(INTDATA)*3))==NULL) mesh_error(MESH_ERR_MALLOC);
                    new_faces[j].vertices[0] = m->faces[i].vertices[0];
                    new_faces[j].vertices[1] = m->faces[i].vertices[1];
                    new_faces[j].vertices[2] = m->faces[i].vertices[2];
                    ++j;
                }
            }
            if(m->is_fcolors)
            {
                if((new_fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                j = 0;
                for(i=0; i<m->num_faces; ++i)
                {
                    if(fflags[i]!=1)
                    {
                        new_fcolors[j].r = m->fcolors[i].r;
                        new_fcolors[j].g = m->fcolors[i].g;
                        new_fcolors[j].b = m->fcolors[i].b;
                        new_fcolors[j].a = m->fcolors[i].a;
                        ++j;
                    }
                }
                free(m->fcolors);
                m->fcolors = new_fcolors;
            }

            if(m->is_fnormals)
            {
                if((new_fnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                for(i=0; i<m->num_faces; ++i)
                {
                    if(fflags[i]!=1)
                    {
                        new_fnormals[j].x = m->fnormals[i].x;
                        new_fnormals[j].y = m->fnormals[i].y;
                        new_fnormals[j].z = m->fnormals[i].z;
                    }
                }
                free(m->fnormals);
                m->fnormals = new_fnormals;
            }

            if(m->is_vfaces)
            {
                for(i=0; i<m->num_vertices; ++i)
                {
                    if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
                }
                free(m->vfaces);
                m->vfaces = NULL;
            }
            m->is_vfaces = 0;

            m->num_faces -= num_deleted;
            free(m->faces);
            m->faces = new_faces;
            free(fflags);
            mesh_remove_unreferenced_vertices(m);
        }

    }
    return 0;
}

/** \endcond */

/** \brief Removes boundary vertices and connecting elements
 *
 * \param[in] m Input mesh
 * \param[in] iters Number of iterations
 * \return Error code
 *
 */

int mesh_remove_boundary_vertices(MESH m, int iters)
{
    return __mesh_remove_boundary_elements(m, iters, __mesh_rm_vertices);
}

/** \brief Removes boundary faces and connecting elements
 *
 * \param[in] m Input mesh
 * \param[in] iters Number of iterations
 * \return Error code
 *
 */

int mesh_remove_boundary_faces(MESH m, int iters)
{
    return __mesh_remove_boundary_elements(m, iters, __mesh_rm_faces);
}

/** \brief Removes triangles with area smaller than a given value
 *
 * \param[in] m Input mesh
 * \param[in] area Given area
 * \return Error code
 *
 */

int mesh_remove_triangles_with_small_area(MESH m, FLOATDATA area)
{
    char *fflags = NULL;
    MESH_FACE new_faces = NULL;
    MESH_COLOR new_fcolors = NULL;
    MESH_NORMAL new_fnormals = NULL;
    INTDATA i, j, num_deleted = 0;
    if(area==0) area = 1e-18;

    if(m->is_trimesh && m->is_faces)
    {
        if((fflags = (char *)malloc(sizeof(char)*(m->num_faces)))==NULL) mesh_error(MESH_ERR_MALLOC);
        memset(fflags, 0, sizeof(char)*(m->num_faces));
        for(i=0; i<m->num_faces; ++i)
        {
            if(mesh_calc_triangle_area(&(m->vertices[m->faces[i].vertices[0]]),&(m->vertices[m->faces[i].vertices[1]]),&(m->vertices[m->faces[i].vertices[2]]))<area)
            {
                fflags[i]= 1;
                ++num_deleted;
            }
        }
        if(num_deleted>0)
        {
            if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
            j = 0;
            for(i=0; i<m->num_faces; ++i)
            {
                if(fflags[i]!=1)
                {
                    new_faces[j].num_vertices = 3;
                    if((new_faces[j].vertices = (INTDATA *)malloc(sizeof(INTDATA)*3))==NULL) mesh_error(MESH_ERR_MALLOC);
                    new_faces[j].vertices[0] = m->faces[i].vertices[0];
                    new_faces[j].vertices[1] = m->faces[i].vertices[1];
                    new_faces[j].vertices[2] = m->faces[i].vertices[2];
                    ++j;
                }
            }
            if(m->is_fcolors)
            {
                if((new_fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                j = 0;
                for(i=0; i<m->num_faces; ++i)
                {
                    if(fflags[i]!=1)
                    {
                        new_fcolors[j].r = m->fcolors[i].r;
                        new_fcolors[j].g = m->fcolors[i].g;
                        new_fcolors[j].b = m->fcolors[i].b;
                        new_fcolors[j].a = m->fcolors[i].a;
                        ++j;
                    }
                }
                free(m->fcolors);
                m->fcolors = new_fcolors;
            }

            if(m->is_fnormals)
            {
                if((new_fnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                for(i=0; i<m->num_faces; ++i)
                {
                    if(fflags[i]!=1)
                    {
                        new_fnormals[j].x = m->fnormals[i].x;
                        new_fnormals[j].y = m->fnormals[i].y;
                        new_fnormals[j].z = m->fnormals[i].z;
                    }
                }
                free(m->fnormals);
                m->fnormals = new_fnormals;
            }

            if(m->is_vfaces)
            {
                for(i=0; i<m->num_vertices; ++i)
                {
                    if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
                }
                free(m->vfaces);
                m->vfaces = NULL;
            }
            m->is_vfaces = 0;

            m->num_faces -= num_deleted;
            free(m->faces);
            m->faces = new_faces;
            free(fflags);
            mesh_calc_vertex_adjacency(m);
        }
    }
    return 0;
}

/** \brief Removes triangles with zero area
 *
 * \param[in] m Input mesh
 * \return Error code
 *
 */

int mesh_remove_zero_area_faces(MESH m)
{
    return mesh_remove_triangles_with_small_area(m, 0);
}

/** \brief Removes unreferenced vertices
 *
 * \param[in] m Input mesh
 * \return Error code
 *
 */

int mesh_remove_unreferenced_vertices(MESH m)
{
    char *vflags = NULL;
    INTDATA *vindx = NULL;
    MESH_VERTEX new_vertices = NULL;
    MESH_COLOR new_vcolors = NULL;
    MESH_NORMAL new_vnormals = NULL;
    INTDATA num_valid_flags = 0, i, j;
    if((vflags = (char *)malloc(sizeof(char)*(m->num_vertices)))==NULL) mesh_error(MESH_ERR_MALLOC);
    if((vindx = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices)))==NULL) mesh_error(MESH_ERR_MALLOC);
    memset(vflags, 0, sizeof(char)*(m->num_vertices));
    memset(vindx, 0, sizeof(INTDATA)*(m->num_vertices));

    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            vflags[m->faces[i].vertices[j]] = 1;
        }
    }
    vindx[0] = vflags[0]-1;
    for(i=1; i<m->num_vertices; ++i)
    {
        vindx[i] = vindx[i-1]+vflags[i];
    }
    num_valid_flags = vindx[m->num_vertices-1]+1;
    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            m->faces[i].vertices[j] = vindx[m->faces[i].vertices[j]];
        }
    }

    if((new_vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*(num_valid_flags)))==NULL) mesh_error(MESH_ERR_MALLOC);
    for(i=0; i<m->num_vertices; ++i)
    {
        if(vflags[i]==1)
        {
            new_vertices[vindx[i]].x = m->vertices[i].x;
            new_vertices[vindx[i]].y = m->vertices[i].y;
            new_vertices[vindx[i]].z = m->vertices[i].z;
        }
    }
    if(m->is_vcolors)
    {
        if((new_vcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(num_valid_flags)))==NULL) mesh_error(MESH_ERR_MALLOC);
        for(i=0; i<m->num_vertices; ++i)
        {
            if(vflags[i]==1)
            {
                new_vcolors[vindx[i]].r = m->vcolors[i].r;
                new_vcolors[vindx[i]].g = m->vcolors[i].g;
                new_vcolors[vindx[i]].b = m->vcolors[i].b;
                new_vcolors[vindx[i]].a = m->vcolors[i].a;
            }
        }
        free(m->vcolors);
        m->vcolors = new_vcolors;
    }

    if(m->is_vnormals)
    {
        if((new_vnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(num_valid_flags)))==NULL) mesh_error(MESH_ERR_MALLOC);
        for(i=0; i<m->num_vertices; ++i)
        {
            if(vflags[i]==1)
            {
                new_vnormals[vindx[i]].x = m->vnormals[i].x;
                new_vnormals[vindx[i]].y = m->vnormals[i].y;
                new_vnormals[vindx[i]].z = m->vnormals[i].z;
            }
        }
        free(m->vnormals);
        m->vnormals = new_vnormals;
    }

    if(m->is_vfaces)
    {
        for(i=0; i<m->num_vertices; ++i)
        {
            if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
        }
        free(m->vfaces);
        m->vfaces = NULL;
    }
    m->is_vfaces = 0;

    m->num_vertices = num_valid_flags;
    free(m->vertices);
    m->vertices = new_vertices;
    free(vflags);
    free(vindx);
    mesh_calc_vertex_adjacency(m);
    return 0;
}

/** \brief Removes ear faces and connecting vertices
 *
 * \param[in] m Input mesh
 * \param[in] niters Number of iterations
 * \return Error code
 *
 */

int mesh_remove_ear_faces(MESH m, int niters)
{
    char *fflags = NULL;
    MESH_FACE new_faces = NULL;
    MESH_COLOR new_fcolors = NULL;
    MESH_NORMAL new_fnormals = NULL;
    INTDATA i, j, num_deleted;
    int iters;
    if(!m->is_vfaces) mesh_calc_vertex_adjacency(m);
    if(m->is_trimesh)
    {
        for(iters=0; iters<niters; ++iters)
        {
            if((fflags = (char *)malloc(sizeof(char)*(m->num_faces)))==NULL) mesh_error(MESH_ERR_MALLOC);
            memset(fflags, 0, sizeof(char)*(m->num_faces));
            num_deleted = 0;
            for(i=0; i<m->num_vertices; ++i)
            {
                if(m->vfaces[i].num_faces==1 && fflags[m->vfaces[i].faces[0]]==0)
                {
                    fflags[m->vfaces[i].faces[0]]= 1;
                    ++num_deleted;
                }
            }
            if(num_deleted>0)
            {
                if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                j = 0;
                for(i=0; i<m->num_faces; ++i)
                {
                    if(fflags[i]!=1)
                    {
                        new_faces[j].num_vertices = 3;
                        if((new_faces[j].vertices = (INTDATA *)malloc(sizeof(INTDATA)*3))==NULL) mesh_error(MESH_ERR_MALLOC);
                        new_faces[j].vertices[0] = m->faces[i].vertices[0];
                        new_faces[j].vertices[1] = m->faces[i].vertices[1];
                        new_faces[j].vertices[2] = m->faces[i].vertices[2];
                        ++j;
                    }
                }

                if(m->is_fcolors)
                {
                    if((new_fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                    j = 0;
                    for(i=0; i<m->num_faces; ++i)
                    {
                        if(fflags[i]!=1)
                        {
                            new_fcolors[j].r = m->fcolors[i].r;
                            new_fcolors[j].g = m->fcolors[i].g;
                            new_fcolors[j].b = m->fcolors[i].b;
                            new_fcolors[j].a = m->fcolors[i].a;
                            ++j;
                        }
                    }
                    free(m->fcolors);
                    m->fcolors = new_fcolors;
                }

                if(m->is_fnormals)
                {
                    if((new_fnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_faces-num_deleted)))==NULL) mesh_error(MESH_ERR_MALLOC);
                    for(i=0; i<m->num_faces; ++i)
                    {
                        if(fflags[i]!=1)
                        {
                            new_fnormals[j].x = m->fnormals[i].x;
                            new_fnormals[j].y = m->fnormals[i].y;
                            new_fnormals[j].z = m->fnormals[i].z;
                        }
                    }
                    free(m->fnormals);
                    m->fnormals = new_fnormals;
                }

                if(m->is_vfaces)
                {
                    for(i=0; i<m->num_vertices; ++i)
                    {
                        if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
                    }
                    free(m->vfaces);
                    m->vfaces = NULL;
                }
                m->is_vfaces = 0;

                m->num_faces -= num_deleted;
                free(m->faces);
                m->faces = new_faces;
                free(fflags);
                mesh_calc_vertex_adjacency(m);
            }
            else break;
        }
        mesh_remove_unreferenced_vertices(m);
    }
    return 0;
}

/** \brief Removes close vertices
 *
 * \param[in] m Input mesh
 * \param[in] r Maximum distance between two vertices
 * \return Error code
 *
 */

int mesh_remove_close_vertices(MESH m, FLOATDATA r)
{
    INTDATA i, j, k;
    INTDATA *vparents = NULL;
    INTDATA *vranks = NULL;
    if(!m->is_vfaces) mesh_calc_vertex_adjacency(m);
    if((vparents = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices)))==NULL) mesh_error(MESH_ERR_MALLOC);
    if((vranks = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices)))==NULL) mesh_error(MESH_ERR_MALLOC);
    r *= r;
    for(i=0; i<m->num_vertices; ++i)
    {
        vparents[i] = i;
        vranks[i] = 0;
    }
    for(i=0; i<m->num_vertices; ++i)
    {
        for(j=0; j<m->vfaces[i].num_faces; ++j)
        {
            for(k=0; k<m->faces[m->vfaces[i].faces[j]].num_vertices; ++k)
            {
                if(i==m->faces[m->vfaces[i].faces[j]].vertices[k]) continue;
                if(__mesh_calc_vertex_distance_squared(&(m->vertices[i]), &(m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]]))<r)
                {
                    __mesh_union(i, m->faces[m->vfaces[i].faces[j]].vertices[k], vparents, vranks);
                }
            }
        }
    }

    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            m->faces[i].vertices[j] = __mesh_find_parent(m->faces[i].vertices[j], vparents);
        }
    }
    free(vparents);
    free(vranks);
    mesh_remove_zero_area_faces(m);
    mesh_remove_unreferenced_vertices(m);
    return 0;
}



