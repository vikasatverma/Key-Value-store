all: KVServer KVClient
KVServer: header.hpp KVCache.o KVStore.o KVServer.cpp
	g++ KVServer.cpp header.hpp -o KVServer

KVClient: header.hpp KVClient.cpp
	g++ header.hpp KVClient.cpp -o KVClient

KVCache.o: header.hpp KVCache.cpp
	g++ -c KVCache.cpp header.hpp

KVStore.o: header.hpp KVStore.cpp
	g++ -c KVStore.cpp header.hpp

clean: 
	rm -rf *.o KVServer KVClient *.gch 
