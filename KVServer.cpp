#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#define PORT 8080

using namespace std;


int main(){
	

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in address;
	int new_socket, valread;
	char buffer[1024] = {0};
	int opt = 1;
	int addr_len = sizeof(address);

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));	

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 

    //binding the address to the socket..address is the ip adress of the machine
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    //listening on the socket with max no. of waiting connections
    listen(server_fd, 10);


    	//accept creates a new socket for comunication
    	new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&(addr_len));
    	cout<<"connection made with client fd==========>"<<new_socket<<"\n";

    	//reading from the socket
    	valread = read(new_socket, buffer, 1024);
    	buffer[valread] = '\0';
    	cout<<buffer<<"\n";
    	//string hello = "Hello from server";
    	send(new_socket, "Hello from server", sizeof("Hello from server"), 0);

    return 0;

}
