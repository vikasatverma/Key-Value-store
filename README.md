TEAM MEMBERS
--------------

1.Vikas Verma(193059003)

2.Apaar Bansal(193050035)

HOW TO EXECUTE THE KVSTORE
------------

STEP 0: CHANGE THE FOLLOWING PARAMETER THROUGH THE config.h FILE
------

1.PORT 

2.threadPoolSize 

3.numSetsInCache 

4.sizeOfSet 


For example:

    #define PORT 8080

    #define threadPoolSize 0

    #define numSetsInCache 20

    #define sizeOfSet 20


STEP1: COMPILE 	THE PROGRAM 
---------

It will create the executable of client and server namedKVServer and KVClient respectively.

      make

STEP2: START THE SERVER
------

There are three modes to start a server:

1.Dump the KVStore key value pairs to a file.

    ./KVServer dumpToFile[filename]

2.Restore the key value pairs from a file to the KVStore.

    ./KVServer restoreFromFile[filename]

3.Simply start the Server 

    ./KVServer


STEP3:START THE CLIENT
---------------

There are two modes of operation to start a client:

1.Interactive mode: In interactive mode multiple clients can give their requests through terminal to
the server and get the output of the requests on the same terminal.

    ./KVClient

2.Batch mode: In the batch mode mutliple clients can give their requests to the server through a file
and get the output of their requests written in another files.Each client has it own different request file
and response file.

    ./KVClient [Inputfile] [Outputfile]


STEP4: CLEAN THE PROJECT(OBJECT FILES AND EXCECUTABLE FILES) 
----------

     make clean


Server Compilation:

    g++ KVServer.cpp -lcrypto -pthread -o KVServer
    ./KVServer

Client:

    g++ KVClient.cpp 
    ./KVClient port <port>
