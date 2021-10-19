main:
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib src/main.cpp -o program -lsdsl -ldivsufsort -ldivsufsort64

run: 
	./program