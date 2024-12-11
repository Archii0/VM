flags=-O2 -Wall -std=c2x -g
ldflags=

.PHONY: all clean

all: clean VM

VM: VM.o utils.o
	cc $(flags) $^ -o $@ $(ldflags)

VM.o: VM.c VM.h
	cc $(flags) -c $<

utils.o: utils.c utils.h
	cc $(flags) -c $<

clean:
	rm -f *.o VM
