#include "meshlib.h"
#include <GL/gl.h>
#include <GL/glu.h>


void __mesh_calculate_vertex_normal(MESH_VERTEX v1, MESH_VERTEX v2, MESH_VERTEX v3, MESH_NORMAL n)
{
    GLfloat qx, qy, qz, px, py, pz;
    qx = v2->x-v1->x;
    qy = v2->y-v1->y;
    qz = v2->z-v1->z;
    px = v3->x-v1->x;
    py = v3->y-v1->y;
    pz = v3->z-v1->z;
    n->x = py*qz - pz*qy;
    n->y = pz*qx - px*qz;
    n->z = px*qy - py*qx;
}

void mesh_draw_mesh(MESH m)
{
    INTDATA i, j;
    mesh_normal n;
    GLfloat currcolor[4];
    if(m->is_trimesh)
    {
        if(m->is_fcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, currcolor);
                __mesh_calculate_vertex_normal(&m->vertices[m->faces[i].vertices[0]], &m->vertices[m->faces[i].vertices[1]], &m->vertices[m->faces[i].vertices[2]], &n);
                glNormal3f(n.x, n.y, n.z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
        else
        {
            glEnable(GL_COLOR_MATERIAL);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                __mesh_calculate_vertex_normal(&m->vertices[m->faces[i].vertices[0]], &m->vertices[m->faces[i].vertices[1]], &m->vertices[m->faces[i].vertices[2]], &n);
                glNormal3f(n.x, n.y, n.z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
    }
    else if(m->num_faces>0)
    {
        if(m->is_fcolors)
        {
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, currcolor);
                /* glColor4f(m->fcolors[i].r, m->fcolors[i].g, m->fcolors[i].b, m->fcolors[i].a); */
                glBegin(GL_TRIANGLE_FAN);
                for(j=0; j<m->faces[i].num_vertices; ++j)
                {
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
        }
        else
        {
            for(i=0; i<m->num_faces; ++i)
            {
                glBegin(GL_TRIANGLE_FAN);
                for(j=0; j<m->faces[i].num_vertices; ++j)
                {
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
        }
    }
}

