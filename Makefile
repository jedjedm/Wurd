CC = $(shell if [ -x /usr/local/cs/bin/g32 ]; then echo g32; else echo g++ -std=c++17; fi)
CCFLAGS = -Wno-unused-parameter
LIBS = -lncurses -Wl,--rpath=/usr/local/cs/lib64

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

PRODUCT = wurd

all: $(PRODUCT)

%.o: %.cpp $(HEADERS)
	$(CC) -c $(CCFLAGS) $< -o $@

$(PRODUCT): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PRODUCT)
