#include "../include/obj.h"

//definitions
#define VERTEX_LINE_START "v "
#define NORMAL_VECTOR_LINE_START "vn "
#define TEXTURE_COORD_LINE_START "vt "
#define FACE_LINE_START "f "
#define COMMENT_LINE_START "#"

#define EMPTY_VEC4_STR " 0 0 0 0"
#define EMPTY_VEC4_LEN 9
#define MAX_INDEX_STR_LEN 10


//types
typedef enum{
    UNDEFINED      = 0,
    VERTEX         = 1,
    NORMAL_VECTOR  = 2,
    TEXTURE_COORD  = 3,
    FACE           = 4,
    COMMENT        = 5,
} obj_file_line_content;

//globals
static on_error on_error_callback = NULL;

//local functions
bool is_string_has_equal_start(char* first, char * second){
    char *f_ptr = first;
    char *s_ptr = second;

    while (*f_ptr == *s_ptr && *f_ptr) f_ptr++, s_ptr++;

    if(*f_ptr == *s_ptr) return TRUE;
    if(*f_ptr == 0 || *s_ptr == 0) return TRUE;
    return FALSE;
}

obj_file_line_content check_obj_file_line_content(char *line){

    if     (is_string_has_equal_start(line, NORMAL_VECTOR_LINE_START)) return NORMAL_VECTOR;
    else if(is_string_has_equal_start(line, TEXTURE_COORD_LINE_START)) return TEXTURE_COORD;
    else if(is_string_has_equal_start(line, VERTEX_LINE_START))        return VERTEX;
    else if(is_string_has_equal_start(line, FACE_LINE_START))          return FACE;
    else if(is_string_has_equal_start(line, COMMENT_LINE_START))       return COMMENT;
    
    else return UNDEFINED;
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

bool str_contains_digits(char *beg, char *end){
    bool haveDigits = FALSE;
    for(char *it = beg; it <= end; it++){
        haveDigits = haveDigits && isdigit(*it);
        if(haveDigits) return TRUE;
    }
    return FALSE;
}

void handle_face_line(char* line, obj_content *content){
    char *prev = strstr(line, " ") + 1, *curr;

    char *coord_index_beg;
    char *text_coord_index_beg;
    char *normal_vector_beg;
    char *normal_vector_end;
    size_t arr_sz;
    bool is_not_last = TRUE;

    while(is_not_last){
        curr = strstr(prev, " ");
        is_not_last = curr;
        // "coord_index/text_coord_index/normal_vector_index"
        coord_index_beg = prev;
        text_coord_index_beg = strstr(coord_index_beg, "/") + 1;
        normal_vector_beg = strstr(text_coord_index_beg, "/") + 1;
        char *buffer = malloc(MAX_INDEX_STR_LEN);

        arr_sz = ++content->coord_indexes.count;
        content->coord_indexes.array = realloc(content->coord_indexes.array,arr_sz * sizeof(index));
        content->coord_indexes.array[arr_sz - 1] = strtoull(coord_index_beg,NULL,0);

        arr_sz = ++content->tex_coord_indexes.count;
        content->tex_coord_indexes.array = realloc(content->tex_coord_indexes.array,arr_sz * sizeof(index));
        content->tex_coord_indexes.array[arr_sz - 1] = strtoull(text_coord_index_beg,NULL,0);

        arr_sz = ++content->normal_indexes.count;
        content->normal_indexes.array = realloc(content->normal_indexes.array,arr_sz * sizeof(index));
        content->normal_indexes.array[arr_sz - 1] = strtoull(normal_vector_beg,NULL,0);

        printf("%ld,%ld,%ld\n",content->coord_indexes.array[content->coord_indexes.count - 1],
                                content->tex_coord_indexes.array[content->tex_coord_indexes.count - 1],
                                content->normal_indexes.array[content->normal_indexes.count - 1]);

        prev = curr +1;
    }


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
