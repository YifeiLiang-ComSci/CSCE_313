all: shell

main.o: main.cpp
	g++ -c -g main.cpp

shell:main.o
	g++ -o shell main.o -g -fsanitize=address
clean:
	rm *.o