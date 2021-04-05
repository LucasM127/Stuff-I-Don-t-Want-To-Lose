SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
INC=-I $(SFML)/include
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib

test: main.o
	g++ -g -Wall main.o ../../SFML_Functions/SFMLCameraController.o -o test $(LINK)

main.o: main.cpp
	g++ -g -Wall main.cpp -c $(INC)