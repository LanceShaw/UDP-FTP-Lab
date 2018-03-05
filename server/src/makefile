server: main.o server.o common.o CommandHandle.o
	gcc -pthread main.o server.o common.o CommandHandle.o -o server -Wall

server.o: server.c server.h
	cc -c server.c -Wall

common.o: common.c common.h
	cc -c common.c -Wall

CommandHandle.o: CommandHandle.c CommandHandle.h
	cc -c  CommandHandle.c -Wall

clean:
	rm -rf *.o
	rm -rf server

