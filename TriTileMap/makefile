CC=g++ -g -Wall
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
SFMLDIR=/home/lucas/Headers/SFML-2.5.1
LPATH=$(SFMLDIR)/lib/
IPATH=$(SFMLDIR)/include/
LINK= -Wl,-rpath=$(LPATH) -L $(LPATH) $(LIBS)
OBJECTS=main.o MapEditor.o TriGridMap.o

test: $(OBJECTS)
	g++ -g -Wall $(OBJECTS) -o test $(LINK)

main.o: main.cpp
	g++ -g -Wall main.cpp -c -I $(IPATH)

MapEditor.o: MapEditor.cpp MapEditor.hpp TriGridMap.o
	g++ -g -Wall MapEditor.cpp -c -I $(IPATH)

TriGridMap.o: TriGridMap.cpp TriGridMap.hpp
	g++ -g -Wall TriGridMap.cpp -c -I $(IPATH)

clean:
	rm *.o