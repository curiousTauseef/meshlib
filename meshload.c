#include "meshlib.h"
#include <string.h>


MESH mesh_load_off(const char* fname)
{
    FILEPOINTER fp = NULL;
    MESH m = NULL;
    if((fp = fopen(fname,"rb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
    m = mesh_create_mesh_new();
    m = __mesh_parse_off_header(m, fp);
    if(m->num_vertices>0) m = __mesh_parse_off_vertices(m, fp);
    if(m->num_faces>0 && m->is_vertices) m = __mesh_parse_off_faces(m, fp);
    if(m->is_vertices) m->is_loaded = 1;
    fclose(fp);
    return m;
}

MESH __mesh_parse_off_header(MESH m, FILEPOINTER fp)
{
    char dummy[16];
    int flag;
    do
    {
        flag = mesh_read_word(fp, dummy, 16);
    } while(flag==3);
    if(flag>0) return m;
    if(strcmp(dummy, "OFF")==0) m->origin_type = MESH_ORIGIN_TYPE_OFF;
    else if(strcmp(dummy, "NOFF")==0) m->origin_type = MESH_ORIGIN_TYPE_NOFF;
    else if(strcmp(dummy, "COFF")==0) m->origin_type = MESH_ORIGIN_TYPE_COFF;
    else return m;
    m->is_loaded = 0;
    do
    {
        flag = mesh_read_word(fp, dummy, 16);
    }while(flag==3);
    if(flag>0) return m;
    m->num_vertices = strtol(dummy, NULL, 0);

    if(mesh_read_word(fp, dummy, 16)>0) return m;
    m->num_faces = strtol(dummy, NULL, 0);

    if(mesh_read_word(fp, dummy, 16)>0) return m;
    /* ignore edges */
    return m;
}

MESH __mesh_parse_off_vertices(MESH m, FILEPOINTER fp)
{
    INTDATA i;
    if(m->is_vertices) free(m->vertices);
    if((m->vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    m->is_vertices = 1;

    switch(m->origin_type)
    {
        case MESH_ORIGIN_TYPE_OFF:
        for(i=0; i<m->num_vertices; ++i)
        {
            if(fscanf(fp, " %lf %lf %lf \n", &m->vertices[i].x, &m->vertices[i].y, &m->vertices[i].z)!=3)
            {
                free(m->vertices);
                m->is_vertices = 0;
                return m;
            }
        }
        break;

        case MESH_ORIGIN_TYPE_COFF:
        if((m->vcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_vcolors = 1;
        for(i=0; i<m->num_vertices; ++i)
        {
            if(fscanf(fp, " %lf %lf %lf %lf %lf %lf %lf \n", &m->vertices[i].x, &m->vertices[i].y, &m->vertices[i].z, &m->vcolors[i].r, &m->vcolors[i].g, &m->vcolors[i].b, &m->vcolors[i].a)!=7)
            {
                free(m->vertices);
                free(m->vcolors);
                m->is_vertices = 0;
                m->is_vcolors = 0;
                return m;
            }
        }
        break;

        case MESH_ORIGIN_TYPE_NOFF:
        if((m->vnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_vnormals = 1;
        for(i=0; i<m->num_vertices; ++i)
        {
            if(fscanf(fp, " %lf %lf %lf %lf %lf %lf \n", &m->vertices[i].x, &m->vertices[i].y, &m->vertices[i].z, &m->vnormals[i].x, &m->vnormals[i].y, &m->vnormals[i].z)!=6)
            {
                free(m->vertices);
                free(m->vnormals);
                m->is_vertices = 0;
                m->is_vnormals = 0;
                return m;
            }
        }
        break;
    }

    return m;
}

MESH __mesh_parse_off_faces(MESH m, FILEPOINTER fp)
{
    INTDATA i, j, nv;
    char dummy[32];
    long int currpos;
    int nwrds = 0, nverts = 0, flag = 0;
    if(m->is_faces) free(m->faces);
    if((m->faces = (MESH_FACE)malloc(sizeof(mesh_face)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
    m->is_faces = 1;
    m->is_trimesh = 1;
    currpos = ftell(fp);
    mesh_count_words_in_line(fp, &nwrds);
    fseek(fp, currpos, SEEK_SET);
    if(fscanf(fp, "%d", &nverts)!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
    fseek(fp, currpos, SEEK_SET);
    if(nwrds==(nverts+1))
    {
        m->is_fcolors = 0;
        for(i=0; i<m->num_faces; ++i)
        {
            do
            {
                flag = mesh_read_word(fp, dummy, 32);
            } while(flag==3);
            if(flag>0)
            {
                free(m->faces);
                m->is_faces = 0;
                return m;
            }
            nv = strtol(dummy, NULL, 0);
            m->faces[i].num_vertices = nv;
            if(nv!=3) m->is_trimesh = 0;
            if((m->faces[i].vertices = (INTDATA *)malloc(sizeof(INTDATA)*nv))==NULL) mesh_error(MESH_ERR_MALLOC);
            for(j=0; j<nv; ++j)
            {
#if MESH_INTDATA_TYPE == 0
                if(fscanf(fp, "%d", &(m->faces[i].vertices[j]))!=1)
#else
                if(fscanf(fp, "%ld", &m->faces[i].vertices[j])!=1)
#endif
                {
                    free(m->faces);
                    m->is_faces = 0;
                    return m;
                }
            }
        }
    }
    else if(nwrds==(nverts+2))
    {
        if(m->is_fcolors) free(m->fcolors);
        if((m->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_faces = 1;
        m->is_fcolors = 1;
        for(i=0; i<m->num_faces; ++i)
        {
            do
            {
                flag = mesh_read_word(fp, dummy, 32);
            } while(flag==3);
            if(flag>0)
            {
                free(m->faces);
                m->is_faces = 0;
                return m;
            }
            nv = strtol(dummy, NULL, 0);
            m->faces[i].num_vertices = nv;
            if(nv!=3) m->is_trimesh = 0;
            if((m->faces[i].vertices = (INTDATA *)malloc(sizeof(INTDATA)*nv))==NULL) mesh_error(MESH_ERR_MALLOC);
            for(j=0; j<nv; ++j)
            {
    #if MESH_INTDATA_TYPE == 0
                if(fscanf(fp, "%d", &(m->faces[i].vertices[j]))!=1)
    #else
                if(fscanf(fp, "%ld", &m->faces[i].vertices[j])!=1)
    #endif
                {
                    mesh_error(MESH_ERR_SIZE_MISMATCH);
                }
            }
            if(fscanf(fp, "%lf", &(m->fcolors[i].r))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            m->fcolors[i].g = m->fcolors[i].r;
            m->fcolors[i].b = m->fcolors[i].r;
            m->fcolors[i].a = 0.0f;
        }
    }
    else if(nwrds==(nverts+4))
    {
        if(m->is_fcolors) free(m->fcolors);
        if((m->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_faces = 1;
        m->is_fcolors = 1;
        for(i=0; i<m->num_faces; ++i)
        {
            do
            {
                flag = mesh_read_word(fp, dummy, 32);
            } while(flag==3);
            if(flag>0)
            {
                free(m->faces);
                m->is_faces = 0;
                return m;
            }
            nv = strtol(dummy, NULL, 0);
            m->faces[i].num_vertices = nv;
            if(nv!=3) m->is_trimesh = 0;
            if((m->faces[i].vertices = (INTDATA *)malloc(sizeof(INTDATA)*nv))==NULL) mesh_error(MESH_ERR_MALLOC);
            for(j=0; j<nv; ++j)
            {
    #if MESH_INTDATA_TYPE == 0
                if(fscanf(fp, "%d", &(m->faces[i].vertices[j]))!=1)
    #else
                if(fscanf(fp, "%ld", &m->faces[i].vertices[j])!=1)
    #endif
                {
                    mesh_error(MESH_ERR_SIZE_MISMATCH);
                }
            }
            if(fscanf(fp, "%lf", &(m->fcolors[i].r))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            if(fscanf(fp, "%lf", &(m->fcolors[i].g))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            if(fscanf(fp, "%lf", &(m->fcolors[i].b))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            m->fcolors[i].a = 0.0f;
        }
    }
    else
    {
        if(m->is_fcolors) free(m->fcolors);
        if((m->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*(m->num_faces))) == NULL) mesh_error(MESH_ERR_MALLOC);
        m->is_faces = 1;
        m->is_fcolors = 1;
        for(i=0; i<m->num_faces; ++i)
        {
            do
            {
                flag = mesh_read_word(fp, dummy, 32);
            } while(flag==3);
            if(flag>0)
            {
                free(m->faces);
                m->is_faces = 0;
                return m;
            }
            nv = strtol(dummy, NULL, 0);
            m->faces[i].num_vertices = nv;
            if(nv!=3) m->is_trimesh = 0;
            if((m->faces[i].vertices = (INTDATA *)malloc(sizeof(INTDATA)*nv))==NULL) mesh_error(MESH_ERR_MALLOC);
            for(j=0; j<nv; ++j)
            {
    #if MESH_INTDATA_TYPE == 0
                if(fscanf(fp, "%d", &(m->faces[i].vertices[j]))!=1)
    #else
                if(fscanf(fp, "%ld", &m->faces[i].vertices[j])!=1)
    #endif
                {
                    mesh_error(MESH_ERR_SIZE_MISMATCH);
                }
            }
            if(fscanf(fp, "%lf", &(m->fcolors[i].r))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            if(fscanf(fp, "%lf", &(m->fcolors[i].g))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            if(fscanf(fp, "%lf", &(m->fcolors[i].b))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
            if(fscanf(fp, "%lf", &(m->fcolors[i].a))!=1) mesh_error(MESH_ERR_SIZE_MISMATCH);
        }
    }
    return m;
}


MESH mesh_load_xyz(const char* fname)
{
    FILEPOINTER fp = NULL;
    MESH m = NULL;
    if((fp = fopen(fname,"rb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
    m = mesh_create_mesh_new();
    m = __mesh_parse_xyz_data(m, fp);
    if(m->is_vertices) m->is_loaded = 1;
    fclose(fp);
    return m;

}

MESH __mesh_parse_xyz_data(MESH m, FILEPOINTER fp)
{
    INTDATA n1 = 0, n2 = 0, i, flag = 0, tmp = 0, k = 0;
    FLOATDATA in_value = 0;
    char c_word[100];

    while(!flag)
    {
        k = mesh_isnumeric(fp);
        if(k==1)
        {
            flag = mesh_count_words_in_line(fp, &tmp);
            if(tmp == 3) n2++; /*improved skip non-data line*/
            else if(tmp>0) mesh_error(MESH_ERR_SIZE_MISMATCH);
        }
        else if(k==0)
        {
            flag = mesh_count_words_in_line(fp, &tmp);
        }
        else flag = 1;
    }
    n1 = tmp;
    if(n1==0 || n2==0) mesh_error(MESH_ERR_SIZE_MISMATCH);
    rewind(fp);

    m->num_vertices = n2;
    if(m->is_vertices) free(m->vertices);
    if((m->vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*(m->num_vertices))) == NULL) mesh_error(MESH_ERR_MALLOC);
    m->is_vertices = 1;
    for (i=0; i<n2; ++i)
    {
        k = mesh_isnumeric(fp);
        if(k==0)
        {
            do
            {
                flag = mesh_count_words_in_line(fp, &tmp);
                k = mesh_isnumeric(fp);
            }
            while(k==0);
        }
        do
        {
            flag = mesh_read_word(fp, c_word, 100);
        } while(flag==3);
        in_value = (float)strtod(c_word, NULL);
        in_value = strtod(c_word, NULL);
        m->vertices[i].x = in_value;

        mesh_read_word(fp, c_word, 100);
        in_value = (float)strtod(c_word, NULL);
        in_value = strtod(c_word, NULL);
        m->vertices[i].y = in_value;

        mesh_read_word(fp, c_word, 100);
        in_value = (float)strtod(c_word, NULL);
        in_value = strtod(c_word, NULL);
        m->vertices[i].z = in_value;
    }
    return m;
}


MESH mesh_load_ply(const char* fname)
{
    FILEPOINTER fp = NULL;
    MESH m = NULL;
    if((fp = fopen(fname,"rb")) == NULL) mesh_error(MESH_ERR_FNOTOPEN);
    m = mesh_create_mesh_new();
    m = __mesh_parse_ply_header(m, fp);
    if(m->is_vertices) m->is_loaded = 1;
    fclose(fp);
    return m;
}

MESH __mesh_parse_ply_header(MESH m, FILEPOINTER fp)
{
    char dummy[32];
    int flag, element_done = 0;
    do
    {
        flag = mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "comment")==0)
        {
            mesh_skip_line(fp);
            flag = 3;
        }
    } while(flag==3);
    if(flag>0) return m;
    if(strcmp(dummy, "ply")==0) m->origin_type = MESH_ORIGIN_TYPE_PLY_ASCII;
    else return m;
     m->is_loaded = 0;
    do
    {
        flag = mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "comment")==0)
        {
            mesh_skip_line(fp);
            flag = 3;
        }
    } while(flag==3);
    if(flag>0) return m;
    if(strcmp(dummy, "format")==0)
    {
        flag = mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "ascii")==0) m->origin_type = MESH_ORIGIN_TYPE_PLY_ASCII;
        /*
        else if(strcmp(dummy, "binary_little_endian")==0) m->origin_type = MESH_ORIGIN_TYPE_PLY_BINARY_LITTLE_ENDIAN;
        else if(strcmp(dummy, "binary_big_endian")==0) m->origin_type = MESH_ORIGIN_TYPE_PLY_BINARY_BIG_ENDIAN;
        */
        else mesh_error(MESH_ERR_UNKNOWN);
        mesh_skip_line(fp);
    }
    else return m;
    do
    {
        flag = mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "comment")==0)
        {
            mesh_skip_line(fp);
            flag = 3;
        }
    } while(flag==3);
    if(strcmp(dummy, "element")==0)
    {
        mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "vertex")==0)
        {
            if(mesh_read_word(fp, dummy, 32)>0) return m;
            m->num_vertices = strtol(dummy, NULL, 0);
            element_done = 0;
            do
            {
                do
                {
                    flag = mesh_read_word(fp, dummy, 32);
                    if(strcmp(dummy, "element")==0) break;
                    if(strcmp(dummy, "comment")==0)
                    {
                        mesh_skip_line(fp);
                        flag = 3;
                    }
                } while(flag==3);
                if(strcmp(dummy, "element")==0) break;
                if(strcmp(dummy, "property")==0)
                {
                    mesh_read_word(fp, dummy, 32);
                    mesh_read_word(fp, dummy, 32);
                    if(strcmp(dummy, "red")==0) m->is_vcolors = 1;
                    if(strcmp(dummy, "nx")==0) m->is_vnormals = 1;
                }
                else element_done = 1;
            } while(element_done==0);
        }
    }
    if(strcmp(dummy, "element")==0)
    {
        mesh_read_word(fp, dummy, 32);
        if(strcmp(dummy, "face")==0)
        {
            if(mesh_read_word(fp, dummy, 32)>0) return m;
            m->num_faces = strtol(dummy, NULL, 0);
            element_done = 0;
            do
            {
                do
                {
                    flag = mesh_read_word(fp, dummy, 32);
                    if(strcmp(dummy, "comment")==0)
                    {
                        mesh_skip_line(fp);
                        flag = 3;
                    }
                } while(flag==3);
                if(strcmp(dummy, "property")==0)
                {
                    mesh_read_word(fp, dummy, 32);
                    if(strcmp(dummy, "red")==0) m->is_fcolors = 1;
                    if(strcmp(dummy, "nx")==0) m->is_fnormals = 1;
                    mesh_skip_line(fp);
                }
                else element_done = 1;
            } while(element_done==0);
        }
    }
    while(strcmp(dummy, "end_header")!=0)
    {
        flag = mesh_read_word(fp, dummy, 32);
        if(flag==1) mesh_error(MESH_ERR_UNKNOWN);
    }
    return m;
}

MESH __mesh_parse_ply_body(MESH m, FILEPOINTER fp)
{
    __mesh_parse_off_vertices(m, fp);
    __mesh_parse_off_faces(m, fp);
    return m;
}




