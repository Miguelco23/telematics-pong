#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h> //Biblioteca de Unix
//#include <arpa/inet.h> //Biblioteca de Unix
#include <Winsock2.h> //Biblioteca de Windows
#include <WS2tcpip.h> //Biblioteca de Windows
#include <pthread.h>
#include <math.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define MOVE "MOVE"
#define STATE "STATE"
#define POINT "POINT"
#define OPPOSITE_POINT "OPPOSITE_POINT"
#define GAME_START "GAME_START"
#define GAME_WINNER "GAME_WINNER"
#define AD_WINNER "AD_WINNER"

typedef struct {
    int socket;
    char name[50];
    int idUnico;
    int roomId;
    // Agrega aquí cualquier otra información que necesites para mantener el estado del jugador
} Client;

Client clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void send_to_all_room(char *message, int sender_socket) { //Al cliente que lo mando y al cliente opuesto
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].roomId == clients[sender_socket].roomId) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void send_to_room(char *message, int sender_socket) { //solamente al cliente opuesto
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].socket != sender_socket && clients[i].roomId == clients[sender_socket].roomId) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void assign_room_id(Client *client) {
    client->roomId = (num_clients + 1) / 2;
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);
    char buffer[1024];
    int bytes_received;
    char client_name[50];
    int client_id;

    pthread_mutex_lock(&mutex);
    if (num_clients < MAX_CLIENTS) {
        client_id = num_clients;  // Se asigna una identificación única basada en el número actual de clientes
        clients[num_clients].socket = client_socket;
        clients[num_clients].idUnico = client_id;
        assign_room_id(&clients[num_clients]);  // Asignar un identificador de pareja
        num_clients++;
    }
    pthread_mutex_unlock(&mutex);

    // Se recibe el mensaje de conexion y se asigna el nombre del cliente
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';
    char *remote_command = strtok(buffer, " ");
    char *remote_data = strtok(NULL, " ");
    if (remote_data != NULL) {
        strcpy(client_name, remote_data);
        strcpy(clients[client_id].name, client_name);
    } else {
        snprintf(client_name, sizeof(client_name), "Anonymous-%d", client_id); // Si no se recibe un nombre se asigna uno predeterminado
        strcpy(clients[client_id].name, client_name);
    }

    sprintf(buffer, "CONNECTED %s %d", client_name, client_id);
    send(client_socket, buffer, strlen(buffer), 0);
    //send_to_all(buffer, client_socket);

    int is_connected = 1;
    while (is_connected) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            printf("Client disconnected...\n");
            is_connected = 0;
            break;
        }

        buffer[bytes_received] = '\0';
        char *remote_command = strtok(buffer, " ");
        char *remote_data = strtok(NULL, " ");
        printf("Command received from client: %s\n", buffer);

        if (strcmp(remote_command, DISCONNECT) == 0) {
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++) { // Elimina al cliente de la lista
                if (clients[i].socket == client_socket) {
                    for (int j = i; j < num_clients - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            
            sprintf(buffer, "DISCONNECTED %s", client_name);
            send_to_room(buffer, client_socket);
            is_connected = 0;

        } else if (strcmp(remote_command, MOVE) == 0) {
            if (strcmp(remote_data,"UP") == 0) {
                sprintf(buffer, "OPPOSITE_MOVE UP");
                send_to_room(buffer, client_socket);
            } else if (strcmp(remote_data,"DOWN") == 0) {
                sprintf(buffer, "OPPOSITE_MOVE DOWN");
                send_to_room(buffer, client_socket);
            }
    
        } else if (strcmp(remote_command, POINT) == 0) {
            sprintf(buffer, "OPPOSITE_POINT");
            send_to_room(buffer, client_socket);
            
        } else if (strcmp(remote_command, AD_WINNER) == 0) {
            sprintf(buffer, "GAME_WINNER %s", remote_data);
            send_to_all_room(buffer, client_socket);
        } else {
            continue;
        }
    }
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket del servidor a la dirección y el puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(1);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_socket);
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Aceptar una conexión entrante
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");

        // Crear un hilo para manejar al cliente
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *)client_socket_ptr);
        pthread_detach(thread); // Liberar recursos automáticamente después de la finalización del hilo
    }    

    close(server_socket);

    return 0;
}