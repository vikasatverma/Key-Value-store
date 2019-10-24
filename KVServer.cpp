#include "header.hpp"
#include "KVCache.cpp"
#include "KVStore.cpp"

#include <queue>

KVCache cacheMap;

void HandleRequest(int new_socket, int valread, const char *buffer1) {
    if (debugger_mode) {
        cout << buffer1 << "\n";

    }

    std::string buffer;
    for (int i = 0; i < valread; i++) {
        buffer += (buffer1[i]);
    }


    std::string buffer2 = fromxml(buffer);

    char chararr_of_buffer[buffer2.length() + 1];
    strcpy(chararr_of_buffer, buffer2.c_str());

    // Extract request type
    std::string request_type = strtok(chararr_of_buffer, delimiter);
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
            cout << "Value=" << value << "\n";
        }

        std::map<std::string, std::string> tmp_map;

        putIntoFile(key, value);
        response = "Success";


    } else if (request_type == "DEL") {
        std::map<std::string, std::string> tmp_map;
        populateMap(key, &tmp_map);
        if (cacheMap.get(key) == "Does not exist" && tmp_map[key].empty()) {
            response = "Does not exist";
        } else {
            tmp_map.erase(key);
            storeMapToFile(key, &tmp_map);
            cacheMap.del(key);
            response = "Success";
        }
    } else if (request_type == "GET") {
        if (cacheMap.get(key) == "Does not exist") {
            std::map<std::string, std::string> tmp_map;
            populateMap(key, &tmp_map);
            if (tmp_map[key].empty()) {
                response = "Does not exist";
            } else {
                cacheMap.put(key, tmp_map[key]);
                response = key + " " + cacheMap.get(key);
            }
        } else {
            response = key + " " + cacheMap.get(key);
        }
    } else {
        response = error_msg;
    }
    response = toXML(response);
    strcpy(return_value, response.c_str());

    if (debugger_mode) {
        cout << "Response: \n" << response;
    }
    cout << return_value << std::endl;
    send(new_socket, return_value, sizeof(return_value), 0);
//        close(new_socket);
}

int main(int argc, char *argv[]) {

    cout << "To dump the KVStore key value pairs to a file, use command:\n"
            "./[ServerExecutable] dumpToFile [filename]\n"
            "==================================OR==================================\n"
            "To restore the key value pairs from a file to the, use command:\n"
            "./[ServerExecutable] restoreFromFile [filename]\n";

    KVStore kvStore;
    if (argc == 3) {
        if (strcmp(argv[1], "restoreFromFile") != 0) {
            kvStore.RestoreFromFile(argv[2]);
            cout << "Restore from file " << argv[1] << " successful." << std::endl;
        } else if (strcmp(argv[1], "dumpToFile") != 0) {
            kvStore.dumpToFile(argv[2]);
            cout << "Dump to file " << argv[1] << " successful." << std::endl;
        }
    }
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);


    struct sockaddr_in address = {address.sin_family = AF_INET,
            address.sin_port = htons(PORT),
            address.sin_addr.s_addr = INADDR_ANY};
    int new_socket, valread;
    char buffer1[max_buffer_size] = {0};
    int opt = 1;
    int addr_len = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    //binding the address to the socket..address is the ip adress of the machine
    bind(server_fd, (struct sockaddr *) &address, sizeof(address));

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    int rc = listen(server_fd, numOfTotalFDs);
    if (rc < 0) {
        perror("Network Error: listen() failed");
        close(server_fd);
        exit(-1);
    }

    int i = 0;
    // Server runs forever
    while (True) {
        //accept creates a new socket for comunication
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &(addr_len));
        cout << ++i;
        if (debugger_mode) {
            cout << "connection made with client fd==========>" << new_socket << "\n";
        }
        //reading from the socket
        valread = read(new_socket, buffer1, max_buffer_size);
        buffer1[valread] = '\0';
        HandleRequest(new_socket, valread, buffer1);


    }
}
