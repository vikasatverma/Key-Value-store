#include "header.hpp"

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {address.sin_family = AF_INET,
            address.sin_port = htons(PORT),
            address.sin_addr.s_addr = INADDR_ANY};
    int new_socket, valread;
    char buffer[max_buffer_size] = {0};
    int opt = 1;
    int addr_len = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    //binding the address to the socket..address is the ip adress of the machine
    bind(server_fd, (struct sockaddr *) &address, sizeof(address));

    //listening on the socket with max no. of waiting connections
    listen(server_fd, 10);
    std::map<std::string, std::string> KVStore;

    // Server runs forever
    while (True) {
        //accept creates a new socket for comunication
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &(addr_len));
        if (debugger_mode) {
            cout << "connection made with client fd==========>" << new_socket << "\n";
        }
        //reading from the socket
        valread = read(new_socket, buffer, max_buffer_size);
        buffer[valread] = '\0';
        if (debugger_mode) {
            cout << buffer << "\n";
        }
        // Extract request type
        std::string request_type = strtok(buffer, delimiter);
        if (debugger_mode) {
        std::cout << request_type << '\n';
        }
        // Extract key
        std::string key = strtok(nullptr, delimiter);
        std::cout << key << '\n';
        std::string value;
        std::string response;
        std::string error_msg = "Error Message";
        char return_value[max_buffer_size];
        // Extract value if the request type is PUT
        if (request_type == "PUT") {
            value = strtok(nullptr, delimiter);
            if (debugger_mode) {
                cout << value << '\n';
            }
            KVStore[key] = value;
            response = "Success";
        } else if (request_type == "DEL") {
            if (KVStore[key].empty()) {
                response = "Does not exist";
            } else {
                KVStore.erase(key);
                response = "Success";
            }

        } else if (request_type == "GET") {
            if (KVStore[key].empty()) {
                response = "Does not exist";

            } else {
                response = key + " " + KVStore[key];
            }
        } else {
            response = error_msg;
        }


        strcpy(return_value, response.c_str());
        send(new_socket, return_value, sizeof(return_value), 0);
    }


}
