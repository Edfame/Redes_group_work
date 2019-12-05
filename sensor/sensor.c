#include "../system_config.h"
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
#define READ_INTERVAL 1

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

    send(sockfd, sensor1, sizeof(struct sensor), 0);
    printf("REGISTER MSG SENT.\n");

    /*
    Every X seconds, that comes from YAML file (read_interval), sends a sensor read.
    For now, every read is a radom value.
    */

    for(;;) {

        sleep(READ_INTERVAL);
        sensor1->read_value = rand() % 500;
        send(sockfd, sensor1, sizeof(struct sensor), 0);

        printf("READ SENT: %d\n", sensor1->read_value);

    }

    close(sockfd);
    free(sensor1);

    return 0;
}
