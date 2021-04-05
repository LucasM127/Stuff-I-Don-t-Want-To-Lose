SFML = /home/lucas/Headers/SFML-2.5.1
SFML_INC = $(SFML)/include/
SFML_LIB = $(SFML)/lib/
INCLUDE = -I $(SFML_INC)
LIBS = -lsfml-system -lsfml-window -lsfml-graphics
LINK = $(LIBS) -L $(SFML_LIB) -Wl,-rpath=$(SFML_LIB)

all:
	g++ -g -Wall main.cpp ../SFML_Functions/SFUtilLib.a -o test $(INCLUDE) $(LINK)