SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics -lGL
INC=-I $(SFML)/include -I /home/lucas/Headers/imgui/ -I /home/lucas/Headers/imgui-sfml
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib

test: main.o PerlinNoise.o
	g++ -g -Wall main.o PerlinNoise.o imguiLib.a -o test $(LINK)

main.o: main.cpp
	g++ -g -c main.cpp $(INC)

PerlinNoise.o:
	g++ -g -c PerlinNoise.cpp