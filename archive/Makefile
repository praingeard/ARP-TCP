CC = gcc
LIBFLAG = -pthread
DEP = logarp/logarp.o

logarp/logarp.o: logarp/logarp.c logarp/logarp.h
	$(CC) logarp/logarp.c -c -o logarp/logarp.o

named_pipe/named_pipe: named_pipe/named_pipe.c $(DEP)
	$(CC) $(DEP) named_pipe/named_pipe.c -o named_pipe/named_pipe

circle_buffer/circle_buffer: circle_buffer/circle_buffer.c $(DEP)
	$(CC) $(DEP) circle_buffer/circle_buffer.c -o circle_buffer/circle_buffer $(LIBFLAG)

unnamed_pipe/unnamed_pipe: unnamed_pipe/unnamed_pipe.c $(DEP)
	$(CC) $(DEP) unnamed_pipe/unnamed_pipe.c -o unnamed_pipe/unnamed_pipe
	
socket/client: socket/client.c $(DEP)
	$(CC) $(DEP) socket/client.c -o socket/client
	
socket/server: socket/server.c $(DEP)
	$(CC) $(DEP) socket/server.c -o socket/server
	
master/master: master/master.c $(DEP)
	$(CC) $(DEP) master/master.c -o master/master
	
clean:
	rm -f master/master named_pipe/named_pipe circle_buffer/circle_buffer unnamed_pipe/unnamed_pipe socket/server socket/client logarp/logarp.o
