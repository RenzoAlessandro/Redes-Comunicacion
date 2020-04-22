# Calculadora

- Se le envía 2 valores y el tipo de operación el servidor debe devolver la respuesta indicando el tipo de operación-
- El servidor debe implementar threads
  - El Cliente ingresa su operación: 562 326  +
  - El servidor responde: La suma es 888

# Comandos para Compilar:

  - Server:  g++ -std=c++11 -o server Server.cpp -lpthread
  - CLient:  g++ -std=c++11 -o client Client.cpp -lpthread
