
build:
	gcc -Wall cube.c -o cube -lGL -lGLU -lGLEW -lm -lglfw 

clean:
	rm cube

run:
	./cube


