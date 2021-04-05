LIBS=-lsfml-system-d -lsfml-window-d -lsfml-graphics-d
CC=g++ -g -Wall
SFMLDIR=/home/lucas/Headers/SFML-2.5.1
IDIR=$(SFMLDIR)/include/
LDIR=$(SFMLDIR)/lib/
LINK=-Wl,-rpath=$(LDIR) -L $(LDIR) $(LIBS)
OBJECTS=main.o LineGenerator.o ../SFML_Functions/SFUtilLib.a ../Perlin_Noise/PerlinNoise.o ../sfml/GUI/QuickSFGui.o LineOptimizations.o
DEF=-D LOG_DEBUG_STDOUT

test: $(OBJECTS)
	$(CC) $(OBJECTS) -o test $(LINK)

main.o: main.cpp
	$(CC) main.cpp -c -I $(IDIR) $(DEF)

LineGenerator.o: LineGenerator.cpp LineGenerator.hpp
	$(CC) LineGenerator.cpp -c -I $(IDIR) $(DEF)

LineOptimizations.o: LineOptimizations.cpp LineOptimizations.hpp
	$(CC) LineOptimizations.cpp -c -I $(IDIR)

clean:
	rm *.o