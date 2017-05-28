include ../Make.defines

PROGS = cli

serv: main.o
	${CC} -o $@ $< ${LIBS}

main.o: main.c
	${CC} ${CFLAGS} -c $<

clean:
	rm -f ${PROGS} ${CLEANFILES}
