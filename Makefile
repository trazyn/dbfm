
VPATH 			+= src

CC 			:= cc

CFLAGS 			:= -Wall -Werror -std=gnu99

LDLIBS 			+= -lmad
LDLIBS 			+= -lao
LDLIBS 			+= -ljson-c
LDLIBS 			+= -lpthread
LDLIBS 			+= -lncurses

MAIN 			:= ./src/main.c
BINNAME 			:= doubanFM

SOURCE 			:= ${filter-out $(TEST), $(shell find "./src" -type f -name "*.c")}
OBJECT 			:= ${patsubst %.c, %.o, $(SOURCE)}

.PHONY: 		all clean

all: 			$(OBJECT) main

main: 			$(OBJECT)
	$(CC) $(LDLIBS) $(OBJECT) -o $(BINNAME)

clean: 
	@cd ${VPATH}; rm *.o 2> /dev/null
	@rm $(BINNAME) 2> /dev/null
