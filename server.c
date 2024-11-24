// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "database.h"
#include "protocol.h"
#include "utils.h"

#define PORT 8080
#define BUFFER_SIZE 4096

void *client_handler(void *socket_desc);

int main(int argc, char *argv[]) {
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server, client;
    int port;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

    // Initialize database connection
    if (!init_database()) {
        fprintf(stderr, "Failed to connect to the database.\n");
        exit(1);
    }

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        exit(1);
    }
    puts("Socket created");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    puts("Bind done");

    // Listen
    listen(server_fd, 5);
    puts("Waiting for incoming connections...");

    while ((client_sock = accept(server_fd, NULL, NULL))) {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            exit(1);
        }

        puts("Handler assigned");
    }

    if (client_sock < 0) {
        perror("Accept failed");
        exit(1);
    }

    close(server_fd);
    close_database();

    return 0;
}

void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    int read_size;
    char client_message[BUFFER_SIZE];

    // Receive messages from client
    while ((read_size = recv(sock, client_message, BUFFER_SIZE - 1, 0)) > 0) {
        client_message[read_size] = '\0';

        // Print the message received from the client
        printf("\n=== Message Received from Client ===\n%s\n", client_message);

        // Process client message
        char response[BUFFER_SIZE];
        if (process_client_message(client_message, response)) {
            // Send response back to client
            send(sock, response, strlen(response), 0);
        } else {
            // Send error code 5000
            strcpy(response, "5000\r\n");
            send(sock, response, strlen(response), 0);
        }

        // Optionally, print the response sent to the client
        printf("\n=== Message Sent to Client ===\n%s\n", response);

        memset(client_message, 0, BUFFER_SIZE);
        memset(response, 0, BUFFER_SIZE);
    }


    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("Recv failed");
    }

    free(socket_desc);
    close(sock);
    pthread_exit(NULL);
}

