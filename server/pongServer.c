#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    char name[50];
    // Agrega aquí cualquier otra información que necesites para mantener el estado del jugador
} Client;

Client clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void send_to_all(char *message, int sender_socket) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void handle_client(int client_socket) {
    char buffer[1024];
    int bytes_received;
    char client_name[50];
    strcpy(client_name, "Anonymous");

    pthread_mutex_lock(&mutex);
    if (num_clients < MAX_CLIENTS) {
        clients[num_clients].socket = client_socket;
        num_clients++;
    }
    pthread_mutex_unlock(&mutex);

    sprintf(buffer, "CONNECTED %s", client_name);
    send(client_socket, buffer, strlen(buffer), 0);
    send_to_all(buffer, client_socket);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }

        printf("Received: %s", buffer);
        send_to_all(buffer, client_socket);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].socket == client_socket) {
            // Elimina al cliente de la lista
            for (int j = i; j < num_clients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    sprintf(buffer, "DISCONNECTED %s", client_name);
    send_to_all(buffer, client_socket);

    close(client_socket);
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
        pthread_t thread;
        pthread_create(&thread, NULL, (void *)handle_client, (void *)&client_socket);
    }

    close(server_socket);

    return 0;
}