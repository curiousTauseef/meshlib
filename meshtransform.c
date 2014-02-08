#include "meshlib.h"

int mesh_translate(MESH m, FLOATDATA x, FLOATDATA y, FLOATDATA z)
{
    INTDATA i;
    if(m==NULL) return 1;
    for(i=0; i<m->num_vertices; ++i)
    {
        m->vertices[i].x += x;
        m->vertices[i].y += y;
        m->vertices[i].z += z;
    }
    return 0;
}

int mesh_translatev(MESH m, MESH_VERTEX v)
{
    INTDATA i;
    if(m==NULL) return 1;
    for(i=0; i<m->num_vertices; ++i)
    {
        m->vertices[i].x += v->x;
        m->vertices[i].y += v->y;
        m->vertices[i].z += v->z;
    }
    return 0;
}

int mesh_scale(MESH m, FLOATDATA s)
{
    INTDATA i;
    if(m==NULL) return 1;
    for(i=0; i<m->num_vertices; ++i)
    {
        m->vertices[i].x *= s;
        m->vertices[i].y *= s;
        m->vertices[i].z *= s;
    }
    return 0;
}


int mesh_rotate(MESH m)
{
    return 0;
}

