ARC = libgnelib.a
LIB =

GCC_COMPILE = -Wall -O2 -s

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(ARC) examples

$(ARC): $(OBJ)
	ar rs $@ $^	

%.o: %.cpp
	gcc -c -o $@ $(GCC_COMPILE) $<

.PHONY: clean install all

examples:
	make -C ../examples

install: $(ARC)
	copy $< $(MINGDIR)\lib
	copy gnelib.h $(MINGDIR)\include

clean:
	if exist *.obj del *.o