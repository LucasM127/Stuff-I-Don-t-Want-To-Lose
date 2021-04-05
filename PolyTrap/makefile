SFMLPATH=/home/lucas/Headers/SFML-2.5.1
INCSFML=-I $(SFMLPATH)/include
LINKSFML=-Wl,-rpath=$(SFMLPATH)/lib -L $(SFMLPATH)/lib -lsfml-system -lsfml-window -lsfml-graphics
CC = g++ -g -Wall
CCSF = $(CC) $(INCSFML)

test: main.o Polygon.o SFPoly.o PolyTriangulator.o PolyLoader.o MonotoneTriangulator.o
	$(CC) *.o -o test $(LINKSFML)

main.o: main.cpp
	$(CCSF) -c main.cpp

Polygon.o: Polygon.cpp Polygon.hpp
	$(CCSF) -c Polygon.cpp

SFPoly.o: SFPoly.cpp SFPoly.hpp
	$(CCSF) -c SFPoly.cpp

PolyTriangulator.o: PolyTriangulator.cpp PolyTriangulator.hpp
	$(CCSF) -c PolyTriangulator.cpp

MonotoneTriangulator.o: MonotoneTriangulator.cpp PolyTriangulator.hpp
	$(CCSF) -c MonotoneTriangulator.cpp

PolyLoader.o: PolyLoader.cpp PolyLoader.hpp
	$(CCSF) -c PolyLoader.cpp

clean:
	rm *.o
