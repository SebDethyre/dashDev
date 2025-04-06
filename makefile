CC = gcc
CFLAGS = -Wall -g
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
GTK_INCLUDE = -I/usr/include/gtk-3.0
GTK_LIBDIR = -L/usr/lib/x86_64-linux-gnu
GTK_LIBRARIES = -lgtk-3 -lX11 -lXtst -lm 

SRC = c.c src/fileman.c src/launching.c
TARGET = ~/sandbox/dash_dev_boisson/dash_dev_boisson

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(GTK_CFLAGS) $(GTK_INCLUDE) $(GTK_LIBDIR) $(GTK_LIBRARIES) $(GTK_LIBS)

clean:
	rm -f $(TARGET)

debug: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(GTK_CFLAGS) $(GTK_INCLUDE) $(GTK_LIBDIR) $(GTK_LIBRARIES) $(GTK_LIBS)
	gdb $(TARGET)


# CXX = g++
# CXXFLAGS = -Wall -g
# GTK_CXXFLAGS = `pkg-config --cflags gtk+-3.0`
# GTK_LIBS = `pkg-config --libs gtk+-3.0`
# GTK_INCLUDE = -I/usr/include/gtk-3.0
# GTK_LIBDIR = -L/usr/lib/x86_64-linux-gnu
# GTK_LIBRARIES = -lgtk-3 -lX11 -lXtst -lm

# SRC = c.c src/fileman.c src/launching.c
# TARGET = ~/sandbox/dash_dev_boisson/dash_dev

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(GTK_CXXFLAGS) $(GTK_INCLUDE) $(GTK_LIBDIR) $(GTK_LIBRARIES) $(GTK_LIBS)

# clean:
# 	rm -f $(TARGET)

# debug: $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(GTK_CXXFLAGS) $(GTK_INCLUDE) $(GTK_LIBDIR) $(GTK_LIBRARIES) $(GTK_LIBS)
# 	gdb $(TARGET)
