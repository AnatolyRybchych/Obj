CC = gcc
out = run


run: objects/main.o objects/obj.o
	gcc -g -o $@ $^

objects/main.o: src/main.c
	@mkdir -p ./objects
	gcc -c -g -o $@ $^

objects/obj.o: src/obj.c
	@mkdir -p ./objects
	gcc -c -g -o $@ $^


