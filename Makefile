all: server client
server: header.hpp KVCache.o KVStore.o KVServer.cpp
	g++ KVServer.cpp header.hpp -o server

client: header.hpp KVClient.cpp
	g++ header.hpp KVClient.cpp -o client

KVCache.o: header.hpp KVCache.cpp
	g++ -c KVCache.cpp header.hpp

KVStore.o: header.hpp KVStore.cpp
	g++ -c KVStore.cpp header.hpp
