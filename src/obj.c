#include "../include/obj.h"

//definitions
#define VERTEX_LINE_START "v "
#define NORMAL_VECTOR_LINE_START "vn "
#define TEXTURE_COORD_LINE_START "vt "
#define FACE_LINE_START "f "
#define COMMENT_LINE_START "#"

#define EMPTY_VEC4_STR " 0 0 0 0"
#define EMPTY_VEC4_LEN 9

//types

typedef enum{
    UNDEFINED      = 0,
    VERTEX         = 1,
    NORMAL_VECTOR  = 2,
    TEXTURE_COORD  = 3,
    FACE           = 4,
    COMMENT        = 5,
} obj_file_line_content;


//static functions

static void handle_obj_file_line(char *line, obj_content *content);
static obj_file_line_content check_obj_file_line_content(char *line);
static void handle_face_line(char* line, obj_content *content);
static void handle_texture_coord_line(char* line, obj_content *content);
static void handle_normal_vector_line(char* line, obj_content *content);
static void handle_vertex_line(char* line, obj_content *content);
static void index_arr_push(index_arr *indexes, index value);
static vec2f parse_vec2(char* text);
static vec4f parse_vec4(char* text);
static bool is_string_has_equal_start(char* first, char * second);
static bool str_contains_digits(char *beg, char *end);



//globals

static on_error on_error_callback = NULL;



bool is_string_has_equal_start(char* first, char * second){
    char *f_ptr = first;
    char *s_ptr = second;

    while (*f_ptr == *s_ptr && *f_ptr) f_ptr++, s_ptr++;

    if(*f_ptr == *s_ptr) return TRUE;
    if(*f_ptr == 0 || *s_ptr == 0) return TRUE;
    return FALSE;
}

bool str_contains_digits(char *beg, char *end){
    bool haveDigits = FALSE;
    for(char *it = beg; it <= end; it++){
        haveDigits = haveDigits && isdigit(*it);
        if(haveDigits) return TRUE;
    }
    return FALSE;
}

void index_arr_push(index_arr *indexes, index value){
    size_t arr_sz = ++indexes->count;
    indexes->array = realloc(indexes->array,arr_sz * sizeof(value));
    indexes->array[arr_sz - 1] = value;
}

vec4f parse_vec4(char* text){
    vec4f vec;
    char *buffer = malloc(strlen(text) + EMPTY_VEC4_LEN + 1);
    sprintf(buffer,"%s%s", text, EMPTY_VEC4_STR);
    sscanf(buffer,"%f %f %f %f", &vec.x, &vec.y, &vec.z, &vec.w);
    free(buffer);

    return vec;
}

vec2f parse_vec2(char* text){
    vec2f vec;
    char *buffer = malloc(strlen(text) + EMPTY_VEC4_LEN + 1);
    sprintf(buffer,"%s%s", text, EMPTY_VEC4_STR);
    sscanf(buffer,"%f %f", &vec.x, &vec.y);
    free(buffer);
    return vec;
}

void handle_vertex_line(char* line, obj_content *content){
    size_t new_len = ++content->coords.count;
    content->coords.array = realloc(content->coords.array, new_len * sizeof(vec4f));
    content->coords.array[new_len-1] = parse_vec4(line + 2);

    vec4f v = parse_vec4(line + 2);
}

void handle_normal_vector_line(char* line, obj_content *content){
    size_t new_len = ++content->normals.count;
    content->normals.array = realloc(content->normals.array,new_len * sizeof(vec4f));
    content->normals.array[new_len-1] = parse_vec4(line + 2);
}

void handle_texture_coord_line(char* line, obj_content *content){
    size_t new_len = ++content->tex_coords.count ;
    content->tex_coords.array = realloc(content->tex_coords.array,new_len * sizeof(vec2f));
    content->tex_coords.array[new_len-1] = parse_vec2(line + 3);
}

void handle_face_line(char* line, obj_content *content){
    // line: "v/vt/vn v/vt/vn v/vt/vn"
    char *prev = strstr(line, " ") + 1, *curr;
    char *coord_index_beg;
    char *text_coord_index_beg;
    char *normal_vector_beg;
    char *normal_vector_end;
    size_t arr_sz;
    bool is_not_last = TRUE;

    while(is_not_last){
        //{prev ; curr} : "v/vt/vn.." 
        curr = strstr(prev, " ");                                   //"v/vt/vn.."
        is_not_last = curr;
        coord_index_beg = prev;
        text_coord_index_beg = strstr(coord_index_beg, "/") + 1;    //"vt/vn.."
        normal_vector_beg = strstr(text_coord_index_beg, "/") + 1;  //"vn.."

        index_arr_push(&content->coord_indexes,strtoull(coord_index_beg,NULL,0));
        index_arr_push(&content->tex_coord_indexes,strtoull(text_coord_index_beg,NULL,0));
        index_arr_push(&content->normal_indexes,strtoull(normal_vector_beg,NULL,0));

        prev = curr +1;
    }
}

obj_file_line_content check_obj_file_line_content(char *line){

    if     (is_string_has_equal_start(line, NORMAL_VECTOR_LINE_START)) return NORMAL_VECTOR;
    else if(is_string_has_equal_start(line, TEXTURE_COORD_LINE_START)) return TEXTURE_COORD;
    else if(is_string_has_equal_start(line, VERTEX_LINE_START))        return VERTEX;
    else if(is_string_has_equal_start(line, FACE_LINE_START))          return FACE;
    else if(is_string_has_equal_start(line, COMMENT_LINE_START))       return COMMENT;
    
    else return UNDEFINED;
}

void handle_obj_file_line(char *line, obj_content *content){
    obj_file_line_content content_type;
    content_type = check_obj_file_line_content(line);

    switch (content_type)
    {
    case VERTEX:
        handle_vertex_line(line, content);
        break;
    case NORMAL_VECTOR:
        handle_normal_vector_line(line, content);
        break;
    case TEXTURE_COORD:
        handle_texture_coord_line(line, content);
        break;
    case FACE:
        handle_face_line(line, content);
        break;
    case COMMENT:
        break;
    case UNDEFINED:
        printf("%s \"%s\"\n","unhandled line:",line);
        break;
    }
}

//header functions
void set_error_callback(on_error callback){
    on_error_callback = callback;
}

obj_content read_obj(char *content){
    obj_content result = {0};
    char *line = NULL;
    char *content_ptr = content;
    size_t line_len = 0;
    
    clear_obj(&result);
    while(*content_ptr){
        if(*content_ptr == '\n'){
            line = realloc(line,++line_len);
            line[line_len - 1] = 0;
            handle_obj_file_line(line,&result);
            line_len = 0;
            while(*++content_ptr == '\n');
        }
        else{
            line = realloc(line,++line_len);
            line[line_len - 1] = *content_ptr;
            content_ptr ++;
        }
    }
    
    free(line);
    return result;
}

void clear_obj(obj_content *object){
    free(object->coords.array);
    free(object->normals.array);
    free(object->tex_coords.array);
    free(object->coord_indexes.array);
    free(object->normal_indexes.array);
    free(object->tex_coord_indexes.array);
    object->coords.count = 0;
    object->normals.count = 0;
    object->tex_coords.count = 0;
    object->coord_indexes.count = 0;
    object->normal_indexes.count = 0;
    object->tex_coord_indexes.count = 0;
}
