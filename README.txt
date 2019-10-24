################################################################################################################################################
TEAM MEMBERS
################################################################################################################################################
1.Vikas Verma(193059003)
2.Apaar Bansal(193050035)
3.Ashwini Yadav(193050057)
4.Parashiv(193050029)

##############################################################################################################################################
HOW TO EXECUTE THE KVSTORE
##############################################################################################################################################
***************************
STEP1: COMPILE 	THE PROGRAM 
****************************
It will create the executable of client and server namedKVServer and KVClient respectively.

Command: make

***********************
STEP2: START THE SERVER
************************
There are three modes to start a server:

1.Dump the KVStore key value pairs to a file.

Command: ./KVServer dumpToFile[filename]

2.Restore the key value pairs from a file to the KVStore.

Command:./KVServer restoreFromFile[filename]

3.Simply start the Server 

Command:./KVServer

************************
STEP3:START THE CLIENT
************************

There are two modes of operation to start a client:

1.Interactive mode: In interactive mode multiple clients can give their requests through terminal to the server and get the output of the requests on the same terminal.

Command:./KVClient

2.Batch mode: In the batch mode mutliple clients can give their requests to the server through a file and get the output of their requests written in another files.Each client has it own different request file and response file.

Command:./KVClient [Inputfile] [Outputfile]

***************************
STEP4: CLEAN THE PROJECT(OBJECT FILES AND EXCECUTABLE FILES) 
***************************

Command: make clean
