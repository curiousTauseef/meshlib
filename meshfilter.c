#include "meshlib.h"
#include <math.h>


int mesh_bilateral_filter(MESH m, FLOATDATA sigma_c, FLOATDATA sigma_s, int niters)
{
    FLOATDATA sum, normalizer, t, tx, ty, tz, h, wc, ws, isigmac, isigmas;
    INTDATA i, j, k, l;
    mesh_calc_vertex_adjacency(m);
    isigmac = 0.5/(sigma_c*sigma_c);
    isigmas = 0.5/(sigma_s*sigma_s);
    MESH_VERTEX est_vertices = (MESH_VERTEX)malloc(m->num_vertices*sizeof(mesh_vertex));
    if(est_vertices==NULL) mesh_error(MESH_ERR_MALLOC);
    for(l=0; l<niters; ++l)
    {
        mesh_calc_vertex_normals(m);
        for(i=0; i<m->num_vertices; ++i)
        {
            sum = 0;
            normalizer = 1e-15;
            for(j=0; j<m->vfaces[i].num_faces; ++j)
            {
                for(k=0; k<m->faces[m->vfaces[i].faces[j]].num_vertices; ++k)
                {
                    tx = (m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].x-m->vertices[i].x);
                    ty = (m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].y-m->vertices[i].y);
                    tz = (m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].z-m->vertices[i].z);
                    t = tx*tx+ty*ty+tz*tz;
                    h = tx*m->vnormals[i].x + ty*m->vnormals[i].y+tz*m->vnormals[i].z;
                    wc = exp(-t*isigmac);
                    ws = exp(-h*h*isigmas);
                    sum += wc*ws*h;
                    normalizer += wc*ws;
                }
            }
            sum /= normalizer;
            est_vertices[i].x = m->vertices[i].x + m->vnormals[i].x*sum;
            est_vertices[i].y = m->vertices[i].y + m->vnormals[i].y*sum;
            est_vertices[i].z = m->vertices[i].z + m->vnormals[i].z*sum;
        }
        for(i=0; i<m->num_vertices; ++i)
        {
            m->vertices[i].x = est_vertices[i].x;
            m->vertices[i].y = est_vertices[i].y;
            m->vertices[i].z = est_vertices[i].z;
        }
    }
    free(est_vertices);
    mesh_calc_vertex_normals(m);
    return 0;
}


int mesh_restricted_vertex_correction(MESH m, FLOATDATA r)
{
    INTDATA i, j, k;
    INTDATA t;
    FLOATDATA l, one_minus_r;
    mesh_vertex s;
    MESH_VERTEX new_vertices = NULL;
    one_minus_r = 1.0-r;
    if(!m->is_vfaces) mesh_calc_vertex_adjacency(m);
    if(!m->is_vnormals) mesh_calc_vertex_normals(m);
    if((new_vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    for(i=0; i<m->num_vertices; ++i)
    {
        s.x = 0;
        s.y = 0;
        s.z = 0;
        t = 0;
        for(j=0; j<m->vfaces[i].num_faces; ++j)
        {
            for(k=0; k<m->faces[m->vfaces[i].faces[j]].num_vertices; ++k)
            {
                if(i==m->faces[m->vfaces[i].faces[j]].vertices[k]) continue;
                ++t;
                s.x +=m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].x;
                s.y +=m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].y;
                s.z +=m->vertices[m->faces[m->vfaces[i].faces[j]].vertices[k]].z;
            }
        }
        if(t>0)
        {
            s.x /= t;
            s.y /= t;
            s.z /= t;
            l = (m->vertices[i].x-s.x)*m->vnormals[i].x + (m->vertices[i].y-s.y)*m->vnormals[i].y + (m->vertices[i].z-s.z)*m->vnormals[i].z;
            new_vertices[i].x = r*(s.x + l*m->vnormals[i].x) + one_minus_r*m->vertices[i].x;
            new_vertices[i].y = r*(s.y + l*m->vnormals[i].y) + one_minus_r*m->vertices[i].y;
            new_vertices[i].z = r*(s.z + l*m->vnormals[i].z) + one_minus_r*m->vertices[i].z;
        }
        else
        {
            new_vertices[i].x = m->vertices[i].x;
            new_vertices[i].y = m->vertices[i].y;
            new_vertices[i].z = m->vertices[i].z;
        }
    }
    free(m->vertices);
    m->vertices = new_vertices;
    mesh_calc_vertex_normals(m);
    return 0;
}


