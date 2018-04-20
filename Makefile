CFLAGS=		-g -Wall -O2 -Wc++-compat
CPPFLAGS=
INCLUDES=
OBJS=		map.o pair.o tad.o neighbor.o
PROG=		hickit
LIBS=		-lm -lz
ASAN_FLAG=

ifneq ($(asan),)
	ASAN_FLAG = -fsanitize=address
endif

.PHONY:all clean depend
.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(CFLAGS) $(ASAN_FLAG) $(CPPFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

hickit:$(OBJS) main.o
		$(CC) -o $@ $^ $(ASAN_FLAG) $(LIBS)

clean:
		rm -fr gmon.out *.o a.out $(PROG) *.a *.dSYM hickit.aux hickit.log hickit.pdf

depend:
		(LC_ALL=C; export LC_ALL; makedepend -Y -- $(CFLAGS) $(CPPFLAGS) -- *.c)

# DO NOT DELETE

main.o: hickit.h
map.o: hickit.h hkpriv.h khash.h kseq.h
neighbor.o: hkpriv.h hickit.h ksort.h
pair.o: hkpriv.h hickit.h ksort.h
tad.o: hkpriv.h hickit.h klist.h kavl.h
