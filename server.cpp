#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

int main(int argc, char **argv){
    //Check to make sure there are enough arguments
    if (argc < 2){
        cout << "Incorrect input format!" << endl;
        cout << "Correct input: <executable file name> <port number>" << endl;
        return 0;
    }
    //take the second element of arg vector. First element is name of executable
    int port = atoi(argv[1]);

    //1. Socket. This will be a tcp streaming socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //2. Bind
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    int bind_value = bind(sock, (struct sockaddr*)&address, sizeof(address));
    if (bind_value == -1){
        cerr << "Could not bind! Unable to bind to requested port number";
        return -1;
    }

    //3. Listen
    int listen_value = listen(sock, 1);
    if (listen_value == -1){
        cerr << "Error! Could not listen.";
        return -2;
    }

    //4. Accept
    struct sockaddr_in remote_address;
    memset(&remote_address, 0, sizeof(remote_address));
    socklen_t remote_addrlen = sizeof(address);
    cout << "Waiting for new connection... " << endl;
    int client_socket = accept(sock, (struct sockaddr*)&remote_address, &remote_addrlen);
    if (client_socket == -1){
        cout << "Error! Could not accept!";
        return -3;
    }

    std::string client_ip = inet_ntoa(remote_address.sin_addr);
    int remote_port = ntohs(remote_address.sin_port);
    cout << "Accepted new client at " << client_ip << ":" << remote_port << endl;

    int msg_buf = 1024;
    char message[msg_buf];

    while(1){
        //Clear out previous message before you receive
        memset(message, 0, msg_buf);
        //5. Receive from the client
        int bytes_received = recv(client_socket, message, msg_buf-1, 0);
        if (bytes_received == -1){
            cout << "Error! Could not receive!";
            return -4;
        }else if (bytes_received == 0){
            cout << "Client has closed its side of the connection " << endl;
            break;
        }
        cout << "Client message: " << message << endl;
    }
    close(client_socket);
}
