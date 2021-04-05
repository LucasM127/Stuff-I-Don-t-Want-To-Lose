CC=g++ -g -Wall
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
SFMLDIR=/home/lucas/Headers/SFML-2.5.1
LPATH=$(SFMLDIR)/lib/
IPATH=$(SFMLDIR)/include/
LINK= -Wl,-rpath=$(LPATH) -L $(LPATH) $(LIBS)
OBJECTS=main.o ConeSprayer.o

test: $(OBJECTS)
	g++ -g -Wall $(OBJECTS) -o test $(LINK)

main.o: main.cpp
	g++ -g -Wall main.cpp -c -I $(IPATH)

ConeSprayer.o: ConeSprayer.cpp ConeSprayer.hpp
	g++ -g -Wall ConeSprayer.cpp -c -I $(IPATH)

clean:
	rm *.o