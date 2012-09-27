all: main.o
	g++ -o main *.o -std=c++0x

main.o: *.h *.cc
	g++ -c *.cc -std=c++0x	

clean:
	rm -f *.o main
