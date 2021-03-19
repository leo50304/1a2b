.PHONY: clean

all: normal
	./run.out

test: normal
	./run.out test

normal: ./1a2b.cpp
	g++ $< -std=c++11 -o run.out

clean:
	rm -f run.out