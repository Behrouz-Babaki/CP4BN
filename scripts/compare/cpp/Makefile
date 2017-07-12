.PHONY: all clean
FLAGS = --std=c++0x

all: testIO testTypes testMultiVal

testIO: testIO.o
	g++ $< -o $@

testTypes: testTypes.o
	g++ $< -o $@

testMultiVal: testMultiVal.o attrSetWriter.o
	g++ $^ -o $@

%.o: %.cpp
	g++ -c $(FLAGS) $< -o $@
clean:
	@rm -rf *~ *.o testIO testTypes testMultiVal