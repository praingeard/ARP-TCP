pwd=$(pwd)
if [ -z "$1" ]; then 
    ARCHIVEVAR=~/archive
else 
    ARCHIVEVAR=$1
fi

unzip -u -d /tmp/archive archive.zip
rm -rf $ARCHIVEVAR
mkdir $ARCHIVEVAR
mv  -v /tmp/archive/archive/* $ARCHIVEVAR
cd $ARCHIVEVAR
make 
make named_pipe/named_pipe
make named_pipe/named_pipe_client
make circle_buffer/writer
make circle_buffer/reader
make unnamed_pipe/unnamed_pipe
make socket/client
make socket/server
make master/master
cd $pwd