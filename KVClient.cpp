#include "header.hpp"

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
        request = request_type + delimiter + key;
    } else if (request_type == "PUT") {
        getline(cin, key);
        getline(cin, value);
        checkLenght(key, value);
        request = request_type + delimiter + key + delimiter + value;
    } else if (request_type == "DEL")
    {
        getline(cin, key);
        request = request_type + delimiter + key;
    } else {
        cout << "Unknown Error: Undefined request type";
        exit(-1);
    }

    //client connect on this
    struct sockaddr_in serv_addr = {AF_INET, htons(PORT)};
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //creating a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    cout << sockfd << "\n";
    if (sockfd < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));


    cout << request << "\n";
    cout << "###################################### SENDING DATA #############################################\n";

    send(sockfd, request.c_str(), request.size(), 0);


    valread = read(sockfd, buffer, max_buffer_size);
    buffer[valread] = '\0';
    cout << buffer << "\n";


}
