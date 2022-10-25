all:  main

main: deecshader.o main.cpp
	g++ -o main deecshader.o main.cpp -I./vendor/stb_image -I./vendor -I./vendor/imgui  -lglfw -lGLEW -lGL -Wall -lrt -lm -ldl


deecshader.o:
	g++ -c -g deecshader.cpp 

clean:
	rm -f main