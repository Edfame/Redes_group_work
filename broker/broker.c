#include "../system_config.h"
#include "../sensor/sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

/*
DELETE comes from a file
*/
#define SENSOR_PORT 79790
#define CLIENT_PORT 1337
#define ADMIN_PORT 42069

struct sockaddr_in set_connection_info(int port) {

    struct sockaddr_in servaddr;

    /*
        Copying the necessary info from "server" to "servaddr".
    */
    //Assign IP and port to the socket.
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    return servaddr;
}

void bind_connection(int sockfd, struct sockaddr_in address) {

    if(bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        printf("Socket not bound.\n");
        exit(EXIT_FAILURE);

    } else {
        printf("Socket bound.\n");

    }
}

void set_option(int sockfd, int option) {

    //Make sure that socket doesnt reserve the port.
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))) {

        perror("setsockopt failed.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[]) {

    struct sockaddr_in sensors,
                        clients,
                        admins;

    int socket_sensors,
        socket_sensors_client,
        socket_clients,
        socket_clients_client,
        socket_admins,
        socket_admins_client,

        option,
        fds_max;

    option = 1;
    fds_max = 0;

    fd_set master,
            read_fds;

    //Creating a new socket for incoming connections.
    socket_sensors = new_socket();
    socket_clients = new_socket();
    socket_admins = new_socket();

    //Teeling se OS to do not reserv that port.
    set_option(socket_sensors, option);
    set_option(socket_clients, option);
    set_option(socket_admins, option);

    sensors = set_connection_info(SENSOR_PORT);
    clients = set_connection_info(CLIENT_PORT);
    admins = set_connection_info(ADMIN_PORT);

    bind_connection(socket_sensors, sensors);
    bind_connection(socket_clients, clients);
    bind_connection(socket_admins, admins);



    return 0;
}
