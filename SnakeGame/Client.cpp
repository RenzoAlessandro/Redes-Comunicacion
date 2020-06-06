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
#include <termios.h>
#include <vector>
#include <utility>



#define TAM_MSG 1000

using namespace std;
int SocketFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
int Res;
string userID;


/***************************** LECTURA DE TECLADO *****************************/

static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}


/***************************** LECTURA DE TECLADO *****************************/


/*****************************      CONEXION     *****************************/

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

/*****************************      CONEXION     *****************************/

int n;
char buffer[TAM_MSG];
bool termino = true;


void writing()
{
  string moviminetos;
  do
  {
    bzero(buffer,TAM_MSG);
    char c;
    c = getch();

    if(c == 'w')
    {
      cout <<"w"<<endl;
      buffer[0] = '1';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 's')
    {
      cout <<"s"<<endl;
      buffer[0] = '2';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);

    }
    if(c == 'a')
    {
      cout <<"a"<<endl;
      buffer[0] = '3';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);

    }
    if(c == 'd')
    {
      cout <<"d"<<endl;
      buffer[0] = '4';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 'x')
    {
      cout <<"Exit"<<endl;
      buffer[0] = '8';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
      termino=false;
    }
  } while(termino/*int(moviminetos.find(cierre)) < 0*/);
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

    if(buffer[0] == '1') //lista de nicknames
    {
      n = read(SocketFD,buffer,1); // read size
      buffer[n]='\0';
      cout << buffer << endl;
    }
    else if(buffer[0] == '2') //lista de nicknames
    {
      n = read(SocketFD,buffer,1); // read size
      buffer[n]='\0';
      cout << buffer << endl;
    }
    else if(buffer[0] == '3') //lista de nicknames
    {
      n = read(SocketFD,buffer,1); // read size
      buffer[n]='\0';
      cout << buffer << endl;
    }
    else if(buffer[0] == '4') //lista de nicknames
    {
      n = read(SocketFD,buffer,1); // read size
      buffer[n]='\0';
      cout << buffer << endl;
    }
    else if(buffer[0] == '5') //el server envia OK
    {
      cout << "Correcto" << endl;
    }

  } while(termino);
}

int main()
{
    std::cout << "\t    SNAKE GAME (CLIENT - CLIENT)" << std::endl;


    /*****************************REGISTRO DEL USUARIO *****************************/
    std::cout << "Ingrese su Nickname:";
    string nick_name,nick_size;
    cin >> nick_name;
    nick_size = nick_name.size() <= 9 ? nick_size = "0" + to_string(nick_name.size()) : nick_size = to_string(nick_name.size());
    string final = "1" + nick_size + nick_name;
    strcpy(buffer,final.c_str());
    buffer[final.size()] = '\0';
    n = write(SocketFD,buffer,final.size());
    /*****************************REGISTRO DEL USUARIO *****************************/


    std::thread hilo1(writing);
    std::thread hilo2(reading);
    hilo1.join();
    hilo2.join();

    shutdown(SocketFD, SHUT_RDWR);

    close(_socket);

	return 0;
}
