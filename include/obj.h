#ifndef OBG_H_
#define OBG_H_

#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>

#define TRUE  1
#define FALSE 0

typedef int BOOl;

typedef struct{
    float x,y,z,w;
} vec4f, coord, normal;

typedef struct{
    vec4f *array;
    size_t count;
} vec4f_arr, coord_arr, normal_arr;

typedef struct{
    float x,y;
} vec2f, tex_coord;

typedef struct{
    vec2f *array;
    size_t count;
} vec2f_arr, tex_coord_arr;

typedef size_t index;

typedef struct{
    index *array;
    size_t count;
} index_arr;

//arrays with reletive index arrays
typedef struct{
    coord_arr coords;
    normal_arr normals;
    tex_coord_arr tex_coords;

    index_arr coord_indexes;
    index_arr normal_indexes;
    index_arr tex_coord_indexes;
} obj_content;

//coords -> vertex array
//normals -> normal array
//tex_coords -> texture coords array
//indexes -> index array 
typedef struct{
    coord_arr coords;
    normal_arr normals;
    tex_coord_arr tex_coords;
    index_arr indexes;
} mesh;

typedef enum{
    NO_ERRORS = 0,
} OBJ_ERROR_CODE;

typedef void (*on_error)(int error, const char *log);
void set_error_callback(on_error callback);

obj_content read_obj(char *content);








#endif
