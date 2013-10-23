#include "meshlib.h"
#include <math.h>

struct _mesh_struct
{
    INTDATA num_items;
    INTDATA *items;
};
typedef struct _mesh_struct __mesh_struct;
typedef struct _mesh_struct * __MESH_STRUCT;


__inline void __mesh_cross(MESH_NORMAL x, MESH_NORMAL y, MESH_NORMAL z)
{
    FLOATDATA n;
    z->x = x->y*y->z - y->y*x->z;
    z->y = x->z*y->x - y->z*x->x;
    z->z = x->x*y->y - y->x*x->y;
    n = sqrt(z->x*z->x+z->y*z->y+z->z*z->z);
    z->x /=n;
    z->y /=n;
    z->z /=n;
}

int mesh_calculate_vertex_normals(MESH m)
{
    __MESH_STRUCT v_faces = NULL;
    INTDATA i, j;
    FLOATDATA t;
    mesh_normal curr_normal, e1, e2, e3;

    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    v_faces = (__MESH_STRUCT)malloc(m->num_vertices*sizeof(__mesh_struct));
    if(v_faces == NULL) mesh_error(MESH_ERR_MALLOC);
    for(i=0; i<m->num_vertices; ++i)
    {
        v_faces[i].num_items = 0;
        v_faces[i].items = NULL;
    }
    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            v_faces[m->faces[i].vertices[j]].items = realloc(v_faces[m->faces[i].vertices[j]].items, sizeof(INTDATA)*(v_faces[m->faces[i].vertices[j]].num_items+1));
            v_faces[m->faces[i].vertices[j]].num_items ++;
            v_faces[m->faces[i].vertices[j]].items[v_faces[m->faces[i].vertices[j]].num_items-1] = i;
        }
    }
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

        for(j=0; j<v_faces[i].num_items; ++j)
        {
            e1.x = m->vertices[m->faces[v_faces[i].items[j]].vertices[0]].x - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].x;
            e1.y = m->vertices[m->faces[v_faces[i].items[j]].vertices[0]].y - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].y;
            e1.z = m->vertices[m->faces[v_faces[i].items[j]].vertices[0]].z - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].z;

            e2.x = m->vertices[m->faces[v_faces[i].items[j]].vertices[2]].x - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].x;
            e2.y = m->vertices[m->faces[v_faces[i].items[j]].vertices[2]].y - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].y;
            e2.z = m->vertices[m->faces[v_faces[i].items[j]].vertices[2]].z - m->vertices[m->faces[v_faces[i].items[j]].vertices[1]].z;

            __mesh_cross(&e2, &e1, &e3);
            curr_normal.x +=e3.x;
            curr_normal.y +=e3.y;
            curr_normal.z +=e3.z;
        }
        t = sqrt(curr_normal.x*curr_normal.x+curr_normal.y*curr_normal.y+curr_normal.z*curr_normal.z);
        m->vnormals[i].x = curr_normal.x /t;
        m->vnormals[i].y = curr_normal.y /t;
        m->vnormals[i].z = curr_normal.z /t;
    }

    for(i=0; i<m->num_vertices; ++i)
    {
        if(v_faces[i].items!=NULL) free(v_faces[i].items);
    }
    free(v_faces);
    return 0;
}

__inline INTDATA __mesh_find(__MESH_STRUCT s, INTDATA q)
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
    __MESH_STRUCT v_table = NULL;
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
        v_table = (__MESH_STRUCT)malloc(m->num_vertices*sizeof(__mesh_struct));
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
        free(m->faces);
        m->faces = new_faces;
        m->num_faces = 4*m->num_faces;
        m->num_vertices = kv+1;
    }
    return 0;
}





