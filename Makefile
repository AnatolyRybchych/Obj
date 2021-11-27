CC = gcc
out = ./build/out/run.exe
linkArgs = -g
compileArgs = -c -g
OBJDIR = ./build/objects/

SRC_TYPE = .c
OBJ_TYPE = .o
SRC_TYPE_GREP = '\$(SRC_TYPE)'

#src dirs
SRC_DIR  						=	./src/

#ALL SOURCE FILES SHOULD HAVE UNIQUE NAME
#src files
src   							=	$(shell ls -r $(SRC_DIR) | grep $(SRC_TYPE_GREP))

#src files grouped 
srcFiles =   $(src)

#object files from src files
objects = $(patsubst %,$(OBJDIR)%,$(srcFiles:$(SRC_TYPE)=$(OBJ_TYPE)))

#link all objects together
build: $(objects) 
	@mkdir -p $(dir $(out))
	$(CC) $(linkArgs) -o $(out) $+ $(additionalLibs) -g


#compile all objects
$(OBJDIR)%$(OBJ_TYPE):$(SRC_DIR)$(notdir %$(SRC_TYPE))							#./src/
	$(CC) $(compileArgs) -o $@ $+

gdb: build
	GDB $(out) 

run: build
	$(out)
