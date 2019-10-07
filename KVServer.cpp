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

    // Server runs forever
    while (True) {
        //accept creates a new socket for comunication
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &(addr_len));
        cout << "connection made with client fd==========>" << new_socket << "\n";

        //reading from the socket
        valread = read(new_socket, buffer, max_buffer_size);
        buffer[valread] = '\0';
        cout << buffer << "\n";

        // Extract request type
        std::string request_type = strtok(buffer, delimiter);

        std::cout << request_type << '\n';
        // Extract key
        std::string key = strtok(nullptr, delimiter);
        std::cout << key << '\n';
        std::string value;
        // Extract value if the request type is PUT
        if (request_type == "PUT") {
            value = strtok(nullptr, delimiter);
            std::cout << value << '\n';
        }
        send(new_socket, "Hello from server", sizeof("Hello from server"), 0);
    }


}
