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
#include <ncurses.h>
#include <cstdlib>

#define TAM_MSG 1000
using namespace std;
int SocketFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
int Res;
string userID;


/***************************** ESTRUCTURA DEL SNAKE GAME *****************************/

struct snakepart{
  int x,y;
  snakepart(int col,int row)
  {
    x=col;
    y=row;
  }

  snakepart()
  {
    x=0;
    y=0;
  }
};

class snakeclass{
  int points,del;
  //indicates that the snake get food (it makes the snake longer)
  bool get;
  //indicates the current direction of the snake
  char direction;

  int maxwidth;
  int maxheight;
  char partchar;
  char oldalchar;
  char etel;
  snakepart food;
  std::vector<snakepart> snake; //represent the snake

  void putfood()
  {
    while(1)
    {
      int tmpx=rand()%maxwidth+1;
      int tmpy=rand()%maxheight+1;
      for(int i=0;i<snake.size();i++)
        if(snake[i].x==tmpx && snake[i].y==tmpy)
          continue;
      if(tmpx>=maxwidth-2 || tmpy>=maxheight-3)
        continue;
      food.x=tmpx;
      food.y=tmpy;
      break;
    }
    move(food.y,food.x);
    addch(etel);
    refresh();
  }

  bool collision()
  {
    if(snake[0].x==0 || snake[0].x==maxwidth-1 || snake[0].y==0 || snake[0].y==maxheight-2)
      return true;
    for(int i=2;i<snake.size();i++)
    {
      if(snake[0].x==snake[i].x && snake[0].y==snake[i].y)
        return true;
    }
    //collision with the food
    if(snake[0].x==food.x && snake[0].y==food.y)
    {
      get=true;
      putfood();
      points+=10;
      move(maxheight-1,0);
      printw("%d",points);
      if((points%100)==0)
        del-=10000;
    }else
      get=false;
    return false;
  }


  void movesnake()
  {
    //detect key
    int tmp=getch();
    switch(tmp)
    {
      case KEY_LEFT:
        if(direction!='r')
          direction='l';
        break;
      case KEY_UP:
        if(direction!='d')
          direction='u';
        break;
      case KEY_DOWN:
        if(direction!='u')
          direction='d';
        break;
      case KEY_RIGHT:
        if(direction!='l')
          direction='r';
        break;
      case KEY_BACKSPACE:
        direction='q';
        break;
    }
    //if there wasn't a collision with food
    if(!get)
    {
      move(snake[snake.size()-1].y,snake[snake.size()-1].x);
      printw(" ");
      refresh();
      snake.pop_back();
    }
    if(direction=='l')
    {
      snake.insert(snake.begin(),snakepart(snake[0].x-1,snake[0].y));
    }else if(direction=='r'){
      snake.insert(snake.begin(),snakepart(snake[0].x+1,snake[0].y));

    }else if(direction=='u'){
      snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y-1));
    }else if(direction=='d'){
      snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y+1));
    }
      move(snake[0].y,snake[0].x);
      addch(partchar);
    refresh();
  }


  
public:
  snakeclass()
  {
    initscr();
    nodelay(stdscr,true);     //if there wasn't any key pressed don't wait for keypress
    keypad(stdscr,true);      //init the keyboard
    noecho();                 //don't write
    curs_set(0);              //cursor invisible
    getmaxyx(stdscr,maxheight,maxwidth);
    partchar='x';
    oldalchar=(char)219;
    etel='*';
    food.x=0;
    food.y=0;
    for(int i=0;i<5;i++)
      snake.push_back(snakepart(40+i,10));
    points=0;
    del=110000;
    get=0;
    direction='l';
    srand(time(NULL));
    putfood();
    //make the game-board -- up-vertical
    for(int i=0;i<maxwidth-1;i++)
    {
      move(0,i);
      addch(oldalchar);
    }
    //left-horizontal
    for(int i=0;i<maxheight-1;i++)
    {
      move(i,0);
      addch(oldalchar);
    }
    //down-vertical
    for(int i=0;i<maxwidth-1;i++)
    {
      move(maxheight-2,i);
      addch(oldalchar);
    }
    //right-horizontal
    for(int i=0;i<maxheight-1;i++)
    {
      move(i,maxwidth-2);
      addch(oldalchar);
    }
    //draw the snake
    for(int i=0;i<snake.size();i++)
    {
      move(snake[i].y,snake[i].x);
      addch(partchar);
    }
    move(maxheight-1,0);
    printw("%d",points);
    move(food.y,food.x);
    addch(etel);
    refresh();
  }

  ~snakeclass()
  {
    nodelay(stdscr,false);      //turn back
    getch();                    //wait until a key is pressed
    endwin();
  }

  void start()
  {
    while(1)
    {
      if(collision())
      {
        move(12,36);
        printw("game_over");
        break;
      }
      movesnake();
      if(direction=='q')        //exit
        break;
      usleep(del);      //Linux delay
    }
  }
};

/***************************** ESTRUCTURA DEL SNAKE GAME *****************************/




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
char getchh(void)
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
    c = getchh();

    if(c == 'w')
    {
      buffer[0] = '1';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 's')
    {
      buffer[0] = '2';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 'a')
    {
      buffer[0] = '3';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 'd')
    {
      buffer[0] = '4';
      buffer[1] = '\0';
      n = write(SocketFD,buffer,1);
    }
    if(c == 'x')
    {
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
