include ../Make.defines

.PHONY: all
all: ${LIB_OBJS}
	ar r ${LIBIM_NAME} $?
	${RANLIB} ${LIBIM_NAME}

.PHONY: clean
clean:
	rm -f ${CLEANFILES}
