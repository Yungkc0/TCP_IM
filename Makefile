LIB=-lim

main : main.o libim.a
	cc -o $@ $< ${LIB} -L.
	make clean

main.o : main.c
	cc -c $<

libim.a : encrypt.o md5.o servcli.o
	ar -r libim.a $^
encrypt.o : encrypt.c
	cc -c $<
md5.o : md5.c
	cc -c $<
servcli.o : servcli.c
	cc -c $<

clean:
	rm *.o
