#include "../include/meshlib.h"
#include <GL/gl.h>
#include <GL/glu.h>


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
                glNormal3f(m->fnormals[i].x, m->fnormals[i].x, m->fnormals[i].y);
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
            currcolor[0] = 1.0;
            currcolor[1] = 1.0;
            currcolor[2] = 1.0;
            currcolor[3] = 1.0;

            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, currcolor);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                glNormal3f(m->fnormals[i].x, m->fnormals[i].x, m->fnormals[i].y);
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
                glBegin(GL_TRIANGLE_FAN);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    mesh_calc_face_normal(&m->vertices[m->faces[i].vertices[j-2]], &m->vertices[m->faces[i].vertices[j-1]], &m->vertices[m->faces[i].vertices[j]], &n);
                    glNormal3f(n.x, n.y, n.z);
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
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    mesh_calc_face_normal(&m->vertices[m->faces[i].vertices[j-2]], &m->vertices[m->faces[i].vertices[j-1]], &m->vertices[m->faces[i].vertices[j]], &n);
                    glNormal3f(n.x, n.y, n.z);
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
        }
    }
}

