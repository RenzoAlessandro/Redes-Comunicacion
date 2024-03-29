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


using namespace std;
int SocketFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
int Res;
string cierre = "salir";

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
    stSockAddr.sin_port = htons(1130);
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

char buffer[256];

void writing()
{
  string mssg;
  do
  {
    bzero(buffer,256);
    getline(cin,mssg);
    int aux = mssg.size();
    strcpy(buffer,mssg.c_str());
    n = write(SocketFD,buffer,aux);

  } while(int(mssg.find(cierre)) < 0);

}
void reading()
{
  string mssg;
  do
  {

    bzero(buffer,256);
    n = read(SocketFD,buffer,256);
    cout<<buffer<<endl;
    mssg = buffer;
  } while(int(mssg.find(cierre)) < 0);
}



int main()
{
    std::cout << endl;
    std::cout << "\t    CALCULADORA (SERVER - CLIENT)" << std::endl;
    std::cout << "\t    -----------------------------" << std::endl << std::endl;
    std::cout << "\t- IMPORTANTE:" << std::endl;
    std::cout << "\t- Formato: [Numero1][espacio][Numero2][espacio][Operacion]" << std::endl;
    std::cout << "\t- Operaciones: [+] [-] [*] [/]" << std::endl;
    std::cout << "\t- Los numeros pueden tener 1,2,3,4... cifras, sin necesidad \n\t  de rellenar de ceros." << std::endl;
    std::cout << "\t- Ejemplo: 456 34 +" << std::endl << std::endl;
    std::cout << "\t- Para salir: salir" << std::endl << std::endl;
    std::cout << "Ingrese sus operaciones:"<< std::endl;

    std::thread hilo1(writing);
    std::thread hilo2(reading);
    hilo1.join();
    hilo2.join();

    shutdown(SocketFD, SHUT_RDWR);

    close(_socket);

	return 0;
}
