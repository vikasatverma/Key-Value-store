#include "header.hpp"

// to convert the plain text to xml format
std::string plaintoxml(std::string msg_type, std::string key, std::string value = "") {

    std::string request = "<?xml version='1.0' encoding='UTF-8'?>\n";

    if (msg_type == "GET") {
        msg_type = "<KVMessage type='getreq'>\n";
        key = "<Key>" + key + "</Key>\n";
    }
    if (msg_type == "PUT") {

        msg_type = "<KVMessage type='putreq'>\n";
        key = "<key>" + key + "</Key>\n";
        value = "<Value>" + value + "</Value>\n";
        key = key + value;

    }
    if (msg_type == "DEL") {

        msg_type = "<KVMessage type='delreq'>\n";
        key = "<Key>" + key + "</Key>\n";
    }

    request = request + msg_type + key + "</KVMessage>\n";

    return request;
}

//to get back xml format to plain text

std::string xmltoplain(std::string str)
{
  int i=0;
  std::string plain_text="";
  if(str[64]=='M')
  {
  	for(i=72;str[i]!='<';i++)
    	plain_text +=str[i];
  }
  else
  {
    for(i=68;str[i]!='<';i++)
    	plain_text+=str[i];
    plain_text+=" ";
    int j=i+14;
    for(;str[j]!='<';j++)
    	plain_text+=str[j];

  }
  return plain_text;
}
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
    std::string key;
    std::string value;
    char buffer1[max_buffer_size] = {0};
    int valread;
    std::string finalRequest;

    for (std::string line; getline(infile, line);) {
        finalRequest = "";
        std::vector<std::string> request = split(line.c_str(), ',');
        if (debugger_mode) {
            cout << request[0] << "\t" << request[1] << "\t";
        }
        request_type = request[0];
        key = request[1];
//        finalRequest.append(request[0]).append(delimiter).append(request[1]);
        if (request[0] == "PUT") {
            if (debugger_mode) {
                cout << request[2];
            }
            checkLenght(request[1], request[2]);
            value = request[2];
//            finalRequest.append(delimiter).append(request[2]);
        } else if (request[0] != "GET" && request[0] != "DEL") {
            cout << "Unknown Error: Undefined finalRequest type";
            exit(-1);
        }

        if (debugger_mode) {
            cout << "\n";
        }
        finalRequest = plaintoxml(request_type, key, value);
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

        valread = read(sockfd, buffer1, max_buffer_size);
        buffer1[valread] = '\0';
        

        std::string buffer;
        for (int i = 0; i < valread; i++) {
            buffer += (buffer1[i]);
        }
        
        std::string buffer2 = xmltoplain(buffer);

        char chararr_of_buffer[buffer2.length() + 1];
        strcpy(chararr_of_buffer, buffer2.c_str());
        cout << chararr_of_buffer<< "\n";
        FILE *fp = fopen("response.txt", "a");
        if (!fp) {
            return -errno;
        }
        fprintf(fp, "%s", chararr_of_buffer);
        fprintf(fp, "\n");

        fclose(fp);

    }

}
