#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <ctime>
#include <cstring>

const char* SOCKET_PATH = "/tmp/hw1_socket";

std::string getCurrentTime() {
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un address;

   
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);
    
   
    unlink(SOCKET_PATH); 

   
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

  
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << SOCKET_PATH << "...\n";

  
    while (true) {
        if ((client_fd = accept(server_fd, NULL, NULL)) < 0) {
            perror("accept failed");
            continue;
        }

        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);
        std::string request(buffer);
        
        std::cout << "[Server] Received: " << request << "\n";

        std::string response; 
        if (request == "TIME") {
            response = getCurrentTime();
        } else if (request.find("ECHO ") == 0) {
            response = request.substr(5) + "\n"; 
        } else {
            response = "Unknown Command\n";
        }

  
        send(client_fd, response.c_str(), response.length(), 0);
        close(client_fd); 
    }
    close(server_fd);
    return 0;
}
