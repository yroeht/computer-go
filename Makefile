# Project related
NAME?=Goku
SIZE?=13
PROGRAM=goku
SOURCE=gtp.cc main.cc
OBJS := $(patsubst %.cc, %.o, $(SOURCE))

# Compilation related
CXX=clang++
CXXDEBUG=-ggdb3
CXXFLAGS=-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic	\
	 -Wno-global-constructors -Wno-exit-time-destructors		\
	 --std=c++11 $(CXXDEBUG)
DEFINES=-D__GOBAN_SIZE=$(SIZE) -D__NAME=$(NAME)

# Gogui Twogtp related
WHITE?=$(PROGRAM)
BLACK?=$(PROGRAM)
TWOGTP="gogui-twogtp -black $(BLACK) -white $(WHITE) -size $(SIZE)
TWOGTP+=-sgffile `date +%s` -debugtocomment -verbose"

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) -I . $(OBJS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -I . $(DEFINES) -c $< -o $@

mostlyclean:
	rm -vf $(OBJS)

clean: mostlyclean
	find . -name $(PROGRAM) -delete

test: test-twogtp

test-human: $(PROGRAM)
	gogui 								\
	  -computer-white 						\
	  -size $(SIZE)							\
	  -program $(PROGRAM)

test-twogtp: $(PROGRAM)
	gogui 								\
	  -computer-both 						\
	  -size $(SIZE)							\
	  -program $(TWOGTP)

test-dummy: $(PROGRAM)
	BLACK=gogui-dummy make test-twogtp
