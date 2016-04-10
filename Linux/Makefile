LIB=-lim

main : main.o libim.a
	cc -o $@ $< ${LIB} -L.
	make clean

main.o : main.c
	cc -c $<

encrypt : encrypt.o md5.o
	ar -r libencrypt.a $^
	make clean

libim.a : encrypt.o md5.o server.c
	ar -r libim.a $^
encrypt.o : encrypt.c
	cc -c $<
md5.o : md5.c
	cc -c $<
server.o : server.c
	cc -c $<

clean:
	rm *.o
