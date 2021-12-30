[GENERAL DESCRIPTION]

authors:
  - Paul Raingeard de la Bletiere
  - Elea Papin
  If there is any problem while running this code, we can be contacted with the email adresses p.raingeard.37@gmail.com and elea.papin@eleves.ec-nantes.fr

usage: 

  prerequisites : 
    - gcc compiler 

  to run the programs : 
    - first use install by giving it the path to extract your files : . install.sh WANTED_PATH
    - if no path is specified, everything is unzipped in the ./archive folder
    BE CAREFUL : no slash in the call to the scripts
    - then run using . run.sh
    - to uninstall use . uninstall.sh
    - once installed, you can call help.sh to get more information about the different processes

  scripts :

    - install.sh : unzip the archive and compile the executables.
    - run.sh : run the executables once they have been installed.
    - cleanup.sh : remove the executables. You need to run install.sh again to recompile. Also clean logs.
    - uninstall.sh : delete the added files and the unziped archive.
    - help.sh : shows this document and the description of the other processes if they have been installed

  processes : 

    - master : run all the other processes and get user input.
    - unnamed_pipe : send data through unnamed pipe.
    - named_pipe and named_pipe_client : send an array of random data through a named pipe.
    - socket server and client : send an array of random data through a socket.
    - logarp : manage logs.
    - circle_buffer writer and reader : send an array of random data through a circle buffer with shared memory.
    
  description:
    
  Implements several examples of ways to send data through different processes. Returns the execution time of send.
    
  commands:

  The user has to input the desired process to run (either unnamed_pipe, named_pipe, socket or circle_buffer), the size of data to send and the size of the buffer in case of circle_buffer.
