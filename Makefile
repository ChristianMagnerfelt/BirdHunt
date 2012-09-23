all: main.o
	g++ -o main *.o -std=c++0x

main.o:
	g++ -c *.cc -std=c++0x	

clean:
	rm -f *.o main
