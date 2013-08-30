# Project related
NAME?=Goku
SIZE?=13
PROGRAM=ai
SOURCE=gtp.cc main.cc
OBJS := $(patsubst %.cc, %.o, $(SOURCE))

# Compilation related
CXX=clang++
CXXDEBUG=-ggdb3
CXXFLAGS=-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic \
	 --std=c++11 $(CXXDEBUG)
DEFINES=-D__GOBAN_SIZE=$(SIZE) -D__NAME=$(NAME)

# Gogui Twogtp related
WHITE?=$(PROGRAM)
BLACK?=$(PROGRAM)
TWOGTP="gogui-twogtp -black $(BLACK) -white $(WHITE) -size $(SIZE)"

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) -I . $(OBJS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -I . $(DEFINES) -c $< -o $@

mostlyclean:
	rm -vf $(OBJS)

clean: mostlyclean
	find . -name $(PROGRAM) -delete

test: $(PROGRAM)
	gogui 								\
	  -computer-both 						\
	  -auto								\
	  -size $(SIZE)							\
	  -program $(TWOGTP)
