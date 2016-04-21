include ../Make.defines

PROGS = cli

cli: main.o
	${CC} -o $@ $< ${LIBS}

main.o: main.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -f ${PROGS} ${CLEANFILES}
