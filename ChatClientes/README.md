# MultiChat (Client - Client)

- Implementacion de un chat entre clientes.
- El servidor mantiene una lista de clientes con su nickname y socket descriptor
    - Utiliza un pair, una variable global.
    - vector<pair<string,int>> list_users;
- El cliente envia:
    - El "msg", solicitara el nickname a quien comunicarse posterior el mensaje a enviar.
    - Si solo se envía el "nickname", solicitara el nickname del nuevo cliente.
    - Si es “lista”, el servidor enviara la lista de nicknames al cliente.
- El Thread en el servidor:
    - Busca el nickname en el pair y extraer el socket descriptor. Si no existe debe de enviar ‘Nickname no existe.’.
- El cliente tiene implementado 02 threads:
   - Uno para leer del socket e imprimir en el terminal por el momento la lectura es una especia de pulling o busywait.
   - Otro para leer los mensajes del terminal y enviarlo  a través del socket.

# Comandos en el Programa:
- nickname : Para registrase.
- list     : Listar todos los usuarios.
- msg      : Enviar un mensaje.
- exit     : Para salir.

# Comandos para Compilar:

  - Server:  g++ -std=c++11 -o server Server.cpp -lpthread
  - Client1:  g++ -std=c++11 -o client1 Client.cpp -lpthread
  - Client2:  g++ -std=c++11 -o client2 Client.cpp -lpthread
