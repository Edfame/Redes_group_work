#include "sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../yaml/cyaml.h"

#define SENSOR_SETTINGS "sensor.yaml"

//TODO Delete this ones.
#define HOSTNAME "127.0.0.1"
#define PORT 79790
#define READ_INTERVAL 10

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
sensor_message *new_sensor_message(char message_type, sensor *sensor) {

    sensor_message *new_message = malloc(sizeof(struct sensor_message));

    new_message->message_type = message_type;
    new_message->message_content = -1;
    new_message->sensor = sensor;

    return new_message;
}

sensor *new_sensor(short id, char type[], char local[], float firmware_version) {

    sensor *new_sensor = malloc(sizeof(struct sensor));

    new_sensor->id = id;
    strcpy(new_sensor->type, type);
    strcpy(new_sensor->local, local);
    new_sensor->firmware_version = firmware_version;

    return new_sensor;
}

int new_socket() {

    int new_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(new_socket < 0) {

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
    if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {

        perror(">Connection to the server failed.\nAborted.\n");
        exit(2);

    //if the connection to the server succeeds.
    } else {

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(servaddr.sin_addr), addr, INET_ADDRSTRLEN);
        printf(">Connection established with: %s.\n", addr);
    }
}

int main(int argc, char const *argv[]) {

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = new_socket();
    //HOSTNAME and PORT must come from yaml file.
    servaddr = set_connection_info(HOSTNAME, PORT);

    create_connection(sockfd, servaddr);

    //Send regestry message with (ID, TYPE, LOCAL, FIRMWARE_V)
    //(ID, TYPE, LOCAL, FIRMWARE_V) must come from YAML file.
    sensor *sensor1 = new_sensor(1, "CO2", "Evora", 1.0);
    sensor_message *new_message= new_sensor_message('r', sensor1);

    send(sockfd, new_message, sizeof(struct sensor_message), 0);
    printf("REGISTER MSG SENT.\n");

    /*
    Every X seconds, that comes from YAML file (read_interval), sends a sensor read.
    For now, every read is a radom value.
    */

    new_message->message_type = 'v';

    for(;;) {

        sleep(READ_INTERVAL);
        new_message->message_content = rand() % 100;
        send(sockfd, new_message, sizeof(struct sensor_message), 0);

        printf("READ SENT.\n");

    }
    return 0;
}
