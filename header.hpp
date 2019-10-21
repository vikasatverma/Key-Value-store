
#define PORT 8080
#define threadPoolSize 0
#define numSetsInCache 20
#define sizeOfSet 7

#define debugger_mode 1
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
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>


#define inputFile "batchRun.txt"
#define True 1
//#define False 0
#define delimiter "_||_"
#define max_key_lenght 256 // 256 Bytes
#define max_value_lenght (256*1024) // 256 KB
#define max_buffer_size (256 + 256*1024 + 4*2) // key_length + value_length + delimiter_length

using std::cout;
using std::cin;

std::vector<std::string> split(const char *str, char c = ' ') {
    std::vector<std::string> result;

    do {
        const char *begin = str;

        while (*str != c && *str)
            str++;

        result.emplace_back(begin, str);
    } while (0 != *str++);

    return result;
}


std::string exec(const char *cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


