program: main.o glad.o Shader.o Mesh.o Model.o Camera.o 
	g++ main.o glad.o Shader.o Mesh.o Model.o Camera.o -lSDL2 -ldl -lassimp -o Raymarcher

main.o: main.cpp
	g++ -c main.cpp -I./include/

glad.o: ./src/glad.c ./include/glad/glad.h
	g++ -c ./src/glad.c -I./include/

Shader.o: Shader.cpp Shader.hpp
	g++ -c Shader.cpp -I./include/

Mesh.o: Mesh.cpp Mesh.hpp
	g++ -c Mesh.cpp -I./include/

Model.o: Model.cpp Model.hpp
	g++ -c Model.cpp -I./include/

Camera.o: Camera.cpp Camera.hpp
	g++ -c Camera.cpp -I./include

clean:
	rm *.o Raymarcher