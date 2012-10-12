
VPATH 			+= src

CC 			:= gcc

CFLAGS 			:= -Wall -Werror -std=gnu99

LDLIBS 			+= -lmad
LDLIBS 			+= -lao
LDLIBS 			+= -ljson
LDLIBS 			+= -lpthread

MAIN 			:= ./src/main.c
TEST 			:= ./test.c
BINNAME 		:= dbfm

SOURCE 			:= ${filter-out $(TEST), $(shell find "./src" -type f -name "*.c")}
OBJECT 			:= ${patsubst %.c, %.o, $(SOURCE)}

.PHONY: 		all clean

all: 			$(OBJECT) main

main: 			$(OBJECT)
	$(CC) $(LDLIBS) $(OBJECT) -o $(BINNAME)

clean: 
	cd ./src && rm *.o
	-rm $(BINNAME)
	-rm $(BINTEST)
