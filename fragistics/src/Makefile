#
# simple Makefile for compiling fragistics
#
# April, 2002, Steffen Schwigon
#

OFILES = database.o crosstable.o htmlfile.o game.o player.o os.o main.o \
         stats.o settings.o gameslogparser.o fileparser.o gameevent.o \
         aux.o

SOURCES = database.cpp crosstable.cpp htmlfile.cpp game.cpp player.cpp \
          os.cpp main.cpp stats.cpp settings.cpp gameslogparser.cpp \
          fileparser.cpp gameevent.cpp aux.cpp

TARGET = rawfragistics

RM = /bin/rm -f

# enforce compilation
full:
	g++ -g -o $(TARGET) $(SOURCES)

all: all1

all1: $(TARGET)

all2:
	for i in *.cpp ; do g++ -c $$i ; done 
	g++ -o $(TARGET) *.o 

$(TARGET): $(SOURCES)
	g++ -g -o $@ $(SOURCES)

clean:
	$(RM) $(OFILES)
	$(RM) $(TARGET)

