include ../Make.defines

PROGS = serv

serv: main.o
	${CC} -o $@ $< ${LIBS}

main.o: main.c
	${CC} ${CFLAGS} -c $<

.PHONY: clean
clean:
	rm -f ${PROGS} ${CLEANFILES}
