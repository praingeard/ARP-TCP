#!/bin/sh 
pwd=$(pwd)
more README.txt

cd $ARCHIVEVAR

cat master/description_master.txt unnamed_pipe/description_unnamed_pipe.txt named_pipe/description_named_pipe.txt socket/description_socket.txt circle_buffer/description_circle_buffer.txt logarp/description_logarp.txt> description_all.txt
more description_all.txt
rm -f description_all.txt
cd $pwd