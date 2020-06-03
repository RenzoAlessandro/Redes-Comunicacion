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
#include "funciones.h"
#include <termios.h>
#include <vector>
#include <utility>

#define TAM_MSG 1000

using namespace std;
int SocketFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
int Res;
string cierre = "adios";
string userID;
char  p[100][100];
int c,f,x=0;


int id=0;
int posx=0,posy=0;
vector<pair<int,Point>> pos_players;
pair<bool,Point> fruna(false,Point());

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
    stSockAddr.sin_port = htons(45550);
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
    char c;
    c = getch();

    //envio de camio de posicion
    //formato: 1 + id + (1 up || 2 down || 3 left || 4 right) = 3 bites
    if(c == 'w')
    {
      string final = "1";
      final += to_string(id);
      final += "1";

      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
      //cout<<"tecla: "<<final<<endl;
    }
    if(c == 's')
    {
      string final = "1";
      final += to_string(id);
      final += "2";
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
      //cout<<"tecla: "<<final<<endl;
    }
    if(c == 'a')
    {
      string final = "1";
      final += to_string(id);
      final += "3";
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
      //cout<<"tecla: "<<final<<endl;
    }
    if(c == 'd')
    {
      string final = "1";
      final += to_string(id);
      final += "4";
      strcpy(buffer,final.c_str());
      buffer[final.size()] = '\0';
      n = write(SocketFD,buffer,final.size());
      //cout<<"tecla: "<<final<<endl;
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
    //inicializacion obtenemos id del server y la posicion inicial del jugador asi como la lista de jugadores existentes
    //formato: 0 + id_asignado(1) + cantidad_players(1) + posxposy(4)*cantidad_players = 7+4*cantidad_players bites
    if(buffer[0] == '0')
    {
      //leemos el id asigando por el server al jugador
      n = read(SocketFD,buffer,1);
      buffer[n] = '\0';
      id = atoi(buffer);

      //leemos la antidad de players actual
      int cant_players;
      n = read(SocketFD,buffer,1);
      buffer[n] = '\0';
      cant_players = atoi(buffer);

      for(int i=0;i< cant_players;i++)
      {
        //leemos la posiciones x e y de los players existentes
        n = read(SocketFD,buffer,2);
        buffer[n] = '\0';
        posx = atoi(buffer);

        n = read(SocketFD,buffer,2);
        buffer[n] = '\0';
        posy = atoi(buffer);
        pos_players.push_back(make_pair(i,Point(posx,posy)));
      }
    }
    //actualiza las posiciones del propio jugador
    //formato: 1 + pos(x) + pos(y) = 5 bites
    if(buffer[0] == '1')
    {

      n= read(SocketFD,buffer,4); // read size

      char num[2];

      num[0] = buffer[0];
      num[1] = buffer[1];
      posx = atoi(num);
      pos_players[id].second.x = atoi(num);
      num[0] = buffer[2];
      num[1] = buffer[3];
      posy = atoi(num);
      pos_players[id].second.y = atoi(num);

    }
    //actualizacion de las posiciones de los demas jugadores
    //formato: 3 + id_player(1) + pos_p(2) + pos_p(2) = 6 bites
    if(buffer[0] == '3')
    {
      n= read(SocketFD,buffer,1);
      buffer[n] = '\0';

      int id_aux = atoi(buffer);

      n= read(SocketFD,buffer,4);
      char num[2];
      num[0] = buffer[1];
      num[1] = buffer[2];
      pos_players[id_aux].second.x = atoi(num);
      num[0] = buffer[3];
      num[1] = buffer[4];
      pos_players[id_aux].second.y = atoi(num);
    }
    //se agrega un nuevo jugador
    //4 + id(1) + posx_p(2) + posy_p(2) = 5 bites
    if(buffer[0] == '4')
    {
      n= read(SocketFD,buffer,1);
      buffer[n] = '\0';
      bool existe = false;
      int id_aux = atoi(buffer);
      char num[2];

      n= read(SocketFD,buffer,4);
      num[0] = buffer[0];
      num[1] = buffer[1];
      pos_players.push_back(make_pair(id_aux,Point()));
      pos_players[id_aux].second.x = atoi(num);
      num[0] = buffer[2];
      num[1] = buffer[3];
      pos_players[id_aux].second.y = atoi(num);
    }
    //el jugador lee la fruta enviada por el server
    // 6(1) + posx_fresa(2) + posy fresa(2) = 5
    if(buffer[0] == '6')
    {

      char num[2];

      n= read(SocketFD,buffer,4);
      num[0] = buffer[0];
      num[1] = buffer[1];
      fruna.second.x = atoi(num);
      num[0] = buffer[2];
      num[1] = buffer[3];
      fruna.second.y = atoi(num);
      fruna.first = true;
    }
    //el server envia OK
    else if(buffer[0] == '5')
    {
      //cout << "OK" << endl;
    }
//----------------------------

//-----------------------------


  } while(termino);
}


int main()
{
  //-----------------nickname ----------------------------------------------------//
  string nickname;
  cout << "Nickname: ";
  cin >> nickname;

  string final = "0";
  final += int_to_char_spaces(nickname.size(),2);
  final += nickname;
  strcpy(buffer,final.c_str());
  buffer[final.size()] = '\0';
  n = write(SocketFD,buffer,final.size());
  //-----------------nickname ----------------------------------------------------//

  //write(SocketFD,"1",1);
  strcpy(p[x++],"||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
  strcpy(p[x++],"||||||||||||||||||||||||||||||||  |||||||||        |||||||||");
  strcpy(p[x++],"|||||| ||||||||||||||||||||||||    ||||||||        |||||||||");
  strcpy(p[x++],"|||||   ||||||||||||||||||||||      ||||||||        ||||||||");
  strcpy(p[x++],"||||     ||||||||||||||| ||||        |||||||||       ||  | |");
  strcpy(p[x++]," ||      ||||||||| ||||   |||         |||||||            |  ");
  strcpy(p[x++]," |        ||||||    ||    |||         ||||||                ");
  strcpy(p[x++],"          |||||            |          ||||||                ");
  strcpy(p[x++],"           |||                         ||||                 ");
  strcpy(p[x++],"           ||                          ||||                 ");
  strcpy(p[x++],"                                        ||                  ");
  strcpy(p[x++],"                                        ||                  ");
  strcpy(p[x++],"                                                            ");
  strcpy(p[x++],"                                --                          ");
  strcpy(p[x++],"      |                        |##|                    |    ");
  strcpy(p[x++],"     |#|                      |####|            |      |    ");
  strcpy(p[x++],"    |###|               |    |######|          |#|____|#|   ");
  strcpy(p[x++],"   |####|         |    |#|   |#######|       |###########|  ");
  strcpy(p[x++],"  |######|      |##|  |##|   |########|      |############| ");
  strcpy(p[x++],"|########|     |####-|####| |########|      |##############|");
  strcpy(p[x++],"##########|   |###########||###########|    |###############");
  strcpy(p[x++],"##########|  |#########################|    |###############");
  strcpy(p[x++],"###########-|###########################|  |################");
  strcpy(p[x++],"########################################|  |################");


    std::thread hilo1(writing);
    std::thread hilo2(reading);


        do
        {
          for(f=0;f<=24;f++)
          {
           p[f][60]=p[f][0];
          }
          for(f=0;f<=24;f++)
          {
              for(c=0;c<=60;c++)
              {

                if(p[f][c+1] != 'O' and p[f][c+1] !='F')
                 p[f][c]=p[f][c+1];

              }
          }
          //dibujar cosas en el mapa
          for(int i=0;i<pos_players.size();i++)
          {
            //cout<< "pos: " << pos_players[i].second.y << " " <<pos_players[i].second.x << endl;
              p[pos_players[i].second.y][pos_players[i].second.x] = 'O';
              if(pos_players[i].second.y == fruna.second.y and pos_players[i].second.x == fruna.second.x)
                {
                  fruna.first = false;
                }
          }
          //fruta
          if(fruna.first)
            p[fruna.second.y][fruna.second.x] = 'F';
          for(f=0;f<=24;f++){
             printf("%s\n",p[f]);
          }


          usleep(500000);

        }while(1);

    hilo1.join();
    hilo2.join();





    shutdown(SocketFD, SHUT_RDWR);

    close(_socket);

	return 0;
}
