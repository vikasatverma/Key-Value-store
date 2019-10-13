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
    std::ifstream infile("batchRun.txt");
    std::string request_type;
    char buffer[max_buffer_size] = {0};
    int valread;
    std::string finalRequest;

    for (std::string line; getline(infile, line);) {
        finalRequest = "";
        std::vector<std::string> request = split(line.c_str(), ',');
        if (debugger_mode) {
            cout << request[0] << "\t" << request[1] << "\t";
        }
        finalRequest.append(request[0]).append(delimiter).append(request[1]);
        if (request[0] == "PUT") {
            if (debugger_mode) {
                cout << request[2];
            }
            checkLenght(request[1], request[2]);
            finalRequest.append(delimiter).append(request[2]);
        } else if (request[0] != "GET" && request[0] != "DEL") {
            cout << "Unknown Error: Undefined finalRequest type";
            exit(-1);
        }

        if (debugger_mode) {
            cout << "\n";
        }

        if (debugger_mode) {
            cout << finalRequest << "\n";
            cout << "###################################### "
                    "SENDING DATA "
                    "######################################\n";
        }

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

        send(sockfd, finalRequest.c_str(), finalRequest.size(), 0);

        valread = read(sockfd, buffer, max_buffer_size);
        buffer[valread] = '\0';
        cout << buffer << "\n";
        FILE *fp = fopen("response.txt", "a");
        if (!fp) {
            return -errno;
        }
        fprintf(fp, "%s", buffer);
        fprintf(fp, "\n");

        fclose(fp);

    }

}
