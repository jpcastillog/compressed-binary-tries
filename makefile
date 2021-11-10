CFLAGS = -std=c++11 -O -msse4.2
CC = g++

program: main.o intersection.o util_functions.o barbay_and_kenyon.o
	$(CC) $(CFLAGS) -o program main.o intersection.o util_functions.o barbay_and_kenyon.o -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

intersection.o: src/intersection.cpp
	$(CC) $(CFLAGS) -c src/intersection.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

util_functions.o: src/util_functions.cpp
	$(CC) $(CFLAGS) -c src/util_functions.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

barbay_and_kenyon.o: src/barbay_and_kenyon.cpp
	$(CC) $(CFLAGS) -c src/barbay_and_kenyon.cpp

main.o: src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64
	
clean:
	rm -f core *.o program

run: 
	./program

# g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib src/main.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64
