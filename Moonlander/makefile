LIBS=-lsfml-system -lsfml-window -lsfml-graphics
CC=g++ -g -Wall
SFMLDIR=/home/lucas/Headers/SFML-2.5.1
IDIR=$(SFMLDIR)/include/
LDIR=$(SFMLDIR)/lib/
LINK=-Wl,-rpath=$(LDIR) -L $(LDIR) $(LIBS)
OBJECTS=main.o World.o Entity.o Resources.o SpatialHashMap.o ../ParticleEngine/ConeSprayer.o ../SFML_Functions/SFUtilLib.a

test: $(OBJECTS)
	$(CC) $(OBJECTS) -o test $(LINK) -D LOG_DEBUG_STDOUT

main.o: main.cpp
	$(CC) main.cpp -c -I $(IDIR)

World.o: World.hpp World.cpp Resources.hpp
	$(CC) World.cpp -c -I $(IDIR) -D LOG_DEBUG_STDOUT

Entity.o: Entity.cpp Entity.hpp
	$(CC) Entity.cpp -c -I $(IDIR)

Resources.o: Resources.cpp Resources.hpp
	$(CC) Resources.cpp -c -I $(IDIR)

SpatialHashMap.o: SpatialHashMap.cpp SpatialHashMap.hpp
	$(CC) SpatialHashMap.cpp -c -I $(IDIR)

clean:
	rm *.o