all:  main textures

main: deecshader.o main.cpp cgraimage.o
	g++ -o main cgraimage.o deecshader.o main.cpp -I./vendor/stb_image -I./vendor -I./vendor/imgui  -lglfw -lGLEW -lGL -Wall -lrt -lm -ldl -I/opt/local/include -lnetpbm

cgraimage.o:	cgraimage.h cgraimage.cpp
	g++ -c -g -I/opt/local/include -I/usr/include/netpbm cgraimage.cpp

textures: deecshader.o textures.cpp cgraimage.o
	g++ -o textures	deecshader.o cgraimage.o textures.cpp	-I/opt/local/include -I./vendor/stb_image -I./vendor -I./vendor/imgui  -lglfw -lGLEW -lGL -Wall -lrt -lm -ldl -lnetpbm 

deecshader.o:
	g++ -c -g deecshader.cpp 

clean:
	rm -f main textures
