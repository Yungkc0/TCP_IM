include ./Make.defines

all:
	cd ${LIB_DIR} && make
	cd ${SERVER_DIR} && make

.PHONY: clean
clean:
	cd ${LIB_DIR} && make clean
	cd ${SERVER_DIR} && make clean
	rm *.a
