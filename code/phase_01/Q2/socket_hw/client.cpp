#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

const char* SOCKET_PATH = "/tmp/hw1_socket";

int main(int argc, char const *argv[]) {
 
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Command> [Arguments]\n";
        std::cerr << "Commands:\n  TIME\n  ECHO <text>\n";
        return -1;
    }

  
    std::string request = argv[1];
    for (int i = 2; i < argc; ++i) {
        request += " ";
        request += argv[i];
    }

    int sock = 0;
    struct sockaddr_un serv_addr;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCKET_PATH);

   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed. Is the server running?\n";
        return -1;
    }


    send(sock, request.c_str(), request.length(), 0);

  
    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    std::cout << buffer;

    close(sock);
    return 0;
}
