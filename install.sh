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
make circle_buffer/circle_buffer
make unnamed_pipe/unnamed_pipe
make socket/client
make socket/server
make master/master
cd $pwd