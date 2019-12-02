#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SENSOR_SETTINGS "sensor.yaml"
#define MESSAGE_SIZE 64

//TODO Delete this ones.
#define HOSTNAME "127.0.0.1"
#define PORT 240231

typedef struct message {

    int client_type;
    char message_type;
    char message_content[MESSAGE_SIZE];

} message;

/*
    new_message:
        Allocates space in memory for a new message.
        Copies the inrfomation passed through the args to that space.

        args:
            - message_type: represents the type of message that will be sent.
            - message_content: is the message that is going to be sent.

        return:
            Returns the address address of the new message.
*/
message *new_message(char message_type, char message_content[MESSAGE_SIZE]) {

    message *new_message = malloc(sizeof(struct message));

    new_message->client_type = 2;
    new_message->message_type = message_type;
    strcpy(new_message->message_content, message_content);

    return new_message;
}

int new_socket() {

    int new_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(new_message < 0) {

        perror(">Socket creation failed.\nAborted.\n");
        exit(1);

    } else {

        printf(">Socket creation succeeded.\n");

    }

    return new_socket;
}

struct sockaddr_in set_connection_info(char *hostname, int port) {

    struct sockaddr_in servaddr;
    struct hostent *server;

    server = gethostbyname(hostname);

    //Checking if the hostname provided was a valid one.
    if(server == NULL) {
        fprintf(stderr, "ERROR, no such host.\n");
        exit(1);
    }

    /*
        Copying the necessary info from "server" to "servaddr".
    */
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    bcopy((char *) server->h_addr, (char *) &servaddr.sin_addr.s_addr, server->h_length);

    return servaddr;
}

void create_connection(int sockfd, struct sockaddr_in servaddr) {

    //if the connection to the server doesn't succeed.
    if(connect(sockfd, (struct sockaddr) &servaddr, sizeof(servaddr)) < 0) {

        perror(">Connection to the server failed.\nAborted.\n");
        exit(2);

    //if the connection to the server succeeds.
    } else {

        char addr[INET_AADRSTRLEN];
        inet_ntop(AF_INET, &(servaddr.sin_addr), addr, INET_ADDRSTRLEN);
        printf(">Connection established with: %s.\n", addr);
    }
}

int main(int argc, char const *argv[]) {

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = new_socket();
    servaddr = set_connection_info(HOSTNAME, PORT);

    create_connection(sockfd, servaddr);


    return 0;
}
