SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
INC=-I $(SFML)/include
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib

test: main.cpp delaunay.cpp delaunay.hpp
	g++ -g -Wall main.cpp delaunay.cpp ../../SFML_Functions/SFUtilLib.a -o test $(INC) $(LINK)