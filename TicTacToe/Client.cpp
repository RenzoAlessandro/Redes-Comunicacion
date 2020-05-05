  // Copyright 2020 Renzo Sucari Velasquez
#include <thread>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

using namespace std;

bool status = true;

void write_thread(int socket_fd)
{
    while (status)
    {
        string request;
        getline(cin, request);
        write(socket_fd, request.c_str(), request.size());
        if(request == "salir")
            status = false;
    }
}

void read_thread(int socket_fd) //lee al server para imprimir al cliente
{
    while (status)
    {
        char buffer[256];
        bzero(buffer, 256);
        read(socket_fd, buffer, 255);
        std::cout << buffer << std::endl;
    }
}

int main()
{
    std::cout << endl;
    std::cout << "\t    TICTACTOE (CLIENT - CLIENT)" << std::endl;
    std::cout << "\t    ---------------------------" << std::endl << std::endl;
    std::cout << "\t- Movimientos: [simbolo][posicion x][posicion y]" << std::endl;
    std::cout << "\t- Ejemplo    : x21" << std::endl;
    std::cout << "Ingrese sus movimientos:"<< std::endl;
    std::cout << endl;

    int port_number = 45502;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("127.0.0.1");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);


    
    connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    thread thr(read_thread, socket_fd);
    thread thw(write_thread, socket_fd);

    thr.join();
    thw.join();

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}