CFLAGS = -std=c++11 -O
CC = g++

program: main.o intersection.o utils_functions.o
	$(CC) $(CFLAGS) -o program main.o intersection.o utils_functions.o -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

intersection.o: src/intersection.cpp
	$(CC) $(CFLAGS) -c src/intersection.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

utils_functions.o: src/utils_functions.cpp
	$(CC) $(CFLAGS) -c src/utils_functions.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

main.o: src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp -O3 -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64
	
clean:
	rm -f core *.o program

run: 
	./program

# g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib src/main.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64
