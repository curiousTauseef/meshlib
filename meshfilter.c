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
