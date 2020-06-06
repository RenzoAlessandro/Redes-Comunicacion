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
  #include <utility>
  #include <string.h>
  #include <vector>

  #define TAM_MSG 1000
  using namespace std;
  string cierre = "adios";
  string userID;

  vector<pair<string,int>> list_users;


 int crearSocket(int &SocketFD1)
  {
      SocketFD1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
      int SocketFD2 = SocketFD1;
     struct sockaddr_in stSockAddr;
     if(-1 == SocketFD2)
     {
       perror("can not create socket");
       exit(EXIT_FAILURE);
     }

     memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

     stSockAddr.sin_family = AF_INET;
     stSockAddr.sin_port = htons(45501);
     stSockAddr.sin_addr.s_addr = INADDR_ANY;

     if(-1 == bind(SocketFD2,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
     {
       perror("error bind failed");
       close(SocketFD2);
       exit(EXIT_FAILURE);
     }

     if(-1 == listen(SocketFD2, 10))
     {
       perror("error listen failed");
       close(SocketFD2);
       exit(EXIT_FAILURE);
     }
     return SocketFD2;
   }


void cli(int &ConnectFD,int &SocketFD1,int &_socket)
{
  _socket = crearSocket(SocketFD1);
}


int n;
void writing(int ConnectFD, char buffer[])
  {

    string mssg = "";
    do
    {
      bzero(buffer,TAM_MSG);
      getline(cin,mssg);
      if(mssg =="exit")
        break;
      mssg = "[" + userID + "] " + mssg;
      int aux = mssg.size();
      strcpy(buffer,mssg.c_str());
      n = write(ConnectFD,buffer,aux);

   }
   while(int(mssg.find(cierre)) < 0);
}

void reading(int ConnectFD, char buffer[])
{
  string mssg;
  do
  {

    bzero(buffer,TAM_MSG);
    n=read(ConnectFD,buffer,1);
    mssg = buffer;

    if(buffer[0] == '1' ) 
    {
      string final;
      final = "1w";
      bzero(buffer,TAM_MSG);
      strcpy(buffer,final.c_str());
      for(int i=0;i<list_users.size();i++)
      {
        write(list_users[i].second,buffer,final.size());
      }
    }
    else if(buffer[0] == '2' ) 
    {
      string final;
      final = "2s";
      bzero(buffer,TAM_MSG);
      strcpy(buffer,final.c_str());
      for(int i=0;i<list_users.size();i++)
      {
        write(list_users[i].second,buffer,final.size());
      }
    }
    else if(buffer[0] == '3' ) 
    {
      string final;
      final = "3a";
      bzero(buffer,TAM_MSG);
      strcpy(buffer,final.c_str());
      for(int i=0;i<list_users.size();i++)
      {
        write(list_users[i].second,buffer,final.size());
      }
    }
    else if(buffer[0] == '4' ) 
    {
      string final;
      final = "4d";
      bzero(buffer,TAM_MSG);
      strcpy(buffer,final.c_str());
      for(int i=0;i<list_users.size();i++)
      {
        write(list_users[i].second,buffer,final.size());
      }
    }
    // Salir
    else if(buffer[0] == '8')
    {
      bzero(buffer,TAM_MSG);
      //quitamos el cliente de la lista
      for(int i=0;i<list_users.size();i++)
      {
        if(ConnectFD == list_users[i].second)
        {
          list_users.erase(list_users.begin()+i);
          break;
        }
      }

      buffer[0] = '5';
      buffer[1] = '\0';
      n = write(ConnectFD,buffer,1);

      shutdown(ConnectFD, SHUT_RDWR);
      close(ConnectFD);
      break;
    }
  }
  while(int(mssg.find(cierre)) < 0);

}

int main()
{
  userID = "server";

  int socketFD1;
  int ConnectFD;
  int _socket;

  cli(ConnectFD,socketFD1,_socket);

  char buffer[TAM_MSG];

  for(;;){
    int ConnectClient = accept(socketFD1, NULL, NULL);
    bzero(buffer,TAM_MSG);
    //inicializando comando NICK
    read(ConnectClient,buffer,1); // leemos command
    if ( buffer[0] == '1' )
    {
      n= read(ConnectClient,buffer,2);  // leemos size
      buffer[n]='\0';
      int tamano = atoi(buffer);
      n= read(ConnectClient,buffer,tamano); // leemos nickname
      buffer[n]='\0';
      list_users.push_back(make_pair(buffer,ConnectClient));
      std::thread (reading,ConnectClient,buffer).detach();
      //std::thread (writing,ConnectClient,buffer).detach();
      bzero(buffer,TAM_MSG);
      buffer[0] = '5';
      write(ConnectClient,buffer,1); //devolucion OK;
      //shutdown(ConnectClient, SHUT_RDWR);
      //close(ConnectClient);
    }
    //  ------------------------------------------------------
    // comando list-------------------------------------------
    else {
      string error = "ingrese primero comando: 'nickname'";
      string final = "4035" + error;
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(ConnectClient,buffer,final.size());
    }


  }
  shutdown(ConnectFD, SHUT_RDWR);
  close(ConnectFD);
  close(_socket);
  return 0;
}
