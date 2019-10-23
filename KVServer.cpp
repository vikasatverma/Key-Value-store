#include "header.hpp"
#include "KVCache.cpp"
#include "KVStore.cpp"
#include"linked_list.cpp"
 KVCache cacheMap;
 linked_list job_queue;
    char buffer1[max_buffer_size];
    int len, rc, on = 1;
    int server_fd = -1, new_socket = -1;
    int end_server = False, compress_array = False;
    int close_conn;

    struct sockaddr_in client_add{};
    struct pollfd fds[numOfTotalFDs];

    void check_function()
    {

    	cout<<"yes i am working \n";
    }
 void thread_function()
 {
 					//cout<<job_queue.head->request;
                    std:string buffer=job_queue.head->request;
                    int fd_index=job_queue.head->fd;
                    std::string buffer2 = fromxml(buffer);
                    //cout<<buffer2<<std::endl;
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
                    char return_value[max_buffer_size];
                    // Extract value if the request type is PUT
                    if (request_type == "PUT") {
                        value = strtok(nullptr, delimiter);
                        if (debugger_mode) {
                            cout << "Value=" << value << "\n";
                        }

                        std::map<std::string, std::string> tmp_map;

//                        populateMap(key, &tmp_map);
//                        tmp_map[key] = value;
//                        cout << "\n\n\n" << value << "\n\n\n";
//                        storeMapToFile(key, &tmp_map);
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

                    rc = send(fds[fd_index].fd, return_value, sizeof(return_value), 0);
                    if (rc < 0) {
                        perror("  send() failed");
                        close_conn = True;
                       
                    }
                    close_conn = True;

                	job_queue.del_job();

                /*******************************************************/
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                /*******************************************************/
                if (close_conn) {
                    close(fds[fd_index].fd);
                    fds[fd_index].fd = -1;
                    compress_array = True;
                }
 }
int main(int argc, char *argv[]) {

//    system("exec rm -rf KVStore/*");
//    FILE *fp = fopen("response.txt", "w");
//    fclose(fp);

	 pthread_t threadpool;
	 for(int i=0;i<threadPoolSize;i++)
	 {
	 	 pthread_create(&threadpool, NULL, reinterpret_cast<void *(*)(void *)>(check_function), NULL);
	 }

    int num_fds = 1, current_size = 0, i, j;


    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Network Error: Could not create socket");
        exit(-1);
    }


    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               (char *) &on, sizeof(on));

    ioctl(server_fd, FIONBIO, (char *) &on);


    struct sockaddr_in address = {address.sin_family = AF_INET,
            address.sin_port = htons(PORT),
            address.sin_addr.s_addr = INADDR_ANY};
    rc = bind(server_fd,
              (struct sockaddr *) &address, sizeof(address));
    if (rc < 0) {
        perror("Network Error: bind() failed");
        close(server_fd);
        exit(-1);
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    rc = listen(server_fd, numOfTotalFDs);
    if (rc < 0) {
        perror("Network Error: listen() failed");
        close(server_fd);
        exit(-1);
    }
   
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
                if (debugger_mode) {
                    printf("  Error! revents = %d\n", fds[i].fd);
                }
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
                            perror("Network Error: accept() failed");
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


                

                    rc = read(fds[i].fd, buffer1, max_buffer_size);
                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Network Error: read() failed");
                            close_conn = True;
                        }
                        
                    }

                    if (rc == 0) {
                        if (debugger_mode) {
                            printf("  Connection closed\n");
                        }
                        close_conn = True;
                        
                    }

                    if (debugger_mode) {
                        printf("  %d bytes received\n", len);
                    }
               
                    buffer1[rc] = '\0'; 
                    std::string buffer;
                    for (int i = 0; i < rc; i++) {
                        buffer += (buffer1[i]);
                    }
                   job_queue.add_job(i,buffer);
                   //cout<<job_queue.head->request;
                   thread_function();


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

