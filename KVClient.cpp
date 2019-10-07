
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <bits/stdc++.h>
#include <string.h>
#define PORT 8080 

using namespace std;



int main(){

	cout<<"enter the request type"<<"\n";
	cout<<"GET, PUT OR DEL\n";
	string request_type;
	cin>>request_type;
	//cout<<"hello\n";
	char buffer[1024] = {0};
	int valread;
	string key;
	string value;

	string request;

	if(request_type == "GET"){
		cin>>key;
		request = request_type+ " " + key;
	}
	else if(request_type == "PUT"){
		cin>>key;
		cin>>value;
		request = request_type+ " " + key + value;
	}else if(request_type == "DEL"){
		cin>>key;
		request = request_type+ " " + key;	
	}
	//cout<<"hello\n";
	//client connect on this
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(struct addrinfo));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	//creating a socket 
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	cout<<sockfd<<"\n";
    if (sockfd < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

	connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));


		cout<<request<<"\n";
		cout<<"###################################### SENDING DATA #############################################\n";
		send(sockfd , request.c_str() , request.size(), 0);
		

		valread = read(sockfd , buffer, 1024);
		buffer[valread] = '\0';
		cout<<buffer<<"\n";


}
