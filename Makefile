#CXXFLAGS=-pg -O2 $(shell pkg-config --cflags xqdbm )
CXXFLAGS=-O2 -g3  -finline-functions  $(shell pkg-config --cflags xqdbm ) -Wall
#CXXFLAGS=-g3 $(shell pkg-config --cflags xqdbm ) -Wall
LIBS=$(shell pkg-config --libs xqdbm) -lboost_filesystem -lboost_regex -lboost_program_options
COMMON=collection.o document.o tabdocument.o
MYINDEX=myindex.o $(COMMON)
SEARCH=search.o $(COMMON) searcher.o
SPLIT=split.o
TEST=test.o searcher.o
INTERFACE=interface.o soapC.o soapServer.o #soapServerLib.o
COMPRESS=compress.o
#INTERFACE=interface.o soapServer.o soapServerLib.o

all: myindex search split compress

run: all
	time ./myindex

myindex: $(MYINDEX)
	g++ -o myindex -pg -g3 $(MYINDEX)  $(LIBS) $(CXXFLAGS)

search: $(SEARCH)
	g++ -o search -g3 $(SEARCH)  $(LIBS) $(CXXFLAGS)

test: $(TEST)
	g++ -o test -g3 $(TEST)  $(LIBS) $(CXXFLAGS)

split: $(SPLIT)
	g++ -o split -g3 $(SPLIT)  $(LIBS) $(CXXFLAGS)

compress: $(COMPRESS)
	g++ -o compress -g3 $(COMPRESS)  $(LIBS) $(CXXFLAGS)


interface.cgi: $(INTERFACE)
	g++ -o interface.cgi -g3 $(INTERFACE)  $(LIBS) $(CXXFLAGS) -lgsoap++


clean:
	@rm -f myindex search *.o

deps:
	@for i in *.cpp; do g++ $(CXXFLAGS) -M $$i; done >.deps

-include .deps