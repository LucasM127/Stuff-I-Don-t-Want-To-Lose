SFML_DIR = /home/lucas/Headers/SFML-2.5.1
LIBS = -lsfml-system -lsfml-window -lsfml-graphics
TARGET = test

$(TARGET): main.cpp
	g++ -g -Wall main.cpp -o $(TARGET) -I $(SFML_DIR)/include/ \
	 -L $(SFML_DIR)/lib/ $(LIBS) -Wl,-rpath=$(SFML_DIR)/lib/