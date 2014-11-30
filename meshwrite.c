#include <string.h>
#include "meshlib.h"

int mesh_write_file(MESH m, const char* fname)
{
    char *ext = strrchr(fname, '.');
    if(strcmp(ext,".off")==0) return mesh_write_off(m, fname);
    else if(strcmp(ext,".ply")==0) return mesh_write_ply(m, fname);
    else if(strcmp(ext,".asc")==0) return mesh_write_xyz(m, fname);
    else if(strcmp(ext,".xyz")==0) return mesh_write_xyz(m, fname);
    return -1;
}

int mesh_write_off(MESH m, const char* fname)
{
    INTDATA i, j;
    if(m->is_vertices)
    {
        FILEPOINTER fp = NULL;
        if((fp = fopen(fname,"wb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
        if(m->is_vcolors)
        {
            fprintf(fp, "COFF\n");
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "%d %d 0\n", m->num_vertices, m->num_faces);
#else
            fprintf(fp, "%ld %ld 0\n", m->num_vertices, m->num_faces);
#endif
            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g %g %g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z, m->vcolors[i].r,  m->vcolors[i].g, m->vcolors[i].b, m->vcolors[i].a);
            }
        }
        else if(m->is_vnormals)
        {
            fprintf(fp, "NOFF\n");
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "%d %d 0\n", m->num_vertices, m->num_faces);
#else
            fprintf(fp, "%ld %ld 0\n", m->num_vertices, m->num_faces);
#endif
            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g %g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z, m->vnormals[i].x,  m->vnormals[i].y, m->vnormals[i].z);
            }
        }
        else
        {
            fprintf(fp, "OFF\n");
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "%d %d 0\n", m->num_vertices, m->num_faces);
#else
            fprintf(fp, "%ld %ld 0\n", m->num_vertices, m->num_faces);
#endif
            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z);
            }
        }
        if(m->is_faces)
            for(i=0; i<m->num_faces; ++i)
            {
#if MESH_INTDATA_TYPE==0
                fprintf(fp, "%d", m->faces[i].num_vertices);
#else
                fprintf(fp, "%ld", m->faces[i].num_vertices);
#endif
                for(j=0; j<m->faces[i].num_vertices; ++j)
                {
#if MESH_INTDATA_TYPE==0
                    fprintf(fp, " %d", m->faces[i].vertices[j]);
#else
                    fprintf(fp, " %ld", m->faces[i].vertices[j]);
#endif
                }
                fprintf(fp, "\n");
            }
        fclose(fp);
    }
    return 0;
}

int mesh_write_xyz(MESH m, const char* fname)
{
    INTDATA i;
    if(m->is_vertices)
    {
        FILEPOINTER fp = NULL;
        if((fp = fopen(fname,"wb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
        for(i=0; i<m->num_vertices; ++i)
        {
            fprintf(fp, "%g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z);
        }
        fclose(fp);
    }
    return 0;
}

int mesh_write_ply(MESH m, const char* fname)
{
    INTDATA i, j;
    if(m->is_vertices)
    {
        FILEPOINTER fp = NULL;
        if((fp = fopen(fname,"wb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
        fprintf(fp, "ply\n");
        fprintf(fp, "format ascii 1.0\n");
        if(m->is_vcolors)
        {
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element vertex %d\n", m->num_vertices);
#else
            fprintf(fp, "element vertex %ld\n", m->num_vertices);
#endif
            fprintf(fp, "property float x\n");
            fprintf(fp, "property float y\n");
            fprintf(fp, "property float z\n");
            fprintf(fp, "property float red\n");
            fprintf(fp, "property float green\n");
            fprintf(fp, "property float blue\n");
            fprintf(fp, "property float alpha\n");

#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element face %d\n", m->num_faces);
#else
            fprintf(fp, "element face %ld\n", m->num_faces);
#endif
            fprintf(fp, "property list uchar int vertex_index\n");
            fprintf(fp, "end_header\n");

            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g %g %g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z, m->vcolors[i].r,  m->vcolors[i].g, m->vcolors[i].b, m->vcolors[i].a);
            }
        }
        else if(m->is_vnormals)
        {
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element vertex %d\n", m->num_vertices);
#else
            fprintf(fp, "element vertex %ld\n", m->num_vertices);
#endif
            fprintf(fp, "property float x\n");
            fprintf(fp, "property float y\n");
            fprintf(fp, "property float z\n");
            fprintf(fp, "property float nx\n");
            fprintf(fp, "property float ny\n");
            fprintf(fp, "property float nz\n");

#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element face %d\n", m->num_faces);
#else
            fprintf(fp, "element face %ld\n", m->num_faces);
#endif
            fprintf(fp, "property list uchar int vertex_index\n");
            fprintf(fp, "end_header\n");


            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g %g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z, m->vnormals[i].x,  m->vnormals[i].y, m->vnormals[i].z);
            }
        }
        else
        {
#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element vertex %d\n", m->num_vertices);
#else
            fprintf(fp, "element vertex %ld\n", m->num_vertices);
#endif
            fprintf(fp, "property float x\n");
            fprintf(fp, "property float y\n");
            fprintf(fp, "property float z\n");

#if MESH_INTDATA_TYPE==0
            fprintf(fp, "element face %d\n", m->num_faces);
#else
            fprintf(fp, "element face %ld\n", m->num_faces);
#endif
            fprintf(fp, "property list uchar int vertex_index\n");
            fprintf(fp, "end_header\n");

            for(i=0; i<m->num_vertices; ++i)
            {
                fprintf(fp, "%g %g %g\n", m->vertices[i].x, m->vertices[i].y, m->vertices[i].z);
            }
        }
        if(m->is_faces)
            for(i=0; i<m->num_faces; ++i)
            {
#if MESH_INTDATA_TYPE==0
                fprintf(fp, "%d", m->faces[i].num_vertices);
#else
                fprintf(fp, "%ld", m->faces[i].num_vertices);
#endif
                for(j=0; j<m->faces[i].num_vertices; ++j)
                {
#if MESH_INTDATA_TYPE==0
                    fprintf(fp, " %d", m->faces[i].vertices[j]);
#else
                    fprintf(fp, " %ld", m->faces[i].vertices[j]);
#endif
                }
                fprintf(fp, "\n");
            }
        fclose(fp);
    }
    return 0;
}

