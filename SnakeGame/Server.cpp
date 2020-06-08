// Copyright 2020 Renzo Sucari Velasquez
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT                7070
#define MAX_PLAYERS         1024
#define HEIGHT              24
#define WIDTH               80
#define MAX_SNAKE_LENGTH    HEIGHT * WIDTH
#define WINNER_LENGTH       15
#define FRUIT               -1024
#define BORDER              -99
#define WINNER              -94
#define UP_KEY              'W'
#define DOWN_KEY            'S'
#define LEFT_KEY            'A'
#define RIGHT_KEY           'D'

//Mapa del juego
int             game_map[HEIGHT][WIDTH];
int             map_size = HEIGHT * WIDTH * sizeof(game_map[0][0]);
pthread_mutex_t map_lock = PTHREAD_MUTEX_INITIALIZER;   
int             someone_won = 0;

//Tipos de teclas de dirección
typedef enum{
    UP    = UP_KEY, 
    DOWN  = DOWN_KEY, 
    LEFT  = LEFT_KEY, 
    RIGHT = RIGHT_KEY
} direction;

//Los bloques de construcción para las serpientes
typedef struct{
    int x, y;
    direction d;
} coordinate;

//Estructura de serpiente, cada parte está compuesta de coordenadas
typedef struct{
    int player_no, length;
    coordinate head;
    coordinate body_segment[MAX_SNAKE_LENGTH - 2];
    coordinate tail;
} snake;

//Función para crear una serpiente.
snake* crear_snake(int player_no, int head_y, int head_x){
    
    //Coloque la serpiente en el mapa (matriz)
    pthread_mutex_lock(&map_lock);
    game_map[head_y][head_x]   = -player_no;
    game_map[head_y+1][head_x] = 
    game_map[head_y+2][head_x] = player_no;
    pthread_mutex_unlock(&map_lock);    
    
    //Crear estructura de serpiente, iniciamos hacia ARRIBA
    snake* s = new snake;
    
    s->player_no = player_no;
    s->length = 3;

    s->head.y = head_y;
    s->head.x = head_x;
    s->head.d = UP;

    s->body_segment[0].y = head_y + 1;
    s->body_segment[0].x = head_x;
    s->body_segment[0].d = UP;

    s->tail.y = head_y + 2;
    s->tail.x = head_x;
    s->tail.d = UP;

    return s;
}

//Función para matar serpientes y liberar memoria
void muerte_snake(snake* s){

    //Establecer todas las coordenadas del Snake a cero en el mapa
    pthread_mutex_lock(&map_lock);
    game_map[s->head.y][s->head.x] = 
    game_map[s->tail.y][s->tail.x] = 0;    
    int i;
    for(i = 0; i < s->length - 2; i++)
        game_map[s->body_segment[i].y][s->body_segment[i].x] = 0;
    pthread_mutex_unlock(&map_lock);

    //Liberamos memoria
    free(s);    
    s = NULL;
}

//Funcion para mover el Snake
void mover_snake(snake* s, direction d){
    memmove(&(s->body_segment[1]), 
            &(s->body_segment[0]), 
            (s->length-2) * sizeof(coordinate));
    s->body_segment[0].y = s->head.y;
    s->body_segment[0].x = s->head.x; 
    s->body_segment[0].d = s->head.d;
    switch(d){
        case UP:{
            s->head.y = s->head.y-1;
            s->head.d = UP;            
            break;
        }
        case DOWN:{
            s->head.y = s->head.y+1;
            s->head.d = DOWN;  
            break;
        }
        case LEFT:{
            s->head.x = s->head.x-1;
            s->head.d = LEFT;  
            break;
        }
        case RIGHT:{
            s->head.x = s->head.x+1;
            s->head.d = RIGHT;  
            break;
        }
        default: break;
    }
    pthread_mutex_lock(&map_lock);
    game_map[s->head.y][s->head.x] = -(s->player_no);
    game_map[s->body_segment[0].y][s->body_segment[0].x] = s->player_no;
    game_map[s->tail.y][s->tail.x] = 0;
    pthread_mutex_unlock(&map_lock);

    s->tail.y = s->body_segment[s->length-2].y;
    s->tail.x = s->body_segment[s->length-2].x;
}

//Funcion para colocar las frutas en random
void agregar_fruta(){
    int x, y;
    do{
        y = rand() % (HEIGHT - 6) + 3;
        x = rand() % (WIDTH - 6) + 3;
    } while (game_map[y][x] != 0);
    pthread_mutex_lock(&map_lock);
    game_map[y][x] = FRUIT;
    pthread_mutex_unlock(&map_lock);
}

//Funcion para comer una fruta
void comer_fruta(snake* s, direction d){
    memmove(&(s->body_segment[1]), 
            &(s->body_segment[0]), 
            (s->length-2) * sizeof(coordinate));
    s->body_segment[0].y = s->head.y;
    s->body_segment[0].x = s->head.x; 
    s->body_segment[0].d = s->head.d;
    switch(d){
        case UP:{
            s->head.y = s->head.y-1;
            s->head.d = UP; 
            if(game_map[s->head.y][s->head.x + 1] == FRUIT){
                pthread_mutex_lock(&map_lock);
                game_map[s->head.y][s->head.x + 1] = 0;   
                pthread_mutex_unlock(&map_lock);        
            }
            break;
        }
        case DOWN:{
            s->head.y = s->head.y+1;
            s->head.d = DOWN; 
            if(game_map[s->head.y][s->head.x + 1] == FRUIT){
                pthread_mutex_lock(&map_lock);
                game_map[s->head.y][s->head.x + 1] = 0; 
                pthread_mutex_unlock(&map_lock);
            }
            break;
        }
        case LEFT:{
            s->head.x = s->head.x-1;
            s->head.d = LEFT;  
            break;
        }
        case RIGHT:{
            s->head.x = s->head.x+1;
            s->head.d = RIGHT;  
            break;
        }
        default: break;
    }
    pthread_mutex_lock(&map_lock);
    game_map[s->head.y][s->head.x] = -(s->player_no);
    game_map[s->body_segment[0].y][s->body_segment[0].x] = s->player_no;
    pthread_mutex_unlock(&map_lock);
    s->length++;
    agregar_fruta();
}

//Crear thread separado - Stevens, capitulo 12, pagina 428
int make_thread(void* (*fn)(void *), void* arg){
    int             err;
    pthread_t       tid;
    pthread_attr_t  attr;

    err = pthread_attr_init(&attr);
    if(err != 0)
        return err;
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(err == 0)
        err = pthread_create(&tid, &attr, fn, arg);
    pthread_attr_destroy(&attr);
    return err;
}

//Mensaje de error y salida
void error(const char* msg){
    perror(msg);
    fflush(stdout);
    exit(1);
}

//Manejar la señal Ctrl + C
void ctrl_c_handler(){
    printf("\nEl servidor ha caido.\n");
    exit(0);
}

//Función del juego + threads
void* main_juego_snake(void* arg){  

    //Determinar el número de jugador a partir del argumento del file descriptor
    int fd = *(int*) arg;
    int player_no = fd-3;
    printf("Ingreso el jugador %d!\n", player_no);

    //Encuentra tres ceros consecutivos en un mapa para comenzar la posición del Snake
    int head_y, head_x;
    srand(time(NULL));
    do{
        head_y = rand() % (HEIGHT - 6) + 3;
        head_x = rand() % (WIDTH - 6) + 3;
    } while (!(
        ((game_map[head_y][head_x] == game_map[head_y+1][head_x]) 
            == game_map[head_y+2][head_x]) == 0));

    //Creamos la estructura del Snake
    snake* player_snake = crear_snake(player_no, head_y, head_x);

    //Variables de entrada del usuario
    char key = UP;
    char key_buffer;
    char map_buffer[map_size];
    int  bytes_sent, n;
    int  success = 1;

    while(success){

        //Comprobamos si alguien ganó
        if(someone_won)
            success = 0;

        //Comprobamos si eres el ganador
        if(player_snake->length >= 15){
            someone_won = player_no;
            pthread_mutex_lock(&map_lock);
            game_map[0][0] = WINNER;
            pthread_mutex_unlock(&map_lock);
        } else if(game_map[0][0]!= BORDER){
            pthread_mutex_lock(&map_lock);
            game_map[0][0] = someone_won;
            pthread_mutex_unlock(&map_lock);
        }

        //Copiamos el mapa en el bufer y enviamos al cliente
        memcpy(map_buffer, game_map, map_size);
        bytes_sent = 0;
        while(bytes_sent < map_size){         
            bytes_sent += write(fd, game_map, map_size);
            if (bytes_sent < 0) error("ERROR writing to socket");
        } 

        //Leemos la entra del jugador (movimiento)
        bzero(&key_buffer, 1);
        n = read(fd, &key_buffer, 1);
        if (n <= 0)
            break;

        //Verificamos la direccion del usuario
        key_buffer = toupper(key_buffer);   
        if(  ((key_buffer == UP)    && !(player_snake->head.d == DOWN))
           ||((key_buffer == DOWN)  && !(player_snake->head.d == UP))
           ||((key_buffer == LEFT)  && !(player_snake->head.d == RIGHT)) 
           ||((key_buffer == RIGHT) && !(player_snake->head.d == LEFT)))
            key = key_buffer;

        switch(key){

            case UP:{
                if((game_map[player_snake->head.y-1][player_snake->head.x] == 0) && 
                    !(game_map[player_snake->head.y-1][player_snake->head.x+1] == FRUIT)){
                    mover_snake(player_snake, UP);
                    printf("Jugador %d SUBE (W)\n",player_no);
                }
                else if((game_map[player_snake->head.y-1][player_snake->head.x] == FRUIT) || 
                    (game_map[player_snake->head.y-1][player_snake->head.x+1] == FRUIT)){
                    comer_fruta(player_snake, UP);
                    printf("Jugador %d come un Fruta.\n",player_no);
                }
                else{
                    mover_snake(player_snake, LEFT);
                    success = 0;
                }
                break;
            }

            case DOWN:{
                if((game_map[player_snake->head.y+1][player_snake->head.x] == 0)&& 
                    !(game_map[player_snake->head.y+1][player_snake->head.x+1] == FRUIT)){
                    mover_snake(player_snake, DOWN);
                    printf("Jugador %d BAJA (S).\n",player_no);
                }
                else if((game_map[player_snake->head.y+1][player_snake->head.x] == FRUIT) || 
                    (game_map[player_snake->head.y+1][player_snake->head.x+1] == FRUIT)){
                    comer_fruta(player_snake, DOWN);
                    printf("Jugador %d come un Fruta.\n",player_no);
                }
                else{
                    mover_snake(player_snake, DOWN);
                    success = 0;
                }
                break;
            }

            case LEFT:{
                if(game_map[player_snake->head.y][player_snake->head.x-1] == 0){
                    mover_snake(player_snake, LEFT);
                    printf("Jugador %d IZQUIERDA (A).\n",player_no);
                }
                else if(game_map[player_snake->head.y][player_snake->head.x-1] == FRUIT){
                    comer_fruta(player_snake, LEFT);
                    printf("Jugador %d come un Fruta.\n",player_no);
                }
                else{
                    mover_snake(player_snake, LEFT);
                    success = 0;
                }
                break;
            }

            case RIGHT:{
                if(game_map[player_snake->head.y][player_snake->head.x+1] == 0){
                    mover_snake(player_snake, RIGHT);
                    printf("Jugador %d DERECHA (D).\n",player_no);
                }
                else if(game_map[player_snake->head.y][player_snake->head.x+1] == FRUIT){
                    comer_fruta(player_snake, RIGHT);
                    printf("Jugador %d come un Fruta.\n",player_no);
                }
                else{
                    mover_snake(player_snake, RIGHT);
                    success = 0;
                }
                break;
            }

            default: break;
        }   
    }

    if(player_snake->length == WINNER_LENGTH){
        fprintf(stderr, "Jugador %d gano!\n", player_no);
        muerte_snake(player_snake);
        close(fd);  
        return 0;
    }

    else{
        fprintf(stderr, "Jugador %d dejo el juego.\n", player_no);
        muerte_snake(player_snake);
        close(fd);  
        return 0;
    }
}

int main(){
     int                socket_fds[MAX_PLAYERS];     
     struct sockaddr_in socket_addr[MAX_PLAYERS];
     int                i;

     //Handle Ctrl+C
     //signal(SIGINT, ctrl_c_handler);

     //Rellenamos la matriz del juego con ceros
     memset(game_map, 0, map_size);
    
     //Establecemos los bordes del juego
     for(i = 0; i < HEIGHT; i++)
        game_map[i][0] = game_map[i][WIDTH-2] = BORDER;     
     for(i = 0; i < WIDTH; i++)
        game_map[0][i] = game_map[HEIGHT-1][i] = BORDER;

    //Agregamos tres frutas random
    srand(time(NULL));
    for(i = 0; i < 3; i++)
        agregar_fruta();

     //Creamos el Socket del Servidor
     socket_fds[0] = socket(AF_INET, SOCK_STREAM, 0);
     if (socket_fds[0] < 0) 
        error("ERROR opening socket");
        
     //Establecemos la direccion del socket a cero y establecemos atributos
     bzero((char *) &socket_addr[0], sizeof(socket_addr[0]));  
     socket_addr[0].sin_family = AF_INET;
     socket_addr[0].sin_addr.s_addr = INADDR_ANY;
     socket_addr[0].sin_port = htons(PORT);
     
     if (bind(socket_fds[0], (struct sockaddr *) &socket_addr[0], sizeof(socket_addr[0])) < 0) 
              error("ERROR on binding");

    //Ponemos el socket en modo LISTEN para aceptar solicitudes de conexión entrantes
    listen(socket_fds[0], 5);
    socklen_t clilen = sizeof(socket_addr[0]);

     for(i = 1;; i++){
        
         //Aceptamos la solicitud de conexión entrante
         socket_fds[i] = accept(socket_fds[0], (struct sockaddr *) &socket_addr[i], &clilen);
         if (socket_fds[i] < 0) 
              error("ERROR on accept");

         //Reiniciamos el juego si alguien ganó
         if(someone_won){
            printf("El juego ha sido reiniciado!\n");
            someone_won = 0;
        }

         make_thread(&main_juego_snake, &socket_fds[i]); 
     }
     
     //Cerramos el Socket del servidor
     close(socket_fds[0]);  
     return 0; 
}
