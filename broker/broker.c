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

int max(int first_number, int second_number) {

    return ((first_number > second_number ? first_number : second_number));
}

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

void listen_to(int sockfd) {

    //Setting the server to listen the client.
    if(listen(sockfd, 3) < 0) {

        printf("Listen failed.\n");
        exit(EXIT_FAILURE);
    }
}

void set_option(int sockfd, int option) {

    //Make sure that socket doesnt reserve the port.
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))) {

        perror("setsockopt failed.");
        exit(EXIT_FAILURE);
    }
}

int accept_connection(int sockfd, struct sockaddr adress) {

    int socket_return;

    socket_return = accept(sockfd, (struct sockaddr *) &adress, (socklen_t *) sizeof(adress));

    if(socket_return < 0) {

        perror("Accept failed.\nAborted.\n");
        exit(EXIT_FAILURE);

    }

    return socket_return;
}
int main(int argc, char const *argv[]) {

    struct sockaddr_in sensors,
                        clients,
                        admins;

    int socket_sensors_server,
        socket_sensors_client,
        socket_clients_server,
        socket_clients_client,
        socket_admins_server,
        socket_admins_client,

        option,
        fds_max;

    option = 1;
    fds_max = 0;

    fd_set master,
            read_fds;

    //Creating a new socket for incoming connections.
    socket_sensors_server = new_socket();
    socket_clients_server = new_socket();
    socket_admins_server = new_socket();

    //Teeling se OS to do not reserv that port.
    set_option(socket_sensors_server, option);
    set_option(socket_clients_server, option);
    set_option(socket_admins_server, option);

    sensors = set_connection_info(SENSOR_PORT);
    clients = set_connection_info(CLIENT_PORT);
    admins = set_connection_info(ADMIN_PORT);

    bind_connection(socket_sensors_server, sensors);
    bind_connection(socket_clients_server, clients);
    bind_connection(socket_admins_server, admins);

    listen(socket_sensors_server);
    listen(socket_clients_server);
    listen(socket_admins_server);

    FD_ZERO(&master);
    FD_SET(socket_sensors_server, &master);
    FD_SET(socket_clients_server, &master);
    FD_SET(socket_admins_server, &master);

    fds_max = max(socket_sensors_server, max(socket_clients_server, socket_admins_server));

    for(;;) {

        read_fds = master;

        if(select(fds_max + 1, &read_fds, NULL, NULL, NULL) < 0) {

            perror("Select failed.");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < fds_max; i++) {

            if(FD_ISSET(i, &master)) {

                /*
                    Handeling new sensors connecting.
                */
                if(i == socket_sensors_server){

                    socket_sensors_client = accept_connection(socket_sensors_server, sensors);

                    FD_SET(socket_sensors_client, &master);
                    printf("New sensor registred.\n");

                    fds_max = (fds_max < socket_sensors_client ? socket_sensors_client : fds_max);

                /*
                    Handeling new clients connecting
                */.
                } else if(i == socket_clients_server) {

                    socket_clients_client = accept_connection(socket_clients_server, clients);

                    FD_SET(socket_clients_client, &master);
                    printf("New client registred.\n");

                    fds_max = (fds_max < socket_clients_client ? socket_clients_client : fds_max);

                /*
                    Handeling new admins connecting
                */.
                } else if(i == socket_admins_server) {

                    socket_admins_client = accept_connection(socket_admins_server, admins);

                    FD_SET(socket_admins_client, &master);
                    printf("New client registred.\n");

                    fds_max = (fds_max < socket_admins_client ? socket_admins_client : fds_max);

                /*
                    Handeling data from any type of clients.
                */
                } else {

                    //Trying to receive data from the client.
                    if(recv(i, ...))
                }
            }
        }
    }


    return 0;
}
