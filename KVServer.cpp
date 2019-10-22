#include "header.hpp"
#include <pthread.h> 
#include <sys/poll.h>
#include <sys/time.h>
#include <signal.h>
#include <bits/stdc++.h>

using namespace std;

#define NUM_OF_CONN 200
#define NUM_OF_WORKERS 4


pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER; 
// pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER; 


//wait syntax
//pthread_cond_wait(&cond1, &lock1); 

//signal syntax
//pthread_cond_signal(&cond1); 

//lock1 syntax
//pthread_mutex_lock(&lock1);

//unlock syntax
//pthread_mutex_unlock(&lock1);

int worker_thread_id[NUM_OF_WORKERS]={0};
pthread_t worker_thread[NUM_OF_WORKERS];

int fds[200];
struct pollfd pollfds[200];

queue< pair<int,string> > client_requests;

map<string, string>

// Used by KVStore function such as dumpToFile and RestoreFromFile to decide which file to refer.
std::string getFilename(std::string key) {

    int fileNumber;

    if (key.length() > 1)
        fileNumber = int(key[1]) % numSetsInCache;
    else
        fileNumber = int(key[0]) % numSetsInCache;

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
}

std::string toXML(std::string str) {
    std::string response, key, value;
    std::string header = "<?xml version='1.0' encoding='UTF-8'?>\n";
    std::string msg = "<KVMessage type='resp'>\n";
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

void process(string buffer, int thread_id, int new_socket){

		pthread_mutex_lock(&lock2);

		cout<<"thread_id is"<<thread_id<<"\n";
        // if (debugger_mode) {
        //     cout << buffer1 << "\n";
        // }

        // std::string buffer;
        // for (int i = 0; i < valread; i++) {
        //     buffer += (buffer1[i]);
        // }
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
//            cacheMap[key] = value;
            std::map<std::string, std::string> tmp_map;
            restoreFromFile(key, &tmp_map);
            tmp_map[key] = value;
            dumpToFile(key, &tmp_map);
            response = "Success";


        }

        else if (request_type == "DEL") {
            std::map<std::string, std::string> tmp_map;
            restoreFromFile(key, &tmp_map);
            if (cacheMap[key].empty() && tmp_map[key].empty()) {
                response = "Does not exist";
            } else {
                tmp_map.erase(key);
                dumpToFile(key, &tmp_map);
                cacheMap.erase(key);
                response = "Success";
            }

        } else if (request_type == "GET") {
            if (cacheMap[key].empty()) {
                std::map<std::string, std::string> tmp_map;
                restoreFromFile(key, &tmp_map);
                if (tmp_map[key].empty()) {
                    response = "Does not exist";
                } else {
                    cacheMap[key] = tmp_map[key];
                    response = key + " " + cacheMap[key];
                    cout << "1" << response;
                }
            } else {
                cout << "2" << response;
                response = key + " " + cacheMap[key];
            }
        } else {
            response = error_msg;
        }
        cout << std::endl;
        response = toXML(response);
        strcpy(return_value, response.c_str());
        cout << return_value;
        send(new_socket, return_value, sizeof(return_value), 0);
        if (add_pair_to_KVStore_flag) {
            putIntoFile(key, value);
        }

        pthread_mutex_unlock(&lock2);

}

void* thread_fun( void * th_id){

	int thread_id = *((int *)th_id);


event:	if(client_requests.empty()){
		pthread_mutex_lock(&lock1);

		pthread_cond_wait(&cond1, &lock1);
	
		pthread_mutex_unlock(&lock1);
	}
    
    pthread_mutex_lock(&lock3);
    pair<int , string >p = client_requests.front();
    client_requests.pop();
    pthread_mutex_unlock(&lock3);

    int newsockfd = p.first;
    string request = p.second;

    //printf("thread_%d connected to client: %s\n",thread_id, inet_ntoa(clientAddress.sin_addr));

    process(request, thread_id, newsockfd);

    worker_thread_id[thread_id] = 0;

	goto event;    
	 
}


int main() {
//    char y_or_n;
//    cout << "Delete previously stored key-value pairs? y/n" << std::endl;
//    cin >> y_or_n;
//    if (y_or_n == 'y') {
	// try{
	//     throw system("exec rm -r KVStore/*");
	// }
	// catch(...){

	// }
 //}

    //Clear response file.
    FILE *fp = fopen("response.txt", "w");
    fclose(fp);

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

    //listening on the socket with max no. of waiting connections
    listen(server_fd, NUM_OF_CONN);
    std::map<std::string, std::string> cacheMap;
    std::string filename = "KVStore_file";

    // Read key value store from file
    restoreFromFile(filename, &cacheMap);


    for(int i = 0; i < NUM_OF_WORKERS; i++){
        if(worker_thread_id[i] == 0){
            worker_thread_id[i]=1;
            pthread_create(&worker_thread[i], NULL, thread_fun, &i);
        }
    }    

    // Server runs forever
    int cnt = -1;
    while (True) {
        //accept creates a new socket for comunication
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &(addr_len));
        if (debugger_mode) {
            cout << "connection made with client fd==========>" << new_socket << "\n";
        }

        cnt++;
        pollfds[cnt].fd = new_socket;
    	pollfds[cnt].events = POLLIN;   
        sleep(1);

  		puts("Begin Round");

		poll(pollfds, cnt, 50000);
 
		for(int i=0;i<cnt;i++) {
			if (pollfds[i].revents & POLLIN){
				pollfds[i].revents = 0;
				memset(buffer1,0,max_buffer_size);
				valread = read(pollfds[i].fd, buffer1, max_buffer_size);
			    buffer1[valread] = '\0';
			    std::string buffer;
        		for (int i = 0; i < valread; i++) {
            		buffer += (buffer1[i]);
        		}    
			    client_requests.push(make_pair(pollfds[i].fd, buffer));
			    pthread_cond_signal(&cond1);		
				//puts(buffer);

				}
			}

    }
    
}

        

       			
 	  
//######################################################################################################
        //reading from the socket
        //valread = read(new_socket, buffer1, max_buffer_size);
