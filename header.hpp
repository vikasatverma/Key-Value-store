#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <bits/stdc++.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define PORT 8080
#define True 1
#define False 0
#define delimiter "_||_"
#define max_key_lenght 256
#define max_value_lenght (256*1024)
#define max_buffer_size (256+256*1024+4*2)

using std::cout;
using std::cin;

