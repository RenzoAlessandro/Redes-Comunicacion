# TicTacToe (Client - Client)

- Implementacion del clasico TicTacToe.

# Comandos en el Programa:
- Agregar un movimiento: [Simbolo][posicion X][posicion Y].
- Ejemplo: x21

# Comandos para Compilar:

  - Server:  g++ -std=c++11 -o server Server.cpp -lpthread
  - Client1:  g++ -std=c++11 -o client1 Client.cpp -lpthread
  - Client2:  g++ -std=c++11 -o client2 Client.cpp -lpthread

# Comandos para Ejecutar:

  - ./server 45502
  - ./client1 127.0.0.1 45502
  - ./client2 127.0.0.1 45502