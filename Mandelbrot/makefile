SFML=/home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics
INC=-I $(SFML)/include
LINK= $(LIBS) -L $(SFML)/lib -Wl,-rpath=$(SFML)/lib
OBJS= main.o

test: $(OBJS)
	g++ -g -Wall $(OBJS) -o test $(LINK)

main.o: main.cpp
	g++ -g -Wall main.cpp -c $(INC)

clean:
	rm *.o test