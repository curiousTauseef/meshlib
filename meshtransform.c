#include "meshlib.h"

MESH_ROTATION mesh_rotation_create()
{
    MESH_ROTATION r;
    if((r = (MESH_ROTATION)malloc(sizeof(mesh_rotation)))==NULL) mesh_error();
    if((r->data = (FLOATDATA *)malloc(sizeof(FLOATDATA)*9))==NULL) mesh_error();
    return r;
}

int mesh_rotation_free(MESH_ROTATION r)
{
    free(r->data);
    free(r);
}

MESH_ROTATION mesh_rotation_set_matrix(FLOATDATA *mat, MESH_ROTATION r)
{
    int k;
    if(r==NULL) r = mesh_rotation_create();
    for(k=0; k<9; ++k) r->data[k] = mat[k];
    return r;
}

MESH_ROTATION mesh_rotation_set_angleaxis(FLOATDATA angle, MESH_NORMAL axis, MESH_ROTATION r)
{
    FLOATDATA c, s, tmp0, tmp1, tmp2;
    if(r==NULL) r = mesh_rotation_create();
    c = cos(angle);
    s = sin(angle);
    tmp0 = 1-c;
    r->data[0] = c+axis.x*axis.x*tmp0;
    r->data[4] = c+axis.y*axis.y*tmp0;
    r->data[8] = c+axis.z*axis.z*tmp0;

    tmp1 = axis.x*axis.y*tmp0;
    tmp2 = axis.z*s;
    r->data[3] = tmp1+tmp2;
    r->data[1] = tmp1-tmp2;
    tmp1 = axis.x*axis.z*tmp0;
    tmp2 = axis.y*s;
    r->data[6] = tmp1-tmp2;
    r->data[2] = tmp1+tmp2;
    tmp1 = axis.y*axis.z*tmp0;
    tmp2 = axis.x*s;
    r->data[7] = tmp1+tmp2;
    r->data[5] = tmp1-tmp2;
    return r;
}

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

int mesh_translate_vertex(MESH m, MESH_VERTEX v)
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

int mesh_scale(MESH m, FLOATDATA sx, FLOATDATA sy, FLOATDATA sz)
{
    INTDATA i;
    if(m==NULL) return 1;
    for(i=0; i<m->num_vertices; ++i)
    {
        m->vertices[i].x *= sx;
        m->vertices[i].y *= sy;
        m->vertices[i].z *= sz;
    }
    return 0;
}


int mesh_vertex_rotate(MESH_VERTEX v, MESH_ROTATION r)
{
    FLOATDATA x, y, z;
    x = r->data[0]*v->x+r->data[1]*v->y+r->data[2]*v->z;
    y = r->data[3]*v->x+r->data[4]*v->y+r->data[5]*v->z;
    z = r->data[6]*v->x+r->data[7]*v->y+r->data[8]*v->z;
    
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

int mesh_rotate(MESH m, MESH_ROTATION r)
{
    INTDATA i;
    FLOATDATA x, y, z;
    if(m==NULL) return 1;
    for(i=0; i<m->num_vertices; ++i)
    {
        x = r->data[0]*m->vertices[i].x+r->data[1]*m->vertices[i].y+r->data[2]*m->vertices[i].z;
        y = r->data[3]*m->vertices[i].x+r->data[4]*m->vertices[i].y+r->data[5]*m->vertices[i].z;
        z = r->data[6]*m->vertices[i].x+r->data[7]*m->vertices[i].y+r->data[8]*m->vertices[i].z;
        
        m->vertices[i].x = x;
        m->vertices[i].y = y;
        m->vertices[i].z = z;
    }
    return 0;
}

