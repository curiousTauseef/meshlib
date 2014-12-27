#include <string.h>
#include "../include/meshlib.h"

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
    mesh_cross_vertex(&p, &q, &r);
    area = 0.5*sqrt(r.x*r.x+r.y*r.y+r.z*r.z);
    return area;
}

int mesh_remove_triangles_with_small_area(MESH m, FLOATDATA area)
{
    MESH_FACE new_faces = NULL;
    char *fflags = NULL;
    INTDATA num_valid_flags = 0, i, k;
    FLOATDATA curr_area;
    if(m->is_faces && m->is_trimesh)
    {
        if((fflags = (char *)malloc(sizeof(char)*(m->num_faces)))==NULL) mesh_error(MESH_ERR_MALLOC);
        for(i=0; i<m->num_faces; ++i)
        {
            curr_area = __mesh_calc_triangle_area(&(m->vertices[m->faces[i].vertices[0]]), &(m->vertices[m->faces[i].vertices[1]]), &(m->vertices[m->faces[i].vertices[2]]));
            if(curr_area>area)
            {
                ++num_valid_flags;
                fflags[i] = 1;
            }
            else fflags[i] = 0;
        }
        if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(num_valid_flags)))==NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_faces = 1;
        k = 0;
        for(i=0; i<m->num_faces; ++i)
        {
            if(fflags[i]==1)
            {
                new_faces[k].num_vertices = 3;
                if((new_faces[k].vertices = (INTDATA *)malloc(sizeof(INTDATA)*3))==NULL) mesh_error(MESH_ERR_MALLOC);
                new_faces[k].vertices[0] = m->faces[i].vertices[0];
                new_faces[k].vertices[1] = m->faces[i].vertices[1];
                new_faces[k].vertices[2] = m->faces[i].vertices[2];
                ++k;
            }
        }
        for(i=0; i<m->num_faces; ++i) free(m->faces[i].vertices);
        free(m->faces);
        m->faces = new_faces;
        m->num_faces = num_valid_flags;
        free(fflags);
        return 0;
    }
    else return 1;
}

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

