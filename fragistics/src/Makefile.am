FRAGISTICSOFILES = database.o crosstable.o htmlfile.o game.o player.o os.o \
         main.o stats.o settings.o gameslogparser.o fileparser.o gameevent.o \
         aux.o

FRAGISTICSSOURCES = database.cpp crosstable.cpp htmlfile.cpp game.cpp \
          player.cpp os.cpp main.cpp stats.cpp settings.cpp \
          gameslogparser.cpp fileparser.cpp gameevent.cpp aux.cpp

FRAGISTICSTARGET = rawfragistics

FRAGISTICSRM = /bin/rm -f

bin_SCRIPTS = $(FRAGISTICSTARGET)

all-local: all1

all1: $(FRAGISTICSTARGET)

$(FRAGISTICSTARGET): $(FRAGISTICSSOURCES)
	$(CXX) -o $@ $(FRAGISTICSSOURCES)

clean-local:
	$(FRAGISTICSRM) $(FRAGISTICSOFILES)
	$(FRAGISTICSRM) $(FRAGISTICSTARGET)

