#include "../include/obj.h"
#include<locale.h>

#define TEST_FILE "./cube.obj"

int main(int argc, char* argv[])
{

    
    FILE *file = fopen(TEST_FILE,"r");
    if(file == NULL)
    {
        fprintf(stderr, "file \"%s\" is not found", TEST_FILE);
        return -1;
    }

    size_t file_size;
    char *buffer;
    
    fseek(file,0,SEEK_END);
    file_size = ftell(file);
    fseek(file,0,SEEK_SET);
    buffer = malloc(file_size+1);
    memset(buffer,0,file_size+1);
    
    fread(buffer,file_size,1,file);
    obj_content c = read_obj(buffer);
    fclose(file);
    
    printf("%ld\n",c.coords.count);
    
    free(buffer);
    return 0;
}