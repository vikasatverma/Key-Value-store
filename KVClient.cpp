#include<iostream>
#include"header.hpp"
using namespace std;
// to convert the plain text to xml format
std::string plaintoxml(std::string msg_type,std::string key,std::string value ="") 
{

std::string request ="<?xml version='1.0' encoding='UTF-8'?>\n";

if(msg_type=="GET")
{
 msg_type="<KVMessage type='getreq'>\n";
 key="<Key>"+key+"</Key>\n";
}
if(msg_type=="PUT")
{

 msg_type="<KVMessage type='putreq'>\n";
 key= "<key>"+key+"</Key>\n";
 value="<Value>"+value+"</Value>\n";
 key=key+value;

}
if(msg_type=="DEL")
{

 msg_type="<KVMessage type='delreq'>\n";
 key="<Key>"+key+"</Key>\n";
}

request= request+msg_type+key+"</KVMessage>\n";

return request;
}
//to get back xml format to plain text
std::string xmltoplain(std::string request)
{

return request;
}

int checkLenght(const std::string &key, const std::string &value = " ") {
    if (key.size() > max_key_lenght) {
        cout << "Oversized key";
        exit(-1);
    }
    if (value.size() > max_value_lenght) {
        cout << "Oversized value";
        exit(-1);
    }

}

//TODO: Increase capacity of request string to store 256KB
//TODO: Accept input from the file batchRun.txt


int main() {

    cout << "enter the request type" << "\n";
    cout << "GET, PUT OR DEL\n";
    std::string request_type;
    getline(cin, request_type);
    //cout<<"hello\n";
    char buffer[max_buffer_size] = {0};
    int valread;
    std::string key;
    std::string value;
    std::string request;
   
    if (request_type == "GET") {
        getline(cin, key);
        checkLenght(key, value);
        //if(key.length()<256)
        //	key.resize(256);
        	
        request=plaintoxml(request_type,key,value);
	
    } else if (request_type == "PUT") {
        getline(cin, key);
        getline(cin, value);
        checkLenght(key, value);
        //if(key.length()<256)
		//key.resize(256,0);
	request=plaintoxml(request_type,key,value);
		
    } 
	else if (request_type == "DEL")
    {
        getline(cin, key);
        checkLenght(key, value);
        //if(key.length()<256)
        //	key.resize(256);
        request=plaintoxml(request_type,key);
    } else {
        cout << "Unknown Error: Undefined request type";
        exit(-1);
    }

    //cout<<request;
    struct sockaddr_in serv_addr = {AF_INET, htons(PORT)};
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //creating a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (debugger_mode) {
        cout << sockfd << "\n";
    }
    if (sockfd < 0) {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (debugger_mode) {
    cout << request << "\n";
    cout << "###################################### SENDING DATA #############################################\n";
    }
  
    send(sockfd, request.c_str(), request.size(), 0);


    valread = read(sockfd, buffer, max_buffer_size);
    buffer[valread] = '\0';
    cout << buffer << "\n";
  
}
