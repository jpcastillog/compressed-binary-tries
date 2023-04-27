CFLAGS = -std=c++11 -O3 -march=native
CC = g++
OBJDIR = obj
BUILDIR = build
SDSLFLAGS = -DNDEBUG -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 --debug -DTEST
vpath %.cpp src
vpath %.hpp src

program: $(OBJDIR)/intersection.o $(OBJDIR)/intersection_query_log.o $(OBJDIR)/barbay_and_kenyon.o 
	$(CC) $(CFLAGS) test/build_tries.cpp -o $(BUILDIR)/build.out $(SDSLFLAGS) 
	$(CC) -o $(BUILDIR)/queries.out $(OBJDIR)/intersection_query_log.o $(OBJDIR)/intersection.o  $(CFLAGS) $(SDSLFLAGS) -pthread 
	$(CC) $(CFLAGS) test/measures.cpp -o $(BUILDIR)/measures.out $(SDSLFLAGS) 
	$(CC) $(CFLAGS) test/classic_coders.cpp -o $(BUILDIR)/classic_coders.out $(SDSLFLAGS)
	$(CC) $(CFLAGS) test/cbv_coding.cpp -o $(BUILDIR)/cbv_coding.out $(SDSLFLAGS)
	$(CC) $(CFLAGS) test/pom.cpp -o $(BUILDIR)/pom.out $(SDSLFLAGS)

$(OBJDIR)/intersection.o: src/intersection.cpp src/intersection.hpp
	mkdir -p obj
	$(CC) -c -o $@ src/intersection.cpp $(CFLAGS) $(SDSLFLAGS) 
	# -pthread

$(OBJDIR)/util_functions.o: src/util_functions.cpp src/util_functions.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/util_functions.cpp $(SDSLFLAGS)

$(OBJDIR)/barbay_and_kenyon.o: src/barbay_and_kenyon.cpp src/barbay_and_kenyon.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/barbay_and_kenyon.cpp

$(OBJDIR)/main.o: src/main.cpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/main.cpp $(SDSLFLAGS)

$(OBJDIR)/intersection_query_log.o: test/intersection_query_log.cpp
	mkdir -p obj
	$(CC) -c -o $@ test/intersection_query_log.cpp $(CFLAGS) $(SDSLFLAGS)
	# $(CC) -c -o $@ test/intersection_query_log.cpp

clean:
	rm -f core $(OBJDIR)/*.o
	rm -f core $(BUILDIR)/*.out