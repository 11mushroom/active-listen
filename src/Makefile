CXX ?= g++
CXXFLAGS ?= -g -Wall
PREFIX ?= /usr
INSTDIR ?= $(PREFIX)/bin
srcdir = src

progfn := listen
progfile := $(progfn)

SRC := $(wildcard *.cpp)

OBJ = $(SRC:.cpp=.o)



.PHONY: clean install uninstall

all: $(progfile)
	@echo "========"
	@echo "| Done |"
	@echo "========"
	@echo ""
	@echo "now you can run"
	@echo "  make install"


$(progfile): listen.o Msock.o
	$(CXX) $(CXXFLAGS) $^ -o $@

listen.o: listen.cpp Msock.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

Msock.o: Msock.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@

install: $(progfile)
	cp $(progfile) $(INSTDIR)/

uninstall:
	rm $(INSTDIR)/$(progfn)

clean:
	rm $(wildcard *.o) $(progfile)
