#include "../system_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
DELETE comes from a file
*/
#define SENSOR_PORT 79790
#define CLIENT_PORT 13375
#define ADMIN_PORT 42069
#define MAX_CLIENTS 200

void read_sensor(int sockfd, fd_set *master, identifier **fds) {

    sensor *temp_sensor = new_sensor(-1, "", "", -1.0);

    //Checks if the the socket was closed by the client.
    if(recv(sockfd, temp_sensor, sizeof(struct sensor), 0) <= 0) {

        printf("Socket %d disconected.\n", sockfd);
        close(sockfd);
        FD_CLR(sockfd, master);


    //In case it was not closed, there is info to work with.
    } else if(temp_sensor->id != -1) {

        printf("I received: %d, %s, %s, %f, %d\n",
                            temp_sensor->id,
                            temp_sensor->type,
                            temp_sensor->local,
                            temp_sensor->firmware_version,
                            temp_sensor->read_value);
    }

    free(temp_sensor);
}

int max(int first_number, int second_number) {

    return (first_number > second_number ? first_number : second_number);
}

int max3(int first_number, int second_number, int third_number) {

    return max(first_number, max(second_number, third_number));
}

void fds_realloc(int *fds_max, int socket_client, identifier **fds, fd_type type) {

    *fds_max = max(*fds_max, socket_client);
    //fds = realloc(fds, (int)(*fds_max) + 1);
    fds[socket_client] = new_identifier(type);
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

    printf(">Info set for: %d.\n", port);

    return servaddr;
}

void bind_connection(int sockfd, struct sockaddr_in address) {

    if(bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {

        printf(">Socket not bound: %d.\n", address.sin_port);
        exit(EXIT_FAILURE);

    } else {

        printf(">Socket bound: %d.\n", address.sin_port);

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

int accept_connection(int sockfd, struct sockaddr_in adress) {

    int socket_return,
        address_len = sizeof(adress);


    socket_return = accept(sockfd, (struct sockaddr *) &adress, (socklen_t *)&address_len);

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

    int //Sensors sockets.
        socket_sensors_server,
        socket_sensors_client,

        //Clients sockets.
        socket_clients_server,
        socket_clients_client,

        //Admins sockets.
        socket_admins_server,
        socket_admins_client,

        option,
        fds_max;

    option = 1;
    fds_max = 0;

    fd_set master,
            read_fds;

    identifier **fds;

    //Creating a new socket for incoming connections.
    socket_sensors_server = new_socket();
    socket_clients_server = new_socket();
    socket_admins_server = new_socket();

    //Teeling se OS to do not reserve that port.
    set_option(socket_sensors_server, option);
    set_option(socket_clients_server, option);
    set_option(socket_admins_server, option);

    sensors = set_connection_info(SENSOR_PORT);
    clients = set_connection_info(CLIENT_PORT);
    admins = set_connection_info(ADMIN_PORT);

    //Binding the socket_servers to the info.
    bind_connection(socket_sensors_server, sensors);
    bind_connection(socket_clients_server, clients);
    bind_connection(socket_admins_server, admins);

    //Telling socket to start listening to their ports.
    listen_to(socket_sensors_server);
    listen_to(socket_clients_server);
    listen_to(socket_admins_server);

    //Cleaning the master. It's a clean machine (The Beatles - Penny Lane)
    FD_ZERO(&master);

    //Adding which fd to the master.
    FD_SET(socket_sensors_server, &master);
    FD_SET(socket_clients_server, &master);
    FD_SET(socket_admins_server, &master);

    //Setting the highest fd number to fds_max.
    fds_max = max3(socket_sensors_server, socket_clients_server, socket_admins_server);
    fds = malloc(MAX_CLIENTS);

    fds[socket_sensors_server] = new_identifier(FD_S);
    fds[socket_clients_server] = new_identifier(FD_C);
    fds[socket_admins_server] = new_identifier(FD_A);

    //Woopa loop!
    for(;;) {

        read_fds = master;

        if(select(fds_max + 1, &read_fds, NULL, NULL, NULL) == -1) {

            perror("Select failed.\n");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i <= fds_max; i++) {

            if(FD_ISSET(i, &read_fds)) {

                /*
                    Handeling new sensors connecting.
                */
                if(i == socket_sensors_server){

                    socket_sensors_client = accept_connection(socket_sensors_server, sensors);

                    FD_SET(socket_sensors_client, &master);
                    printf("New sensor registred.\n");

                    fds_realloc(&fds_max, socket_sensors_client, fds, FD_S);
                /*
                    Handeling new clients connecting
                */
                } else if(i == socket_clients_server) {

                    socket_clients_client = accept_connection(socket_clients_server, clients);

                    //socket_clients_client = accept_connection(socket_clients_server, clients);

                    FD_SET(socket_clients_client, &master);
                    printf("New client registred.\n");

                    fds_realloc(&fds_max, socket_clients_client, fds, FD_C);

                /*
                    Handeling new admins connecting
                */
                } else if(i == socket_admins_server) {

                    socket_admins_client = accept_connection(socket_admins_server, admins);

                    FD_SET(socket_admins_client, &master);
                    printf("New admin registred.\n");

                    fds_realloc(&fds_max, socket_admins_client, fds, FD_A);

                /*
                    Handeling data from any type of clients.
                */
                } else {

                    switch(fds[i]->type) {

                        case FD_S:

                            read_sensor(i, &master, fds);
                            break;

                        case FD_C:

                            printf("client msg.\n");
                            break;

                        case FD_A:

                            printf("admin msg.\n");
                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }


    return 0;
}
