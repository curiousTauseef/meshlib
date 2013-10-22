#include "meshlib.h"
#include <math.h>

struct _mesh_v_faces
{
    INTDATA num_faces;
    INTDATA *faces;
};
typedef struct _mesh_v_faces __mesh_v_faces;
typedef struct _mesh_v_faces * __MESH_V_FACES;


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
    __MESH_V_FACES v_faces = NULL;
    int i, j;
    FLOATDATA t;
    mesh_normal curr_normal, e1, e2, e3;

    if(m==NULL) return 1;
    if(m->is_faces==0) return 2;
    v_faces = (__MESH_V_FACES)malloc(m->num_vertices*sizeof(__mesh_v_faces));
    if(v_faces == NULL) mesh_error(MESH_ERR_MALLOC);
    for(i=0; i<m->num_vertices; ++i)
    {
        v_faces[i].num_faces = 0;
        v_faces[i].faces = NULL;
    }
    for(i=0; i<m->num_faces; ++i)
    {
        for(j=0; j<m->faces[i].num_vertices; ++j)
        {
            v_faces[m->faces[i].vertices[j]].faces = realloc(v_faces[m->faces[i].vertices[j]].faces, sizeof(INTDATA)*(v_faces[m->faces[i].vertices[j]].num_faces+1));
            v_faces[m->faces[i].vertices[j]].num_faces ++;
            v_faces[m->faces[i].vertices[j]].faces[v_faces[m->faces[i].vertices[j]].num_faces-1] = i;
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

        for(j=0; j<v_faces[i].num_faces; ++j)
        {
            e1.x = m->vertices[m->faces[v_faces[i].faces[j]].vertices[0]].x - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].x;
            e1.y = m->vertices[m->faces[v_faces[i].faces[j]].vertices[0]].y - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].y;
            e1.z = m->vertices[m->faces[v_faces[i].faces[j]].vertices[0]].z - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].z;

            e2.x = m->vertices[m->faces[v_faces[i].faces[j]].vertices[2]].x - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].x;
            e2.y = m->vertices[m->faces[v_faces[i].faces[j]].vertices[2]].y - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].y;
            e2.z = m->vertices[m->faces[v_faces[i].faces[j]].vertices[2]].z - m->vertices[m->faces[v_faces[i].faces[j]].vertices[1]].z;

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
    return 0;
}

