#include "../include/obj.h"

//definitions
#define VERTEX_LINE_START "v "
#define NORMAL_VECTOR_LINE_START "vn "
#define TEXTURE_COORD_LINE_START "vt "
#define FACE_LINE_START "f "
#define COMMENT_LINE_START "#"

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
obj_file_line_content check_obj_file_line_content(char *line){

    if     (strcmp(line, NORMAL_VECTOR_LINE_START)) return NORMAL_VECTOR;
    else if(strcmp(line, TEXTURE_COORD_LINE_START)) return TEXTURE_COORD;
    else if(strcmp(line, VERTEX_LINE_START))        return VERTEX;
    else if(strcmp(line, FACE_LINE_START))          return FACE;
    else if(strcmp(line, COMMENT_LINE_START))       return COMMENT;
    
    else return UNDEFINED;
}

void handle_obj_file_line(char *line, obj_content *content){
    obj_file_line_content content_type;

    content_type = check_obj_file_line_content(content);
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
    
    while(content_ptr){
        if(*content_ptr == '\n'){
            handle_obj_file_line(line,&result);
            line_len = 0;
        }
        else{
            line = realloc(line,++line_len);
            line[line_len - 1] = *content_ptr;
        }
        content_ptr++;
    }
    
    free(line);
    return result;
}

