#include<iostream>
#include"header.hpp"
void thread_func(int fd_index)
{

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

                /*******************************************************/
                /* If the close_conn flag was turned on, we need       */
                /* to clean up this active connection. This            */
                /* clean up process includes removing the              */
                /* descriptor.                                         */
                /**************************************************/
                    close(fds[fd_index].fd);
                    fds[fd_index].fd = -1;
                    compress_array = True;

               
}