project2: main.o headers.h
	gcc -o project2 -Wall  -g3 -O0 main.o

main.o: main.c headers.h
	gcc -c -Wall -g3  -O0 main.c

clean:
	rm *.o -f
	touch *.clean
	rm project2 -f