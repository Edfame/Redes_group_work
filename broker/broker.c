#include "../system_config.h"

#define BROKER_SETTINGS "broker.csv"

#define SENSOR_PORT 3
#define CLIENT_PORT 4
#define ADMIN_PORT 5

#define MAX_CLIENTS 200
#define INFO_SIZE 16

void new_register(identifier *fd, char *info) {

    fd->client_info = malloc(sizeof(info));
    strcpy(fd->client_info, info);
    printf("new client: %s", fd->client_info);

}

void disconnected(int sockfd, fd_set *master, identifier *fd) {

    printf("Socket %d disconnected.\n", sockfd);
    close(sockfd);
    FD_CLR(sockfd, master);
    free(fd);

}

void get_sensor_last_read(char *id, int fds_max, identifier **fds, char *return_buffer) {

    for (int i = 0; i < fds_max; i++) {

        if(fds[i]->type == FD_S && strcmp(fds[i]->client_info, id) == 0) {

            strcpy(return_buffer, queue_get_tail(fds[i]->last_reads);
            return;
        }
    }
}

char *list_all_sensors(int fds_max, identifier **fds) {

    char to_return[BUFFER_SIZE];

    clearArray(to_return, BUFFER_SIZE);

    for (int i = 0; i < fds_max; i++) {

        if(fds[i]->type == FD_S) {

            snprintf(to_return, sizeof(to_return), "%s;%s;", fds[i]->client_info, to_return);
        }
    }

    return to_return;
}

void read_admin(int sockfd, char *buffer, int fds_max, identifier **fds) {

    /*TODO
     * according to the message received on buffer, decide which make.
     */

    short operation_index = 0,
          id_index = 1;

    char id[INFO_SIZE],
         return_buffer[INFO_SIZE],
         operation = buffer[operation_index];

    switch (operation) {

        case '0':

            get_info(buffer, id, id_index, DELIM);
            get_sensor_last_read(id, fds_max, fds, return_buffer);

            send(sockfd, return_buffer, sizeof(return_buffer), 0);
            break;

        case '1':

            strcpy(return_buffer, list_all_sensors(fds_max, fds));

            send(sockfd, return_buffer, sizeof(return_buffer), 0);
            break;

        case '2':

            break;

        case '3':
            break;
        case '4':
            break;

        default:
            break;
    }
}
/*
 * read_sensor - if the connection was not closed  by the client, reads the info sent.
 *               registers it or adds more info.
 */
void read_sensor(char *buffer, identifier *fd) {

    if (fd->last_reads == NULL) {
        new_register(fd, buffer);
        fd->last_reads = new_queue();

    } else {
        printf("read: %s", buffer);
        queue_insert(fd->last_reads, buffer);
    }
}

int max(int first_number, int second_number) {

    return (first_number > second_number ? first_number : second_number);
}

int max3(int first_number, int second_number, int third_number) {

    return max(first_number, max(second_number, third_number));
}

void fds_realloc(int *fds_max, int socket_client, identifier **fds, fd_type type) {

    *fds_max = max(*fds_max, socket_client);
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

    char broker_settings[BUFFER_SIZE],
         sensor_port[INFO_SIZE],
         client_port[INFO_SIZE],
         admin_port[INFO_SIZE],
         buffer[BUFFER_SIZE];

    clearArray(broker_settings, BUFFER_SIZE);
    read_file_content(BROKER_SETTINGS, broker_settings);

    get_info(broker_settings, sensor_port, SENSOR_PORT, DELIM);
    get_info(broker_settings, client_port, CLIENT_PORT, DELIM);
    get_info(broker_settings, admin_port, ADMIN_PORT, DELIM);

    option = 1;
    fds_max = 0;

    fd_set master,
            read_fds;

    identifier **fds,
                *fd;

    //Creating a new socket for incoming connections.
    socket_sensors_server = new_socket();
    socket_clients_server = new_socket();
    socket_admins_server = new_socket();

    //Telling se OS to do not reserve that port.
    set_option(socket_sensors_server, option);
    set_option(socket_clients_server, option);
    set_option(socket_admins_server, option);

    sensors = set_connection_info(atoi(sensor_port));
    clients = set_connection_info(atoi(client_port));
    admins = set_connection_info(atoi(admin_port));

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

                    fd = fds[i];

                    if(recv(i, buffer, sizeof(buffer), 0) <= 0) {

                        disconnected(i, &master, fd);

                    } else {

                        switch(fd->type) {

                            case FD_S:

                                read_sensor(buffer, fd);
                                break;

                            case FD_C:

                                printf("client msg.\n");
                                break;

                            case FD_A:

                                read_admin(i, buffer, fds_max, fds);
                                printf("admin msg.\n");
                                break;

                            default:
                                break;
                        }

                    }
                }
            }
        }
    }


    return 0;
}
