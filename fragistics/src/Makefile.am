FRAGISTICSOFILES = database.o crosstable.o htmlfile.o game.o player.o os.o \
         main.o stats.o settings.o gameslogparser.o fileparser.o gameevent.o \
         aux.o

FRAGISTICSSOURCES = database.cpp crosstable.cpp htmlfile.cpp game.cpp \
          player.cpp os.cpp main.cpp stats.cpp settings.cpp \
          gameslogparser.cpp fileparser.cpp gameevent.cpp aux.cpp

FRAGISTICSTARGET = rawfragistics

FRAGISTICSRM = /bin/rm -f

bin_SCRIPTS = $(FRAGISTICSTARGET)

EXTRA_DIST = rawfragistics rawfragistics_static *.cpp *.h *.txt

all-local: all1 static

all1: $(FRAGISTICSTARGET)

static: $(FRAGISTICSTARGET)_static

$(FRAGISTICSTARGET): $(FRAGISTICSSOURCES)
	$(CXX) -o $@ $(FRAGISTICSSOURCES)

$(FRAGISTICSTARGET)_static: $(FRAGISTICSSOURCES)
	$(CXX) -static -o $@ $(FRAGISTICSSOURCES)

clean-local:
	$(FRAGISTICSRM) $(FRAGISTICSOFILES)
	$(FRAGISTICSRM) $(FRAGISTICSTARGET)
	$(FRAGISTICSRM) $(FRAGISTICSTARGET)_static

