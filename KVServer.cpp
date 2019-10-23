#include "header.hpp"
#include "KVCache.cpp"

// Used by KVStore function such as dumpToFile and RestoreFromFile to decide which file to refer.
std::string getFilename(const std::string &key) {

    std::size_t str_hash = std::hash<std::string>{}(key);
//    std::cout << str_hash % numSetsInCache << '\n';

    int fileNumber = str_hash % numSetsInCache;

    std::string fname = "KVStore/" + std::to_string(fileNumber);

    return fname;
}

// To be used to access key value pairs when not found in cache
// this will load the required file into the temporary map ie m
int restoreFromFile(std::string &key, std::map<std::string, std::string> *m) {
    std::string fname = getFilename(key);
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

// Rewrites the whole file in case of a delete
int dumpToFile(std::string &key, std::map<std::string, std::string> *m) {
    std::string fname = getFilename(key);


    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp) {
        return -errno;
    }
    int count = 0;
    if (m->empty()) {
        fclose(fp);
        return 0;
    }


    for (std::map<std::string, std::string>::iterator it = m->begin(); it != m->end(); it++) {
        fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
        count++;
    }

//    std::cout << count;
    fclose(fp);
    return count;
}


// Inserts key-value pair incrementally
int putIntoFile(std::string &key, std::string &value) {
    std::string fname = getFilename(key);


    FILE *fp = fopen(fname.c_str(), "a");
    if (!fp) {
        return -errno;
    }
    fprintf(fp, "%s=%s\n", key.c_str(), value.c_str());

    fclose(fp);
    return 0;
}

std::string toXML(std::string str) {
    std::string response, key, value;
    std::string header = "<?xml version='1.0' encoding='UTF-8'?>\n";
    std::string msg = "<KVMessage type='resp'>\n";
    if (debugger_mode) {
        cout << "\nstr is =>" << str << "<=\n" << std::endl;
    }
    if (str == "Success" || str == "Error Message" || str == "Does not exist")
        msg = msg + "<Message>" + str + "</Message>\n";
    else {
        for (auto i = 0; i < str.length(); i++) {
            if (str[i] != ' ')
                key += str[i];
            else {
                value = str.substr(i + 1);
                break;
            }

        }

        msg = msg + "<Key>" + key + "</Key>\n" + "<Value>" + value + "</Value>\n";
    }
    response = header + msg + "</KVMessage>\n";
    return response;
}



//convert xml format to plain text
std::string fromxml(std::string str) {
    std::string request_type;
    std::string msg_type = str.substr(56, 6);
    std::string key;
    std::string value;
    int i = 0, j = 0;
    if (msg_type == "putreq") {
        request_type = "PUT";
        for (i = 70; str[i] != '<'; i++) {
            key += str[i];
        }
        j = i + 14;
        for (; str[j] != '<'; j++) {
            value += str[j];
        }
        key = key + delimiter + value;
    } else if (msg_type == "getreq") {
        request_type = "GET";
        for (i = 70; str[i] != '<'; i++) {
            key += str[i];
        }
    } else {
        request_type = "DEL";
        for (i = 70; str[i] != '<'; i++) {
            key += str[i];
        }
    }
    request_type = request_type + delimiter + key;
    return request_type;
}

int main(int argc, char *argv[]) {

    system("exec rm -rf KVStore/*");
    FILE *fp = fopen("response.txt", "w");
    fclose(fp);
    char buffer1[max_buffer_size];
    int len, rc, on = 1;
    int server_fd = -1, new_socket = -1;
    int end_server = False, compress_array = False;
    int close_conn;

    struct sockaddr_in client_add;
    struct pollfd fds[200];
    int num_fds = 1, current_size = 0, i, j;


    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket() failed");
        exit(-1);
    }


    rc = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                    (char *) &on, sizeof(on));

    rc = ioctl(server_fd, FIONBIO, (char *) &on);


    struct sockaddr_in address = {address.sin_family = AF_INET,
            address.sin_port = htons(PORT),
            address.sin_addr.s_addr = INADDR_ANY};
    rc = bind(server_fd,
              (struct sockaddr *) &address, sizeof(address));
    if (rc < 0) {
        perror("bind() failed");
        close(server_fd);
        exit(-1);
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    rc = listen(server_fd, 10);
    if (rc < 0) {
        perror("listen() failed");
        close(server_fd);
        exit(-1);
    }
    KVCache cacheMap;
    std::string filename = "KVStore_file";
    /*************************************************************/
    /* Initialize the pollfd structure                           */
    /*************************************************************/
    memset(fds, 0, sizeof(fds));

    /*************************************************************/
    /* Set up the initial listening socket                        */
    /*************************************************************/
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do {
        /***********************************************************/
        /* Call poll() and wait 3 minutes for it to complete.      */
        /***********************************************************/

        rc = poll(fds, num_fds, 5000);

        /***********************************************************/
        /* Check to see if the poll call failed.                   */
        /***********************************************************/
        if (rc < 0) {
            perror("  poll() failed");
            break;
        }

        /***********************************************************/
        /* Check to see if the 3 minute time out expired.          */
        /***********************************************************/
        if (rc == 0) {
//            printf("  poll() timed out.\n");
            continue;
        }


        /***********************************************************/
        /* One or more descriptors are readable.  Need to          */
        /* determine which ones they are.                          */
        /***********************************************************/
        current_size = num_fds;
        for (i = 0; i < current_size; i++) {
            /*********************************************************/
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the listening       */
            /* or the active connection.                             */
            /*********************************************************/
            if (fds[i].revents == 0)
                continue;

            /*********************************************************/
            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            /*********************************************************/
            if (fds[i].revents != POLLIN) {
                printf("  Error! revents = %d\n", fds[i].fd);
                end_server = True;
                break;

            }
            if (fds[i].fd == server_fd) {
                /*******************************************************/
                /* Listening descriptor is readable.                   */
                /*******************************************************/
                if (debugger_mode) {
                    printf("  Listening socket is readable\n");
                }

                do {

                    /*****************************************************/
                    int client_len = sizeof(client_add);
                    new_socket = accept(server_fd, (struct sockaddr *) &client_add, (socklen_t *) &client_len);
                    //rc=ioctl(new_socket, FIONBIO ,(char *)&on);
                    if (new_socket < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  accept() failed");
                            end_server = True;
                        }
                        break;
                    }
                    if (debugger_mode) {
                    printf("  New incoming connection - %d\n", new_socket);
                    }
                    fds[num_fds].fd = new_socket;
                    fds[num_fds].events = POLLIN;
                    num_fds++;

                } while (new_socket != -1);
            } else {
                if (debugger_mode) {
                    printf("  Descriptor %d is readable\n", fds[i].fd);
                }
                close_conn = False;
                rc = ioctl(fds[i].fd, FIONBIO, (char *) &on);


                do {

                    rc = read(fds[i].fd, buffer1, max_buffer_size);

                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  read() failed");
                            close_conn = True;
                        }
                        break;
                    }



                    /*****************************************************/
                    /* Check to see if the connection has been           */
                    /* closed by the client                              */
                    /*****************************************************/
                    if (rc == 0) {
                        printf("  Connection closed\n");
                        close_conn = True;
                        break;
                    }

                    /*****************************************************/
                    /* Data was received                                 */
                    /*****************************************************/
                    len = rc;
                    if (debugger_mode) {
                        printf("  %d bytes received\n", len);
                    }
                    /*****************************************************/
                    /* Echo the data back to the client                  */
                    /*****************************************************/
                    buffer1[rc] = '\0';
                    //cout<<buffer1;
                    if (debugger_mode) {
                        cout << buffer1 << "\n";
                    }

                    std::string buffer;
                    for (int i = 0; i < rc; i++) {
                        buffer += (buffer1[i]);
                    }
                    std::string buffer2 = fromxml(buffer);

                    char chararr_of_buffer[buffer2.length() + 1];
                    strcpy(chararr_of_buffer, buffer2.c_str());
                    //cout<<chararr_of_buffer;
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
                    int add_pair_to_KVStore_flag = 0;
                    char return_value[max_buffer_size];
                    // Extract value if the request type is PUT
                    if (request_type == "PUT") {

                        add_pair_to_KVStore_flag = 1;
                        value = strtok(nullptr, delimiter);
                        cout << "Value=" << value << "\n";
                        if (debugger_mode) {
                            cout << value << '\n';
                        }

                        std::map<std::string, std::string> tmp_map;

                        restoreFromFile(key, &tmp_map);
                        tmp_map[key] = value;
                        cout << "\n\n\n" << value << "\n\n\n";
                        dumpToFile(key, &tmp_map);
                        response = "Success";


                    } else if (request_type == "DEL") {
                        std::map<std::string, std::string> tmp_map;
                        restoreFromFile(key, &tmp_map);
                        if (cacheMap.get(key) == "Does not exist" && tmp_map[key].empty()) {
                            response = "Does not exist";
                        } else {
                            tmp_map.erase(key);
                            dumpToFile(key, &tmp_map);
                            cacheMap.del(key);
                            response = "Success";
                        }

                        /**********************************************
                         * Comment it out
                         *
                         ***********************************************/
                        restoreFromFile(key, &tmp_map);
                        cout << "\n\n\ncache has ==>" << cacheMap.get(key) << "<==\n\n\n file has==>" << tmp_map[key]
                             << "<==\n\n\n" << std::endl;



                    } else if (request_type == "GET") {
                        if (cacheMap.get(key) == "Does not exist") {
                            std::map<std::string, std::string> tmp_map;
                            restoreFromFile(key, &tmp_map);
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

                    rc = send(fds[i].fd, return_value, sizeof(return_value), 0);
                    if (rc < 0) {
                        perror("  send() failed");
                        close_conn = True;
                        break;
                    }
                    close_conn = True;

                } while (True);

                /*******************************************************/
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                /*******************************************************/
                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = True;
                }


            }
            /* End of existing connection is readable             */
        }
        /* End of loop through pollable descriptors              */

        /***********************************************************/
        /* If the compress_array flag was turned on, we need       */
        /* to squeeze together the array and decrement the number  */
        /* of file descriptors. We do not need to move back the    */
        /* events and revents fields because the events will always*/
        /* be POLLIN in this case, and revents is output.          */
        /***********************************************************/
        if (compress_array) {
            compress_array = False;
            for (i = 0; i < num_fds; i++) {
                if (fds[i].fd == -1) {
                    for (j = i; j < num_fds; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    num_fds--;
                }
            }
        }

    } while (True); /* End of serving running.    */

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (i = 0; i < num_fds; i++) {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }
}

