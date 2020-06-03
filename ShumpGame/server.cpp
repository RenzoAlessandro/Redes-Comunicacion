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
#include <sstream>
#include "funciones.h"

#define TAM_MSG 1000
#define SCORE_WIN 100

using namespace std;

string cierre = "adios";

vector<vector<int> > manage_users;
vector<pair<int, string> > list_users;
vector<pair<int, int> > initial_positions;

int generated_gifts = 0;

void print_manage_users()
{
  cout<<"Manage Users"<<endl;
  for (int i=0; i<manage_users.size(); i++)
  {
    for (int j=0; j<manage_users[i].size(); j++)
    {
      cout<<manage_users[i][j]<<"\t";
    }
    cout<<endl;
  }
}

void generate_gift()
{
  for (int i=0; i<manage_users.size(); i++)
  {
    string broadcast_msg = "";
    broadcast_msg += "6";
    broadcast_msg += int_to_char_spaces(rand() % 50 + 1, 2);
    broadcast_msg += int_to_char_spaces(rand() % 5 + 10, 2);

    cout<<"Gift Broadcast: "<<broadcast_msg<<endl;

    write(manage_users[i][1], broadcast_msg.c_str(), broadcast_msg.size());
  }

  generated_gifts += 1;
}

int verify_winner()
{
  int index_winner = -1;
  for (int i=0; i<manage_users.size(); i++)
  {
    if (manage_users[i][4] == SCORE_WIN)
    {
      index_winner = i;
      return index_winner;
    }
  }

  return index_winner;
}

void fill_initial_positions()
{
  initial_positions.push_back(make_pair(10,10));
  initial_positions.push_back(make_pair(10,15));
  initial_positions.push_back(make_pair(10,20));
  initial_positions.push_back(make_pair(10,25));
  initial_positions.push_back(make_pair(10,30));
}

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
  stSockAddr.sin_port = htons(45550);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if(::bind(SocketFD1,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD1);
    exit(EXIT_FAILURE);
  }

  if(-1 == listen(SocketFD1, 10))
  {
    perror("error listen failed");
    close(SocketFD1);
    exit(EXIT_FAILURE);
  }
  return SocketFD2;
}

void cli(int &ConnectFD,int &SocketFD1,int &_socket)
{
  _socket = crearSocket(SocketFD1);
}

int n;

void broadcast_position(int actual_id, int actual_socket_id, int actual_pos_x, int actual_pos_y)
{
  for (int i=0; i<manage_users.size(); i++)
  {
    if (manage_users[i][1] != actual_socket_id)
    {
      string broadcast_msg = "";
      broadcast_msg += "3";
      broadcast_msg += to_string(actual_id);
      broadcast_msg += int_to_char_spaces(actual_pos_x, 2);
      broadcast_msg += int_to_char_spaces(actual_pos_y, 2);

      cout<<"Broadcast: "<<broadcast_msg<<endl;

      write(manage_users[i][1], broadcast_msg.c_str(), broadcast_msg.size());
    }
  }

}

void broadcast_new_player(int actual_id, int actual_socket_id, int actual_pos_x, int actual_pos_y)
{
  for (int i=0; i<manage_users.size(); i++)
  {
    if (manage_users[i][1] != actual_socket_id)
    {
      string broadcast_msg = "";
      broadcast_msg += "4";
      broadcast_msg += to_string(actual_id);
      broadcast_msg += int_to_char_spaces(actual_pos_x, 2);
      broadcast_msg += int_to_char_spaces(actual_pos_y, 2);

      cout<<"Notify New Player: "<<broadcast_msg<<endl;

      write(manage_users[i][1], broadcast_msg.c_str(), broadcast_msg.size());
    }
  }
}

//<<<<<<< HEAD
//=======
/*
void broadcast_position(int actual_id, int actual_socket_id, int actual_pos_x, int actual_pos_y)
{
for (int i=0; i<manage_users.size(); i++)
{
if (manage_users[i][1] != actual_socket_id)
{
string broadcast_msg = "";
broadcast_msg += "3";
broadcast_msg += to_string(actual_id);
broadcast_msg += int_to_char_spaces(actual_pos_x, 2);
broadcast_msg += int_to_char_spaces(actual_pos_y, 2);

cout<<"Broadcast: "<<broadcast_msg<<endl;

write(manage_users[i][1], broadcast_msg.c_str(), broadcast_msg.size());
}
}

}
*/

void reading(int ConnectFD, char buffer[])
{
  string received_message;
  int count_received_messages = 0;
  do
  {
    n = read(ConnectFD, buffer, 1);
    buffer[n] = '\0';

    //Received new position
    if (buffer[0] == '1')
    {
      n = read(ConnectFD, buffer, 1);
      buffer[n] = '\0';


    //Received new position
    if (buffer[0] == '1')
    {
      n = read(ConnectFD, buffer, 1);
      buffer[n] = '\0';

      int rcv_id = atoi(buffer);
      cout<<"ID: "<<rcv_id<<endl;

      n = read(ConnectFD, buffer, 1);
      buffer[n] = '\0';

      int rcv_key = atoi(buffer);

      //Up
      if (rcv_key == 1)
      {
        manage_users[rcv_id][3] -= 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Down
      else if (rcv_key == 2)
      {
        manage_users[rcv_id][3] += 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Left
      else if (rcv_key == 3)
      {
        manage_users[rcv_id][2] -= 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Right
      else if (rcv_key == 4)
      {
        manage_users[rcv_id][2] += 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }
    }

    //received_message = buffer;

    //cout<<"Received: "<<received_message<<endl;


      int rcv_id = atoi(buffer);
      cout<<"ID: "<<rcv_id<<endl;

      n = read(ConnectFD, buffer, 1);
      buffer[n] = '\0';

      int rcv_key = atoi(buffer);

      //Up
      if (rcv_key == 1)
      {
        manage_users[rcv_id][3] -= 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Down
      else if (rcv_key == 2)
      {
        manage_users[rcv_id][3] += 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Left
      else if (rcv_key == 3)
      {
        manage_users[rcv_id][2] -= 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }

      //Right
      else if (rcv_key == 4)
      {
        manage_users[rcv_id][2] += 1;

        string answer_movement = "";
        answer_movement += "1";
        answer_movement += int_to_char_spaces(manage_users[rcv_id][2], 2);
        answer_movement += int_to_char_spaces(manage_users[rcv_id][3], 2);

        cout<<"Answer: "<<answer_movement<<endl;

        broadcast_position(rcv_id, ConnectFD, manage_users[rcv_id][2], manage_users[rcv_id][3]);

        n = write(ConnectFD, answer_movement.c_str(), answer_movement.size());
      }
      print_manage_users();
      count_received_messages += 1;
    }

    if (((count_received_messages % 5) == 0) and (count_received_messages != 0))
    {
      generate_gift();
    }

    //received_message = buffer;

    //cout<<"Received: "<<received_message<<endl;

  } while(int(received_message.find(cierre)) < 0);

}

int main()
{
  int socketFD1;
  int ConnectFD;
  int _socket;

  cli(ConnectFD,socketFD1,_socket);

  char buffer[TAM_MSG];

  int id=0;

  int count = 0;

  fill_initial_positions();

  for(;;)
  {
    int ConnectClient = accept(socketFD1, NULL, NULL);
    bzero(buffer,TAM_MSG);

    //Adding new player in server.
    n = read(ConnectClient,buffer,1);

    if ( buffer[0] == '0' )
    {
      n = read(ConnectClient,buffer,2);
      buffer[n] = '\0';

      int size_nickname = atoi(buffer);

      n = read(ConnectClient, buffer, size_nickname);
      buffer[n] = '\0';

      string nickname_rcv(buffer);

      cout<<"Nickname: "<<nickname_rcv<<endl;

      list_users.push_back(make_pair(id, nickname_rcv));

      vector<int> new_user;
      new_user.push_back(id);                           //ID
      new_user.push_back(ConnectClient);                //SocketID
      new_user.push_back(initial_positions[count % initial_positions.size()].first);             //Start position X
      new_user.push_back(initial_positions[count % initial_positions.size()].second);            //Start position Y
      new_user.push_back(0);                            //Initial Score
      new_user.push_back(1);                            //Bool: 1 (Alive) 0 (Dead)

      manage_users.push_back(new_user);

      print_manage_users();

      count += 1;

      //Answer from server to new player added.
      string answer = "";

      answer += "0";
      answer += to_string(id);
      answer += to_string(count);

      for (int i=0; i<count; i++)
      {
        answer += int_to_char_spaces(manage_users[i][2], 2);
        answer += int_to_char_spaces(manage_users[i][3], 2);
      }

      id += 1;

      cout<<"Answer: "<<answer<<endl;

      n = write(ConnectClient, answer.c_str(), answer.size());

      if (count > 1)
        broadcast_new_player(id-1, ConnectClient, manage_users[id-2][2], manage_users[id-2][3]);

      thread(reading, ConnectClient, buffer).detach();
    }

    else
    {
      string error = "Escriba su nickname para entrar en el juego.";
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
