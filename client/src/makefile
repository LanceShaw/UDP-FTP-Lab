client: main.o client.o common.o CommandHandle.o
	gcc main.o client.o common.o CommandHandle.o -o client -Wall

client.o: client.c client.h
	cc -c client.c -Wall

common.o: common.c common.h
	cc -c common.c -Wall

CommandHandle.o: CommandHandle.c CommandHandle.h
	cc -c  CommandHandle.c -Wall

clean:
	rm -rf *.o
	rm -rf client
