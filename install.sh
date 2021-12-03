unzip -u archive.zip 
cd archive
make 
make socket/client
make master/master
make socket/server
make unnamed_pipe/unnamed_pipe
make named_pipe/named_pipe
make circle_buffer/circle_buffer
