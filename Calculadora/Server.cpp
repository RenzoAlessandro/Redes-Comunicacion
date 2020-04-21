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
  #include <string>
  #include <string.h>

 using namespace std;
 int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 string cierre = "salir";
 string userID;

 int crearSocket()
  {
     struct sockaddr_in stSockAddr;
     if(-1 == SocketFD)
     {
       perror("can not create socket");
       exit(EXIT_FAILURE);
     }

     memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

     stSockAddr.sin_family = AF_INET;
     stSockAddr.sin_port = htons(1130);
     stSockAddr.sin_addr.s_addr = INADDR_ANY;

     if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
     {
       perror("error bind failed");
       close(SocketFD);
       exit(EXIT_FAILURE);
     }

     if(-1 == listen(SocketFD, 10))
     {
       perror("error listen failed");
       close(SocketFD);
       exit(EXIT_FAILURE);
     }
     return SocketFD;
   }


 int _socket = crearSocket();
 int ConnectFD = accept(SocketFD, NULL, NULL);
 char buffer[256];
 //string buffer ="";
 int n;

    string ProtocoloCalculadoraString(string mensaje)
    {
      string PrimerNumero, SegundoNumero, SimboloOperacion, Resultado = "";
      string str2 (" ");

      size_t PrimerEspacio = mensaje.find(str2);
      size_t SegundoEspacio = mensaje.find(" ",PrimerEspacio+1);
      PrimerNumero.append(mensaje,0,PrimerEspacio);
      SegundoNumero.append(mensaje,PrimerEspacio+1,SegundoEspacio-PrimerEspacio-1);
      SimboloOperacion.append(mensaje,SegundoEspacio+1,1);
      int Primer = std::stoi(PrimerNumero);
      int Segundo = std::stoi(SegundoNumero);
      std::cout <<"Primer Numero: "<<Primer<< std::endl;
      std::cout <<"Segundo Numero: "<<Segundo<< std::endl;
      std::cout <<"Operacion: "<<SimboloOperacion<< std::endl;
      if (SimboloOperacion == "+")
      {
        Resultado = "La suma es: " + std::to_string(Primer + Segundo); 
      }
      if (SimboloOperacion == "-")
      {
        Resultado = "La resta es: " + std::to_string(Primer - Segundo); 
      }
      if (SimboloOperacion == "*")
      {
        Resultado = "La multiplicacion es: " + std::to_string(Primer * Segundo); 
      }
      if (SimboloOperacion == "/")
      {
        Resultado = "La division es: " + std::to_string(Primer / Segundo); 
      }
      return Resultado;
    }

  void writing()
  {
    string mssg = "";
    getline(cin,mssg);
    std::cout <<"No tienes permitido mandar mensajes."<< std::endl;
  }

  void reading()
  {
    string mssg;
    do
    {
      bzero(buffer,256);
      n = read(ConnectFD,buffer,256);
      mssg = buffer;
      string final = "[" + userID + "] " + ProtocoloCalculadoraString(mssg);
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(ConnectFD,buffer,final.size());
    }
    while(int(mssg.find(cierre)) < 0);
  }


  int main()
  {
    userID = "Calculadora";

    std::thread hilo1(reading);
    std::thread hilo2(writing);
    hilo1.join();
    hilo2.join();

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD);

    close(_socket);
    return 0;
  }
