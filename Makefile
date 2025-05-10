win:
	g++.exe -fdiagnostics-color=always -I./include -I./include/glm ./src/main.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3dll -lopengl32 -lgdi32
	./build/main.exe

linux:
	g++ -fdiagnostics-color=always -I./include -I./include/glm ./src/main.cpp ./src/glad.c -o ./build/main -Llib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main