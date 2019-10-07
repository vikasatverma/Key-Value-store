#include "header.hpp"

// writes key value pairs to store ie file
int restoreFromFile(const std::string &fname, std::map<std::string, std::string> *m) {
    int count = 0;
    if (access(fname.c_str(), R_OK) < 0)
        return -errno;

    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp)
        return -errno;

    m->clear();

    char *buf = nullptr;
    size_t buflen = 0;

    while (getline(&buf, &buflen, fp) > 0) {
        char *nl = strchr(buf, '\n');
        if (nl == nullptr)
            continue;
        *nl = 0;

        char *sep = strchr(buf, '=');
        if (sep == nullptr)
            continue;
        *sep = 0;
        sep++;

        std::string s1 = buf;
        std::string s2 = sep;

        (*m)[s1] = s2;

        count++;
    }

    if (buf)
        free(buf);

    fclose(fp);
    return count;
}

// reads key value pairs from store ie file
int dumpToFile(const std::string &fname, std::map<std::string, std::string> *m) {
    int count = 0;
    if (m->empty()) {
        return 0;
    }

    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp) {
        return -errno;
    }

    for (std::map<std::string, std::string>::iterator it = m->begin(); it != m->end(); it++) {
        fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
        count++;
    }

    std::cout << count;
    fclose(fp);
    return count;
}

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
    std::string filename = "KVStore_file";

    // Read key value store from file
    restoreFromFile(filename, &KVStore);

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
        if (debugger_mode) {
            std::cout << key << '\n';
        }
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
        dumpToFile(filename, &KVStore);
    }


}
