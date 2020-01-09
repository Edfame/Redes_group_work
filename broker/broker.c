#include "../system_config.h"

#define BROKER_SETTINGS "broker.csv"

#define SENSOR_PORT 3
#define CLIENT_PORT 4
#define ADMIN_PORT 5

#define MAX_CLIENTS 200
#define INFO_SIZE 16

/*
 * new_register - if there is new client connected, registers it with its info.
 */
void new_register(identifier *fd, char *info) {

    fd->client_info = malloc(sizeof(info));
    strcpy(fd->client_info, info);
    printf(">NEW: %s\n", fd->client_info);

}

/*
 * disconnected - if a socket was closed.
 */
void disconnected(int sockfd, fd_set *master, identifier *fd) {

    printf("Socket %d disconnected.\n", sockfd);
    close(sockfd);
    FD_CLR(sockfd, master);
    free(fd);

}

/*
 * disconnect - set a socket to be disconnected.
 */
void disconnect(char *id, int fds_max, identifier **fds, char *return_buffer) {

    char temp_id[INFO_SIZE];
    clear_array(temp_id, INFO_SIZE);

    for (int i = 3; i <= fds_max; i++) {

        if((fds[i]->type == FD_S) && (fds[i]->client_info != NULL)) {

            get_info(fds[i]->client_info, temp_id, 0, DELIM);

            if(strcmp(id, temp_id) == 0) {

                fds[i]->type = NONE;

                snprintf(return_buffer, BUFFER_SIZE, "Set to disconnect sensor %s at socket %d", temp_id, i);
                return;
            }
        }
    }
    strcpy(return_buffer, ADMIN_SENSOR_NOT_FOUND);
}

/*
 * get_sensor_last_read - gets the last last read from the sensor whose ID is id.
 */
void get_sensor_last_read(char *id, int fds_max, identifier **fds, char *return_buffer) {

    char temp_id[strlen(id)];
    clear_array(temp_id, strlen(id));

    for (int i = 3; i <= fds_max; i++) {

        if((fds[i]->client_info != NULL) && (fds[i]->type == FD_S)) {

            get_info(fds[i]->client_info, temp_id, 0, DELIM);

            if(strcmp(id, temp_id) == 0) {

                strcpy(return_buffer, queue_get_tail(fds[i]->last_reads));
                return;
            }
        }
    }
    strcpy(return_buffer, ADMIN_SENSOR_NOT_FOUND);
}

/*
 * list_all_sensors - returns a list of all the sensors in the system.
 */
void list_all_sensors(int fds_max, identifier **fds, char *return_buffer) {

    char temp_return_buffer[BUFFER_SIZE];
    clear_array(temp_return_buffer, BUFFER_SIZE);

    short sensors_counter = 0;

    for (int i = 3; i <= fds_max; i++) {

        if((fds[i]->type == FD_S) && (fds[i]->client_info != NULL)) {

            sensors_counter++;

            strcat(temp_return_buffer, fds[i]->client_info);
            strcat(temp_return_buffer, ADMIN_DELIM);

        }
    }

    snprintf(return_buffer, sizeof(temp_return_buffer) + sizeof(sensors_counter), "%d;%s", sensors_counter, temp_return_buffer);
}

/*
 * read_admin - reads a message sent by an admin and decides what to do accordingly.
 */
void read_admin(char *buffer, char *return_buffer, int fds_max, identifier *fd, identifier **fds) {

    if (fd->client_info == NULL) {

        new_register(fd, buffer);

    } else {

        short operation_index = 0,
              id_index = 1;

        char id[INFO_SIZE],
             operation = buffer[operation_index];

        clear_array(id, INFO_SIZE);

        switch (operation) {

            case '0':

                get_info(buffer, id, id_index, DELIM);
                get_sensor_last_read(id, fds_max, fds, return_buffer);
                break;

            case '1':

                list_all_sensors(fds_max, fds, return_buffer);
                break;

            case '2':

                break;

            case '3':

                get_info(buffer, id, id_index, DELIM);
                disconnect(id, fds_max, fds, return_buffer);
                break;

            default:
                break;
        }
    }
}
/*
 * read_sensor - if the connection was not closed  by the client, reads the info sent.
 *               registers it or adds more info.
 */
void read_sensor(char *buffer, char *return_buffer, identifier *fd) {

    if (fd->client_info == NULL) {

        new_register(fd, buffer);
        fd->last_reads = new_queue();

    } else {

        char id[INFO_SIZE];

        get_info(fd->client_info, id, 0, DELIM);
        printf("ID: %s\tREAD_SENSOR: %s\n", id, buffer);
        queue_insert(fd->last_reads, buffer);
    }

    char *received = "RECEIVED.";
    strcpy(return_buffer, received);
}

void fds_realloc(int *fds_max, int socket_client, identifier **fds, fd_type type) {

    *fds_max = max(*fds_max, socket_client);
    fds[socket_client] = new_identifier(type);
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
         buffer[BUFFER_SIZE],
         return_buffer[BUFFER_SIZE];

    clear_array(broker_settings, BUFFER_SIZE);
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

    sensors = set_connection_info(NULL, atoi(sensor_port));
    clients = set_connection_info(NULL, atoi(client_port));
    admins = set_connection_info(NULL, atoi(admin_port));

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

                if(i == socket_sensors_server){

                    socket_sensors_client = accept_connection(socket_sensors_server, sensors);

                    FD_SET(socket_sensors_client, &master);
                    printf("New sensor registered.\n");

                    fds_realloc(&fds_max, socket_sensors_client, fds, FD_S);

                } else if(i == socket_clients_server) {

                    socket_clients_client = accept_connection(socket_clients_server, clients);

                    FD_SET(socket_clients_client, &master);
                    printf("New client registered.\n");

                    fds_realloc(&fds_max, socket_clients_client, fds, FD_C);

                } else if(i == socket_admins_server) {

                    socket_admins_client = accept_connection(socket_admins_server, admins);

                    FD_SET(socket_admins_client, &master);
                    printf("New admin registered.\n");

                    fds_realloc(&fds_max, socket_admins_client, fds, FD_A);

                } else {

                    fd = fds[i];

                    if((fd->type == NONE) || (recv(i, buffer, sizeof(buffer), 0) <= 0)) {

                        disconnected(i, &master, fd);

                    } else {

                        clear_array(return_buffer, sizeof(return_buffer));

                        switch(fd->type) {

                            case FD_S:

                                read_sensor(buffer, return_buffer, fd);
                                break;

                            case FD_C:

                                printf("client msg.\n");
                                break;

                            case FD_A:

                                read_admin(buffer, return_buffer, fds_max, fd, fds);
                                printf("enviar: %s\n", return_buffer);
                                break;

                            default:
                                break;
                        }

                        send(i, return_buffer, sizeof(return_buffer), 0);
                    }
                }
            }
        }
    }


    return 0;
}
