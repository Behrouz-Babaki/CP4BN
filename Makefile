CC=g++
CPP_FILES := $(wildcard src/*.cpp)
H_FILES := $(wildcard src/*.h)
OBJ_FILES := $(addprefix obj/, $(notdir $(CPP_FILES:.cpp=.o)))
DEP_FILES := $(addprefix obj/, $(notdir $(CPP_FILES:.cpp=.dep)))


CFLAGS+= -O3 -g -Wall -std=c++0x

ifdef GECODE_HOME
    CFLAGS+= -I $(GECODE_HOME)
	LDFLAGS+= -L $(GECODE_HOME)
endif

LDFLAGS+= -lgecodesearch -lgecodekernel -lgecodesupport -lgecodeint -lgecodefloat -lpthread

.PHONY: all clean test

all: bin/likely  bin/likely_gist 

bin/likely: ${OBJ_FILES}
	g++ ${CFLAGS} -o $@ ${OBJ_FILES} $(LDFLAGS)

bin/likely_gist: obj/likely_gist.o
	g++ ${CFLAGS} -o $@ ${OBJ_FILES} $(LDFLAGS) -lgecodegist

obj/likely_gist.o: ${OBJ_FILES}
	g++ ${CFLAGS} -DUSE_GIST -o $@ -c src/likely.cpp		

obj/%.o:src/%.cpp
	g++ ${CFLAGS} -o $@ -c $<

clean:
	@rm -f obj/* bin/* *~

include ${DEP_FILES}

obj/%.dep:src/%.cpp
	@set -e; rm -f $@; \
	gcc -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

test: bin/likely
	@ cd scripts; \
	./unit_tests.py test_parameters.dat --probable
