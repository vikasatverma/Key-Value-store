#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <bits/stdc++.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>


#define True 1
//#define False 0
#define PORT 8080
#define delimiter "_||_"
#define max_key_lenght 256 // 256 Bytes
#define max_value_lenght (256*1024) // 256 KB
#define max_buffer_size (256 + 256*1024 + 4*2) // key_length + value_length + delimiter_length

using std::cout;
using std::cin;

