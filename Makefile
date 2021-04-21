.PHONEY: all
CFLAGS=-g -O4
objs=kallsymlib.o

all: libkallsym.a kallsym

uthash/include/uthash.h: 
	git clone git@github.com:troydhanson/uthash.git

libkallsym.a: ${objs}
	ar -rc $@ ${objs}

kallsymlib.o: kallsymlib.c kallsymlib.h uthash/include/uthash.h
	gcc ${CFLAGS}  -c -I uthash/include $< -o $@ 

kallsym: kallsym.o libkallsym.a
	gcc -o $@ $< -L. -lkallsym 

kallsym.o: kallsym.c kallsymlib.h
	gcc ${CFLAGS} -c $< -o $@ 

clean:
	-rm libkallsym.a *.o kallsym
