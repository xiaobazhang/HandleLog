CC      := g++
LIBS    := -L$(MGJ_REL)/lib
LDFLAGS:= -lpthread
INCLUDE:= -I$(MGJ_REL)/include/ckit
CFLAGS  := -g -Wall -O3
TARGET := Handle_Log
SOURCE := readlog.cpp\
	main.cpp
OBJECT := readlog.o\
	main.o

all : $(TARGET)
 
$(TARGET) : $(OBJECT) 
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(LIBS)
$(OBJECT) : $(SOURCE) 
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)
clean :
	rm -rf *.o
	rm -rf $(TARGET)

