CC = gcc
LIBFLAG = -pthread
LIBFLAG2 = -lrt
DEP = logarp/logarp.o

logarp/logarp.o: logarp/logarp.c logarp/logarp.h
	$(CC) logarp/logarp.c -c -o logarp/logarp.o

named_pipe/named_pipe: named_pipe/named_pipe.c $(DEP)
	$(CC) $(DEP) named_pipe/named_pipe.c -o named_pipe/named_pipe

named_pipe/named_pipe_client: named_pipe/named_pipe_client.c $(DEP)
	$(CC) $(DEP) named_pipe/named_pipe_client.c -o named_pipe/named_pipe_client

circle_buffer/writer: circle_buffer/writer.c $(DEP)
	$(CC) $(DEP) circle_buffer/writer.c -o circle_buffer/writer $(LIBFLAG) $(LIBFLAG2)

circle_buffer/reader: circle_buffer/reader.c $(DEP)
	$(CC) $(DEP) circle_buffer/reader.c -o circle_buffer/reader $(LIBFLAG) $(LIBFLAG2)

unnamed_pipe/unnamed_pipe: unnamed_pipe/unnamed_pipe.c $(DEP)
	$(CC) $(DEP) unnamed_pipe/unnamed_pipe.c -o unnamed_pipe/unnamed_pipe
	
socket/client: socket/client.c $(DEP)
	$(CC) $(DEP) socket/client.c -o socket/client
	
socket/server: socket/server.c $(DEP)
	$(CC) $(DEP) socket/server.c -o socket/server
	
master/master: master/master.c $(DEP)
	$(CC) $(DEP) master/master.c -o master/master
	
clean:
	rm -f master/master named_pipe/named_pipe named_pipe/named_pipe_client circle_buffer/writer circle_buffer/reader unnamed_pipe/unnamed_pipe socket/server socket/client logarp/logarp.o
