build:
	gcc -Wall battleship.c -o battleship -lncurses
run:
	./battleship mapa*.txt
clean:
	rm -r battleship
