// Copyright 2020 Renzo Sucari Velasquez
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <string.h>
#define TAM_MSG 1000

using namespace std;
int SocketFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
int Res;
string userID;


int crearSocket()
{
  struct sockaddr_in stSockAddr;

  if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45501);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }


  return SocketFD;
}

int _socket = crearSocket();
int n;

char buffer[TAM_MSG];
bool termino = true;


void writing()
{
  string mssg;
  do
  {
    bzero(buffer,TAM_MSG);
    mssg ="";
    getline(cin,mssg);
    //mssg = "[" + userID + "] " + mssg;
    //int aux = mssg.size();
    //strcpy(buffer,mssg.c_str());
    //buffer[mssg.size()] = '\0';
    //n = write(SocketFD,buffer,aux);
    if(mssg == "nickname")
    {
      string nick_name,nick_size;
      cout << "nick: ";
      cin >> nick_name;
      nick_size = nick_name.size() <= 9 ? nick_size = "0" + to_string(nick_name.size()) : nick_size = to_string(nick_name.size());
      string final = "1" + nick_size + nick_name;
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
    }
    if(mssg == "list")
    {
      buffer[0] = '2';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    else if(mssg == "msg")
    {
      string nick_dest,_msg, nick_size,msg_size;
      cout << "nick destinatario: ";
      cin >> nick_dest;
      nick_dest.size() <= 9 ? nick_size = "0" +to_string(nick_dest.size()):nick_size =to_string(nick_dest.size());

      cout << "mensaje: ";
      while(!_msg.size())
      {
        getline(cin,_msg);
      }
      _msg.size() <= 9 ? msg_size = "00" + to_string(_msg.size()) :
      _msg.size() <= 99 ? msg_size = "0" + to_string(_msg.size()) : msg_size = to_string(_msg.size());

      string final = "3" + nick_size + nick_dest + msg_size + _msg;
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
    }
    else if(mssg == "exit")
    {
      buffer[0] = '8';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
      termino=false;
    }



  } while(termino/*int(mssg.find(cierre)) < 0*/);

}

void reading()
{
  string mssg;

  do
  {
    bzero(buffer,TAM_MSG);
    n = read(SocketFD,buffer,1);
    //cout << buffer << endl;
    mssg = buffer;

    if(buffer[0] == '4') // mensaje de error por parte del server
    {
      n= read(SocketFD,buffer,3); // read size
      buffer[n]='\0';
      int tamano = atoi(buffer);

      n = read(SocketFD,buffer,tamano); // read nickname
      buffer[n]='\0';
      cout << buffer << endl;
    }
    else if(buffer[0] == '5') //el server envia OK
    {
      cout << "Correcto" << endl;
    }
    else if(buffer[0] == '6') //lista de nicknames
    {
      n = read(SocketFD,buffer,3); // read size
      buffer[n]='\0';
      int tamano = atoi(buffer);
      cout <<"cantidad de gente: " <<  tamano << "\n";

      n = read(SocketFD,buffer,280); // read nickname
      cout << buffer << endl;
    }
    else if(buffer[0] == '7') //enviar un mensaje de X cliente
    {
      n = read(SocketFD,buffer,2);
      buffer[n]='\0';
      int tamano = atoi(buffer);
      n= read(SocketFD,buffer,tamano);
      cout <<  "[" << buffer << "]: ";

      n=read(SocketFD,buffer,3);
      buffer[n] = '\0';
      int msm_size = atoi(buffer);
      n=read(SocketFD,buffer,msm_size);
      buffer[n] = '\0';
      cout << buffer << endl;

      bzero(buffer,TAM_MSG);
      buffer[0] = '5';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
  } while(termino);
}



int main()
{
    std::cout << endl;
    std::cout << "\t    MULTICHAT (CLIENT - CLIENT)" << std::endl;
    std::cout << "\t    ---------------------------" << std::endl << std::endl;
    std::cout << "\t- COMANDOS:" << std::endl;
    std::cout << "\t- nickname : Para registrase." << std::endl;
    std::cout << "\t- list     : Listar todos los usuarios." << std::endl;
    std::cout << "\t- msg      : Enviar un mensaje." << std::endl;
    std::cout << "\t- exit     : Para salir." << std::endl << std::endl;
    std::cout << "Ingrese comando:"<< std::endl;

    std::thread hilo1(writing);
    std::thread hilo2(reading);
    hilo1.join();
    hilo2.join();

    shutdown(SocketFD, SHUT_RDWR);

    close(_socket);

	return 0;
}
