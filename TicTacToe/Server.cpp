  // Copyright 2020 Renzo Sucari Velasquez
#include <thread>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

using namespace std;

vector <int> sockets_jugadores;
string tablero[3][3];

void llenar_tablero()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            tablero[i][j] = " ";
        }        
    }    
}

string convertir_tablero()
{
    string tab = "Tablero: ";
    tab += "\n"; 
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if(j < 2)
                tab += tablero[i][j] + "|";  
            else
            {
                tab += tablero[i][j];
            }            
        }
        tab += "\n"; 
    }
    return tab;
}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void mandar_tablero()
{
    for (int i = 0; i < sockets_jugadores.size(); i++)
    {
        string tab = convertir_tablero();
        write(sockets_jugadores[i], tab.c_str(), tab.size());
    }    
}

void movimientos(char symb, int x, int y)
{
    string symbol(1, symb);
    tablero[x][y] = symbol;
}

bool Ganador()
{
    if(tablero[0][0] != " " && (tablero[0][0] == tablero[0][1]) && (tablero[0][0] == tablero[0][2]))
        return true;
    if(tablero[1][0] != " " && (tablero[1][0] == tablero[1][1]) && (tablero[1][0] == tablero[1][2]))
        return true;
    if(tablero[2][0] != " " && (tablero[2][0] == tablero[2][1]) && (tablero[2][0] == tablero[2][2]))
        return true;
    if(tablero[0][0] != " " && (tablero[0][0] == tablero[1][0]) && (tablero[0][0] == tablero[2][0]))
        return true;
    if(tablero[0][1] != " " && (tablero[0][1] == tablero[1][1]) && (tablero[0][1] == tablero[2][1]))
        return true;
    if(tablero[0][2] != " " && (tablero[0][2] == tablero[1][2]) && (tablero[0][2] == tablero[2][2]))
        return true;
    if(tablero[0][0] != " " && (tablero[0][0] == tablero[1][1]) && (tablero[0][0] == tablero[2][2]))
        return true;
    if(tablero[0][2] != " " && (tablero[0][2] == tablero[1][1]) && (tablero[0][2] == tablero[2][0]))
        return true;
    
    return false;
}

void attend_client(int socket_fd)
{
    sockets_jugadores.push_back(socket_fd);
    bool status = true;
    while (status)
    {        
        char pos[3];
        read(socket_fd, pos, 3);
        int Pos_x = pos[1]-'0';
        int Pos_y = pos[2]-'0';
        std::cout << "Movimiento de "<< pos[0] <<": "<< Pos_x << " - "<< Pos_y<< std::endl;
        movimientos(pos[0], Pos_x, Pos_y);
        if(Ganador())
            write(socket_fd, "GANASTE", sizeof("GANASTE"));
        mandar_tablero();
    }
    close(socket_fd);
}

int main() 
{
    llenar_tablero();
    int socket_fd, newsocket_fd, port_number = 45502;

    struct sockaddr_in serv_addr, cli_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);    

    socklen_t clilen = sizeof(cli_addr);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
        error("Error abriendo el socket del servidor");

    if(bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Error de bind");
    listen(socket_fd, 5);

    while (true)
    {
        newsocket_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsocket_fd < 0)
            error("Error abriendo el socket del cliente");
        thread th(attend_client, newsocket_fd);
        th.detach();
    }


    shutdown(socket_fd, SHUT_RDWR);

    close(socket_fd);
    return 0; 
}
