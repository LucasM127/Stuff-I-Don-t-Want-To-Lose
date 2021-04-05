LIBS=-lsfml-system-d -lsfml-window-d -lsfml-graphics-d
CC=g++ -g -Wall
SFMLDIR=/home/lucas/Headers/SFML-2.5.1
IDIR=$(SFMLDIR)/include/
LDIR=$(SFMLDIR)/lib/
LINK=-Wl,-rpath=$(LDIR) -L $(LDIR) $(LIBS)
OBJECTS=main.o ../../Perlin_Noise/PerlinNoise.o ../../sfml/GUI/QuickSFGui.o ../../SFML_Functions/SFUtilLib.a

test: $(OBJECTS)
	$(CC) $(OBJECTS) -o test $(LINK)

main.o: main.cpp Plotter.hpp PerlinPlotter.hpp PlotterManipulator.hpp
	$(CC) main.cpp -c -I $(IDIR)

clean:
	rm *.o