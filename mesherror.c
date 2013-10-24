
#include "meshlib.h"

void mesh_error(int type)
{
    switch(type)
    {
        case MESH_ERR_MALLOC:
        fprintf(stderr, "mesh: malloc error\n");
        break;

        case MESH_ERR_SIZE_MISMATCH:
        fprintf(stderr, "mesh: dimension size mismatch\n");
        break;

        case MESH_ERR_FNOTOPEN:
        fprintf(stderr, "mesh: can not open file\n");
        break;

        default:
        fprintf(stderr, "mesh: unknown error\n");
        break;
    }
    exit(-1);
}

