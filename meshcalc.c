#include <string.h>
#include "meshlib.h"

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
    INTDATA i, j, in0, in1, in2;
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
            in0 = m->faces[m->vfaces[i].faces[j]].vertices[0];
            in1 = m->faces[m->vfaces[i].faces[j]].vertices[1];
            in2 = m->faces[m->vfaces[i].faces[j]].vertices[2];
            e1.x = m->vertices[in0].x - m->vertices[in1].x;
            e1.y = m->vertices[in0].y - m->vertices[in1].y;
            e1.z = m->vertices[in0].z - m->vertices[in1].z;

            e2.x = m->vertices[in2].x - m->vertices[in1].x;
            e2.y = m->vertices[in2].y - m->vertices[in1].y;
            e2.z = m->vertices[in2].z - m->vertices[in1].z;

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
        else
        {
            m->vnormals[i].x = 0.0;
            m->vnormals[i].y = 0.0;
            m->vnormals[i].z = 0.0;
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

int mesh_remove_zero_area_faces(MESH m)
{
    char *fflags = NULL;
    MESH_FACE new_faces = NULL;
    MESH_COLOR new_fcolors = NULL;
    MESH_NORMAL new_fnormals = NULL;
    INTDATA i, j, num_deleted = 0;
    if(m->is_trimesh)
    {
        if((fflags = (char *)malloc(sizeof(char)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
        memset(fflags, 0, sizeof(char)*(m->num_faces));

        for(i=0; i<m->num_faces; ++i)
        {
            if(mesh_calc_triangle_area(&(m->vertices[m->faces[i].vertices[0]]),&(m->vertices[m->faces[i].vertices[1]]),&(m->vertices[m->faces[i].vertices[2]]))<1e-16)
            {
                fflags[i]= 1;
                ++num_deleted;
            }
        }
        if(num_deleted>0)
        {

            if((new_faces = (MESH_FACE)malloc(sizeof(mesh_face)*(m->num_faces-num_deleted))) == NULL) mesh_error(MESH_ERR_MALLOC);
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
                if((new_fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces-num_deleted))) == NULL) mesh_error(MESH_ERR_MALLOC);
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
                if((new_fnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_faces-num_deleted))) == NULL) mesh_error(MESH_ERR_MALLOC);
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

int mesh_remove_close_vertices(MESH m, FLOATDATA r)
{
    INTDATA i, j, k;
    INTDATA *vparents = NULL;
    INTDATA *vranks = NULL;
    if(!m->is_vfaces) mesh_calc_vertex_adjacency(m);
    if((vparents = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    if((vranks = (INTDATA *)malloc(sizeof(INTDATA)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
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

