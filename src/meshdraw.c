/**
 * @file meshdraw.c
 * @author Sk. Mohammadul Haque
 * @version 1.4.0.0
 * @copyright
 * Copyright (c) 2013, 2014, 2015 Sk. Mohammadul Haque.
 * @brief This file contains functions pertaining to mesh drawing in OpenGL.
 */

#include "../include/meshlib.h"
#include <GL/gl.h>
#include <GL/glu.h>

/** \brief Draws a given mesh in OpenGL context in flat shading
 *
 * \param[in] m Input mesh
 * \return NULL
 *
 */

void mesh_draw_mesh(MESH m)
{
    INTDATA i, j;
    mesh_normal n;
    GLfloat currcolor[4];
    if(!m->is_fnormals) mesh_calc_face_normals(m);
    if(m->is_trimesh)
    {
        if(m->is_fcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
                glNormal3f(m->fnormals[i].x, m->fnormals[i].y, m->fnormals[i].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
        else if(m->is_vcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].r+m->vcolors[m->faces[i].vertices[1]].r+m->vcolors[m->faces[i].vertices[2]].r);
                currcolor[1] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].g+m->vcolors[m->faces[i].vertices[1]].g+m->vcolors[m->faces[i].vertices[2]].g);
                currcolor[2] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].b+m->vcolors[m->faces[i].vertices[1]].b+m->vcolors[m->faces[i].vertices[2]].b);
                currcolor[3] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].a+m->vcolors[m->faces[i].vertices[1]].a+m->vcolors[m->faces[i].vertices[2]].a);

                glColor3fv(currcolor);
                glNormal3f(m->fnormals[i].x, m->fnormals[i].y, m->fnormals[i].z);
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

            glColor3fv(currcolor);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                glNormal3f(m->fnormals[i].x, m->fnormals[i].y, m->fnormals[i].z);
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
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
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
                glDisable(GL_COLOR_MATERIAL);
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


/** \brief Draws a given mesh in OpenGL context in smoothing shading
 *
 * \param[in] m Input mesh
 * \return NULL
 *
 */

void mesh_draw_mesh_smooth(MESH m)
{
    INTDATA i, j;
    GLfloat currcolor[4];
    if(!m->is_vnormals) mesh_calc_vertex_normals(m);
    if(m->is_trimesh)
    {
        if(m->is_fcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
                glNormal3f(m->vnormals[m->faces[i].vertices[0]].x, m->vnormals[m->faces[i].vertices[0]].y, m->vnormals[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[1]].x, m->vnormals[m->faces[i].vertices[1]].y, m->vnormals[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[2]].x, m->vnormals[m->faces[i].vertices[2]].y, m->vnormals[m->faces[i].vertices[2]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
        else if(m->is_vcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].r+m->vcolors[m->faces[i].vertices[1]].r+m->vcolors[m->faces[i].vertices[2]].r);
                currcolor[1] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].g+m->vcolors[m->faces[i].vertices[1]].g+m->vcolors[m->faces[i].vertices[2]].g);
                currcolor[2] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].b+m->vcolors[m->faces[i].vertices[1]].b+m->vcolors[m->faces[i].vertices[2]].b);
                currcolor[3] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].a+m->vcolors[m->faces[i].vertices[1]].a+m->vcolors[m->faces[i].vertices[2]].a);

                glColor3fv(currcolor);
                glNormal3f(m->vnormals[m->faces[i].vertices[0]].x, m->vnormals[m->faces[i].vertices[0]].y, m->vnormals[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[1]].x, m->vnormals[m->faces[i].vertices[1]].y, m->vnormals[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[2]].x, m->vnormals[m->faces[i].vertices[2]].y, m->vnormals[m->faces[i].vertices[2]].z);
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

            glColor3fv(currcolor);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                glNormal3f(m->vnormals[m->faces[i].vertices[0]].x, m->vnormals[m->faces[i].vertices[0]].y, m->vnormals[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[1]].x, m->vnormals[m->faces[i].vertices[1]].y, m->vnormals[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[2]].x, m->vnormals[m->faces[i].vertices[2]].y, m->vnormals[m->faces[i].vertices[2]].z);
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
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_DIFFUSE);
            for(i=0; i<m->num_faces; ++i)
            {
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
                glBegin(GL_TRIANGLE_FAN);
                glNormal3f(m->vnormals[m->faces[i].vertices[0]].x, m->vnormals[m->faces[i].vertices[0]].y, m->vnormals[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[1]].x, m->vnormals[m->faces[i].vertices[1]].y, m->vnormals[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    glNormal3f(m->vnormals[m->faces[i].vertices[j]].x, m->vnormals[m->faces[i].vertices[j]].y, m->vnormals[m->faces[i].vertices[j]].z);
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
                glDisable(GL_COLOR_MATERIAL);
            }
        }
        else
        {
            for(i=0; i<m->num_faces; ++i)
            {
                glBegin(GL_TRIANGLE_FAN);
                glNormal3f(m->vnormals[m->faces[i].vertices[0]].x, m->vnormals[m->faces[i].vertices[0]].y, m->vnormals[m->faces[i].vertices[0]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(m->vnormals[m->faces[i].vertices[1]].x, m->vnormals[m->faces[i].vertices[1]].y, m->vnormals[m->faces[i].vertices[1]].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    glNormal3f(m->vnormals[m->faces[i].vertices[j]].x, m->vnormals[m->faces[i].vertices[j]].y, m->vnormals[m->faces[i].vertices[j]].z);
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
        }
    }
}

