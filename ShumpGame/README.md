# Shump Game (Client - Client)

- Implementacion del Juego Shump.

# Comandos en el Programa:
- Movimientos: w (arriba), 
               s (abajo), 
               a (izquierda), 
               d (derecha).

# Comandos para Compilar:
  - Instalar Ncurses: sudo apt-get install libncurses5-dev libncursesw5-dev
  - Server:   g++ -std=c++11 -o server server.cpp -lpthread
  - Client1:  g++ -std=c++11 -o client1 client.cpp -lpthread
  - Client2:  g++ -std=c++11 -o client2 client.cpp -lpthread

# Comandos para Ejecutar:
  - ./server.exe 
  - ./cliente1.exe 
  - ./cliente2.exe 