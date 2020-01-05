3DT: main.o board.o player.o
	-g++ -Wall -ggdb3 -o 3dt main.o board.o player.o

main.o: main.cpp
	-g++ -Wall -c -g -ggdb3 -o main.o main.cpp

board.o: board.cpp
	-g++ -Wall -c -g -ggdb3 -o board.o board.cpp

player.o: player.cpp
	-g++ -Wall -c -g -ggdb3 -o player.o player.cpp

clean:
	-rm 3dt main.o board.o player.o