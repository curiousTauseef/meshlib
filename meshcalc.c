#include "meshlib.h"
#include <string.h>

__inline void __mesh_cross(MESH_NORMAL x, MESH_NORMAL y, MESH_NORMAL z)
{
    FLOATDATA n;
    z->x = x->y*y->z - y->y*x->z;
    z->y = x->z*y->x - y->z*x->x;
    z->z = x->x*y->y - y->x*x->y;
    n = sqrt(z->x*z->x+z->y*z->y+z->z*z->z);
    if(n>0)
    {
        z->x /=n;
        z->y /=n;
        z->z /=n;
    }
}

void mesh_cross_vertex(MESH_VERTEX x, MESH_VERTEX y, MESH_VERTEX z)
{
    z->x = x->y*y->z - y->y*x->z;
    z->y = x->z*y->x - y->z*x->x;
    z->z = x->x*y->y - y->x*x->y;
}

void mesh_cross_normal(MESH_NORMAL x, MESH_NORMAL y, MESH_NORMAL z)
{
    FLOATDATA n;
    z->x = x->y*y->z - y->y*x->z;
    z->y = x->z*y->x - y->z*x->x;
    z->z = x->x*y->y - y->x*x->y;
    n = sqrt(z->x*z->x+z->y*z->y+z->z*z->z);
    if(n>0)
    {
        z->x /=n;
        z->y /=n;
        z->z /=n;
    }
}

int mesh_calc_vertex_normals(MESH m)
{
    INTDATA i, j;
    FLOATDATA t;
    mesh_normal curr_normal, e1, e2, e3;

    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    if(m->vfaces==0) mesh_calc_vertex_adjacency(m);
    if(!m->is_vnormals)
    {
        if((m->vnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    }
    m->is_vnormals = 1;


    for(i=0; i<m->num_vertices; ++i)
    {
        curr_normal.x = 0;
        curr_normal.y = 0;
        curr_normal.z = 0;

        for(j=0; j<m->vfaces[i].num_faces; ++j)
        {
            e1.x = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[0]].x - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].x;
            e1.y = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[0]].y - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].y;
            e1.z = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[0]].z - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].z;

            e2.x = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[2]].x - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].x;
            e2.y = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[2]].y - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].y;
            e2.z = m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[2]].z - m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[1]].z;

            __mesh_cross(&e2, &e1, &e3);
            curr_normal.x +=e3.x;
            curr_normal.y +=e3.y;
            curr_normal.z +=e3.z;
        }
        t = sqrt(curr_normal.x*curr_normal.x+curr_normal.y*curr_normal.y+curr_normal.z*curr_normal.z);
        if(t>0)
        {
            m->vnormals[i].x = curr_normal.x /t;
            m->vnormals[i].y = curr_normal.y /t;
            m->vnormals[i].z = curr_normal.z /t;
        }
    }
    return 0;
}

int mesh_calc_vertex_adjacency(MESH m)
{
    INTDATA i, j;
    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    if(m->is_vfaces)
    {
        for(i=0; i<m->num_vertices; ++i)
        {
            if(m->vfaces[i].faces!=NULL) free(m->vfaces[i].faces);
        }
        free(m->vfaces);
        m->vfaces = NULL;
    }
    m->vfaces = (MESH_VFACE)malloc(m->num_vertices*sizeof(mesh_vface));
    if(m->vfaces == NULL) mesh_error(MESH_ERR_MALLOC);
    for(i=0; i<m->num_vertices; ++i)
    {
        m->vfaces[i].num_faces = 0;
        m->vfaces[i].faces = NULL;
    }
    m->is_vfaces = 1;
    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            m->vfaces[m->faces[i].vertices[j]].faces = realloc(m->vfaces[m->faces[i].vertices[j]].faces, sizeof(INTDATA)*(m->vfaces[m->faces[i].vertices[j]].num_faces+1));
            ++(m->vfaces[m->faces[i].vertices[j]].num_faces);
            m->vfaces[m->faces[i].vertices[j]].faces[m->vfaces[m->faces[i].vertices[j]].num_faces-1] = i;
        }
    }
    return 0;
}

__inline INTDATA __mesh_find(MESH_STRUCT s, INTDATA q)
{
    INTDATA k;
    for(k=0; k<s->num_items; ++k)
    {
        if(s->items[k]==q) return k;
    }
    return -1;
}

INTDATA mesh_find(MESH_STRUCT s, INTDATA q)
{
    INTDATA k;
    for(k=0; k<s->num_items; ++k)
    {
        if(s->items[k]==q) return k;
    }
    return -1;
}

int mesh_upsample(MESH m, int iters)
{
    MESH_STRUCT v_table = NULL;
    MESH_FACE new_faces = NULL;
    INTDATA i, k, curr_idx;
    FLOATDATA t;
    INTDATA kv, i_01, i_12, i_20, c_v_0_indcs, c_v_1_indcs, c_v_2_indcs;

    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    if(m->is_trimesh==0) return 3;
    if(m->is_fcolors!=0) return 4;
    if(m->is_fnormals!=0) return 5;

    for(k=0; k<iters; ++k)
    {
        new_faces = (MESH_FACE)malloc(4*m->num_faces*sizeof(mesh_face));
        v_table = (MESH_STRUCT)malloc(m->num_vertices*sizeof(mesh_struct));
        if(v_table == NULL ||new_faces == NULL) mesh_error(MESH_ERR_MALLOC);
        for(i=0; i<m->num_vertices; ++i)
        {
            v_table[i].num_items = 0;
            v_table[i].items = NULL;
        }
        kv = m->num_vertices-1;
        for(i=0; i<m->num_faces; ++i)
        {
            i_01 = __mesh_find(&v_table[m->faces[i].vertices[0]], m->faces[i].vertices[1]);

            if(i_01<0)
            {
                ++kv;
                c_v_0_indcs = kv;
                v_table[m->faces[i].vertices[0]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[0]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[0]].num_items+2));
                v_table[m->faces[i].vertices[0]].num_items += 2;
                v_table[m->faces[i].vertices[0]].items[v_table[m->faces[i].vertices[0]].num_items-2] = m->faces[i].vertices[1];
                v_table[m->faces[i].vertices[0]].items[v_table[m->faces[i].vertices[0]].num_items-1] = c_v_0_indcs;

                v_table[m->faces[i].vertices[1]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[1]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[1]].num_items+2));
                v_table[m->faces[i].vertices[1]].num_items += 2;
                v_table[m->faces[i].vertices[1]].items[v_table[m->faces[i].vertices[1]].num_items-2] = m->faces[i].vertices[0];
                v_table[m->faces[i].vertices[1]].items[v_table[m->faces[i].vertices[1]].num_items-1] = c_v_0_indcs;

                m->vertices = (MESH_VERTEX)realloc(m->vertices, sizeof(mesh_vertex)*(kv+1));
                m->vertices[kv].x = 0.5*(m->vertices[m->faces[i].vertices[0]].x+m->vertices[m->faces[i].vertices[1]].x);
                m->vertices[kv].y = 0.5*(m->vertices[m->faces[i].vertices[0]].y+m->vertices[m->faces[i].vertices[1]].y);
                m->vertices[kv].z = 0.5*(m->vertices[m->faces[i].vertices[0]].z+m->vertices[m->faces[i].vertices[1]].z);
                if(m->is_vnormals)
                {
                    m->vnormals = (MESH_NORMAL)realloc(m->vnormals, sizeof(mesh_normal)*(kv+1));
                    m->vnormals[kv].x = m->vnormals[m->faces[i].vertices[0]].x+m->vnormals[m->faces[i].vertices[1]].x;
                    m->vnormals[kv].y = m->vnormals[m->faces[i].vertices[0]].y+m->vnormals[m->faces[i].vertices[1]].y;
                    m->vnormals[kv].z = m->vnormals[m->faces[i].vertices[0]].z+m->vnormals[m->faces[i].vertices[1]].z;
                    t = sqrt(m->vnormals[kv].x*m->vnormals[kv].x+m->vnormals[kv].y*m->vnormals[kv].y+m->vnormals[kv].z*m->vnormals[kv].z);
                    m->vnormals[kv].x /= t;
                    m->vnormals[kv].y /= t;
                    m->vnormals[kv].z /= t;
                }
                if(m->is_vcolors)
                {
                    m->vcolors = (MESH_COLOR)realloc(m->vcolors, sizeof(mesh_color)*(kv+1));
                    m->vcolors[kv].r = 0.5*(m->vcolors[m->faces[i].vertices[0]].r+m->vcolors[m->faces[i].vertices[1]].r);
                    m->vcolors[kv].g = 0.5*(m->vcolors[m->faces[i].vertices[0]].g+m->vcolors[m->faces[i].vertices[1]].g);
                    m->vcolors[kv].b = 0.5*(m->vcolors[m->faces[i].vertices[0]].b+m->vcolors[m->faces[i].vertices[1]].b);
                    m->vcolors[kv].a = 0.5*(m->vcolors[m->faces[i].vertices[0]].a+m->vcolors[m->faces[i].vertices[1]].a);
                }
            }
            else
            {
                c_v_0_indcs = v_table[m->faces[i].vertices[0]].items[i_01+1];
            }

            i_12 = __mesh_find(&v_table[m->faces[i].vertices[1]], m->faces[i].vertices[2]);

            if(i_12<0)
            {
                ++kv;
                c_v_1_indcs = kv;
                v_table[m->faces[i].vertices[1]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[1]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[1]].num_items+2));
                v_table[m->faces[i].vertices[1]].num_items += 2;
                v_table[m->faces[i].vertices[1]].items[v_table[m->faces[i].vertices[1]].num_items-2] = m->faces[i].vertices[2];
                v_table[m->faces[i].vertices[1]].items[v_table[m->faces[i].vertices[1]].num_items-1] = c_v_1_indcs;

                v_table[m->faces[i].vertices[2]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[2]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[2]].num_items+2));
                v_table[m->faces[i].vertices[2]].num_items += 2;
                v_table[m->faces[i].vertices[2]].items[v_table[m->faces[i].vertices[2]].num_items-2] = m->faces[i].vertices[1];
                v_table[m->faces[i].vertices[2]].items[v_table[m->faces[i].vertices[2]].num_items-1] = c_v_1_indcs;

                m->vertices = (MESH_VERTEX)realloc(m->vertices, sizeof(mesh_vertex)*(kv+1));
                m->vertices[kv].x = 0.5*(m->vertices[m->faces[i].vertices[1]].x+m->vertices[m->faces[i].vertices[2]].x);
                m->vertices[kv].y = 0.5*(m->vertices[m->faces[i].vertices[1]].y+m->vertices[m->faces[i].vertices[2]].y);
                m->vertices[kv].z = 0.5*(m->vertices[m->faces[i].vertices[1]].z+m->vertices[m->faces[i].vertices[2]].z);
                if(m->is_vnormals)
                {
                    m->vnormals = (MESH_NORMAL)realloc(m->vnormals, sizeof(mesh_normal)*(kv+1));
                    m->vnormals[kv].x = m->vnormals[m->faces[i].vertices[1]].x+m->vnormals[m->faces[i].vertices[2]].x;
                    m->vnormals[kv].y = m->vnormals[m->faces[i].vertices[1]].y+m->vnormals[m->faces[i].vertices[2]].y;
                    m->vnormals[kv].z = m->vnormals[m->faces[i].vertices[1]].z+m->vnormals[m->faces[i].vertices[2]].z;
                    t = sqrt(m->vnormals[kv].x*m->vnormals[kv].x+m->vnormals[kv].y*m->vnormals[kv].y+m->vnormals[kv].z*m->vnormals[kv].z);
                    m->vnormals[kv].x /= t;
                    m->vnormals[kv].y /= t;
                    m->vnormals[kv].z /= t;
                }
                if(m->is_vcolors)
                {
                    m->vcolors = (MESH_COLOR)realloc(m->vcolors, sizeof(mesh_color)*(kv+1));
                    m->vcolors[kv].r = 0.5*(m->vcolors[m->faces[i].vertices[1]].r+m->vcolors[m->faces[i].vertices[2]].r);
                    m->vcolors[kv].g = 0.5*(m->vcolors[m->faces[i].vertices[1]].g+m->vcolors[m->faces[i].vertices[2]].g);
                    m->vcolors[kv].b = 0.5*(m->vcolors[m->faces[i].vertices[1]].b+m->vcolors[m->faces[i].vertices[2]].b);
                    m->vcolors[kv].a = 0.5*(m->vcolors[m->faces[i].vertices[1]].a+m->vcolors[m->faces[i].vertices[2]].a);
                }
            }
            else
            {
                c_v_1_indcs = v_table[m->faces[i].vertices[1]].items[i_12+1];
            }

           i_20 = __mesh_find(&v_table[m->faces[i].vertices[2]], m->faces[i].vertices[0]);

           if(i_20<0)
            {
                ++kv;
                c_v_2_indcs = kv;
                v_table[m->faces[i].vertices[2]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[2]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[2]].num_items+2));
                v_table[m->faces[i].vertices[2]].num_items += 2;
                v_table[m->faces[i].vertices[2]].items[v_table[m->faces[i].vertices[2]].num_items-2] = m->faces[i].vertices[0];
                v_table[m->faces[i].vertices[2]].items[v_table[m->faces[i].vertices[2]].num_items-1] = c_v_2_indcs;

                v_table[m->faces[i].vertices[0]].items = (INTDATA*)realloc(v_table[m->faces[i].vertices[0]].items, sizeof(INTDATA)*(v_table[m->faces[i].vertices[0]].num_items+2));
                v_table[m->faces[i].vertices[0]].num_items += 2;
                v_table[m->faces[i].vertices[0]].items[v_table[m->faces[i].vertices[0]].num_items-2] = m->faces[i].vertices[2];
                v_table[m->faces[i].vertices[0]].items[v_table[m->faces[i].vertices[0]].num_items-1] = c_v_2_indcs;

                m->vertices = (MESH_VERTEX)realloc(m->vertices, sizeof(mesh_vertex)*(kv+1));
                m->vertices[kv].x = 0.5*(m->vertices[m->faces[i].vertices[2]].x+m->vertices[m->faces[i].vertices[0]].x);
                m->vertices[kv].y = 0.5*(m->vertices[m->faces[i].vertices[2]].y+m->vertices[m->faces[i].vertices[0]].y);
                m->vertices[kv].z = 0.5*(m->vertices[m->faces[i].vertices[2]].z+m->vertices[m->faces[i].vertices[0]].z);
                if(m->is_vnormals)
                {
                    m->vnormals = (MESH_NORMAL)realloc(m->vnormals, sizeof(mesh_normal)*(kv+1));
                    m->vnormals[kv].x = m->vnormals[m->faces[i].vertices[2]].x+m->vnormals[m->faces[i].vertices[0]].x;
                    m->vnormals[kv].y = m->vnormals[m->faces[i].vertices[2]].y+m->vnormals[m->faces[i].vertices[0]].y;
                    m->vnormals[kv].z = m->vnormals[m->faces[i].vertices[2]].z+m->vnormals[m->faces[i].vertices[0]].z;
                    t = sqrt(m->vnormals[kv].x*m->vnormals[kv].x+m->vnormals[kv].y*m->vnormals[kv].y+m->vnormals[kv].z*m->vnormals[kv].z);
                    m->vnormals[kv].x /= t;
                    m->vnormals[kv].y /= t;
                    m->vnormals[kv].z /= t;
                }
                if(m->is_vcolors)
                {
                    m->vcolors = (MESH_COLOR)realloc(m->vcolors, sizeof(mesh_color)*(kv+1));
                    m->vcolors[kv].r = 0.5*(m->vcolors[m->faces[i].vertices[2]].r+m->vcolors[m->faces[i].vertices[0]].r);
                    m->vcolors[kv].g = 0.5*(m->vcolors[m->faces[i].vertices[2]].g+m->vcolors[m->faces[i].vertices[0]].g);
                    m->vcolors[kv].b = 0.5*(m->vcolors[m->faces[i].vertices[2]].b+m->vcolors[m->faces[i].vertices[0]].b);
                    m->vcolors[kv].a = 0.5*(m->vcolors[m->faces[i].vertices[2]].a+m->vcolors[m->faces[i].vertices[0]].a);
                }
            }
            else
            {
                c_v_2_indcs = v_table[m->faces[i].vertices[2]].items[i_20+1];
            }

            curr_idx = i*4;
            new_faces[curr_idx].num_vertices = 3;
            new_faces[curr_idx].vertices = (INTDATA *)malloc(3*sizeof(INTDATA));
            new_faces[curr_idx].vertices[0] = m->faces[i].vertices[0];
            new_faces[curr_idx].vertices[1] = c_v_0_indcs;
            new_faces[curr_idx].vertices[2] = c_v_2_indcs;

            ++curr_idx;
            new_faces[curr_idx].num_vertices = 3;
            new_faces[curr_idx].vertices = (INTDATA *)malloc(3*sizeof(INTDATA));
            new_faces[curr_idx].vertices[0] = c_v_0_indcs;
            new_faces[curr_idx].vertices[1] = m->faces[i].vertices[1];
            new_faces[curr_idx].vertices[2] = c_v_1_indcs;

            ++curr_idx;
            new_faces[curr_idx].num_vertices = 3;
            new_faces[curr_idx].vertices = (INTDATA *)malloc(3*sizeof(INTDATA));
            new_faces[curr_idx].vertices[0] = c_v_1_indcs;
            new_faces[curr_idx].vertices[1] = m->faces[i].vertices[2];
            new_faces[curr_idx].vertices[2] = c_v_2_indcs;

            ++curr_idx;
            new_faces[curr_idx].num_vertices = 3;
            new_faces[curr_idx].vertices = (INTDATA *)malloc(3*sizeof(INTDATA));
            new_faces[curr_idx].vertices[0] = c_v_0_indcs;
            new_faces[curr_idx].vertices[1] = c_v_1_indcs;
            new_faces[curr_idx].vertices[2] = c_v_2_indcs;
       }

        for(i=0; i<m->num_vertices; ++i)
        {
            if(v_table[i].items!=NULL) free(v_table[i].items);
        }
        free(v_table);
        if(m->is_vfaces)
        {
            mesh_calc_vertex_adjacency(m);
        }
        free(m->faces);
        m->faces = new_faces;
        m->num_faces = 4*m->num_faces;
        m->num_vertices = kv+1;
    }
    if(m->is_vfaces==1) mesh_calc_vertex_adjacency(m);
    return 0;
}


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

FLOATDATA mesh_calc_triangle_area(MESH_VERTEX a, MESH_VERTEX b, MESH_VERTEX c)
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
        if((fflags = (char *)malloc(sizeof(char)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
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
        if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(num_valid_flags))) == NULL) mesh_error(MESH_ERR_MALLOC);
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
    if((vflags = (char *)malloc(sizeof(char)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    if((vindx = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
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

    if((new_vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*(num_valid_flags))) == NULL) mesh_error(MESH_ERR_MALLOC);
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
        if((new_vcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(num_valid_flags))) == NULL) mesh_error(MESH_ERR_MALLOC);
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

    if(m->is_vcolors)
    {
        if((new_vnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(num_valid_flags))) == NULL) mesh_error(MESH_ERR_MALLOC);
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




