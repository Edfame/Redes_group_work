#include "../system_config.h"

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

identifier *find_id(char *id, int fds_max, identifier **fds) {

    char temp_id[INFO_SIZE];
    bzero(temp_id, INFO_SIZE);

    for (int i = 3; i <= fds_max; i++) {

        if((fds[i]->type == FD_S) && (fds[i]->client_info != NULL)) {

            get_info(fds[i]->client_info, temp_id, 0, DELIM);

            if(strcmp(id, temp_id) == 0) {
                return fds[i];
            }
        }
    }
    return NULL;
}

/*
 * disconnect - set a socket to be disconnected.
 */
void disconnect(char *id, int fds_max, identifier **fds, char *return_buffer) {

    identifier *fd = find_id(id, fds_max, fds);

    if(fd != NULL) {

        fd->type = NONE;
        snprintf(return_buffer, BUFFER_SIZE, "Set sensor %s to disconnect.", id);

    } else {

        strcpy(return_buffer, ADMIN_SENSOR_NOT_FOUND);
    }
}

/*
 * get_sensor_last_read - gets the last last read from the sensor whose ID is id.
 */
void get_sensor_last_read(char *id, int fds_max, identifier **fds, char *return_buffer) {

    identifier *fd = find_id(id, fds_max, fds);

    if(fd != NULL) {

        strcpy(return_buffer, queue_get_tail(fd->last_reads));

    } else {

        strcpy(return_buffer, ADMIN_SENSOR_NOT_FOUND);
    }
}

/*
 * list_all_sensors - returns a list of all the sensors in the system.
 */
void list_all_sensors(int fds_max, identifier **fds, char *return_buffer) {

    char temp_return_buffer[BUFFER_SIZE];
    bzero(temp_return_buffer, BUFFER_SIZE);

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

void list_all_locals(char *type, int fds_max, identifier **fds, char *return_buffer) {

    char temp_return_buffer[BUFFER_SIZE],
         temp_local[INFO_SIZE],
         temp_type[INFO_SIZE];

    bzero(temp_return_buffer, BUFFER_SIZE);
    bzero(temp_local, INFO_SIZE);
    bzero(temp_type, INFO_SIZE);

    short locals_counter = 0;

    for (int i = 3; i <= fds_max; i++) {

        if((fds[i]->type == FD_S) && (fds[i]->client_info != NULL)) {

            get_info(fds[i]->client_info, temp_type, CLIENT_SENSOR_TYPE, DELIM);

            if (strcmp(temp_type, type) == 0) {

                get_info(fds[i]->client_info, temp_local, CLIENT_SENSOR_LOCAL, DELIM);

                if(strstr(temp_return_buffer, temp_local) == NULL) {

                    locals_counter++;

                    strcat(temp_return_buffer, temp_local);
                    strcat(temp_return_buffer, CLIENT_DELIM);
                }
            }
        }
    }

    snprintf(return_buffer, sizeof(temp_return_buffer) + sizeof(locals_counter), "%d;%s", locals_counter, temp_return_buffer);
}

void subscribe_local(int sockfd, char *local, int fds_max, identifier **fds, char *return_buffer) {

    char temp_local[INFO_SIZE],
         sub_flag = 'n';
    bzero(temp_local, INFO_SIZE);

    for (int i = 3; i <= fds_max ; i++) {

        if((fds[i]->type == FD_S) && (fds[i]->client_info != NULL)) {

            get_info(fds[i]->client_info, temp_local, CLIENT_SENSOR_LOCAL, DELIM);

            if(strcmp(temp_local, local) == 0) {

                fds[i]->subscribed_sensors[sockfd] = CLIENT_SUBSCRIBED;
                sub_flag = 'y';
            }
        }
    }

    if(sub_flag == 'y') {

        strcpy(return_buffer, CLIENT_SUBSCRIBED_MSG);

    } else {

        strcpy(return_buffer, "Not subscribed.");
    }
}
/*
 * read_client - reads a message sent by a client and decides what to do accordingly.
 */
void read_client(int sockfd, char *buffer, char *return_buffer, int fds_max, identifier *fd, identifier **fds) {

    if (fd->client_info == NULL) {

        new_register(fd, buffer);

    } else {

        char temp_return_buffer[BUFFER_SIZE],
             info[INFO_SIZE],
             operation = buffer[OPERATION_INDEX];

        bzero(temp_return_buffer, BUFFER_SIZE);
        bzero(info, INFO_SIZE);

        switch (operation) {

            case '0':

                get_info(buffer, info, INFO_INDEX, DELIM);
                list_all_locals(info, fds_max, fds, temp_return_buffer);
                break;

            case '1':

                break;

            case '2':

                get_info(buffer, info, INFO_INDEX, DELIM);
                subscribe_local(sockfd, info, fds_max, fds, temp_return_buffer);
                break;

            default:
                break;
        }

        snprintf(return_buffer, sizeof(temp_return_buffer) + 2, "%c|%s", operation, temp_return_buffer);
    }
}
/*
 * read_admin - reads a message sent by an admin and decides what to do accordingly.
 */
void read_admin(char *buffer, char *return_buffer, int fds_max, identifier *fd, identifier **fds) {

    if (fd->client_info == NULL) {

        new_register(fd, buffer);

    } else {

        char id[INFO_SIZE],
             operation = buffer[OPERATION_INDEX];

        bzero(id, INFO_SIZE);

        switch (operation) {

            case '0':

                get_info(buffer, id, INFO_SIZE, DELIM);
                get_sensor_last_read(id, fds_max, fds, return_buffer);
                break;

            case '1':

                list_all_sensors(fds_max, fds, return_buffer);
                break;

            case '2':

                break;

            case '3':

                get_info(buffer, id, INFO_INDEX, DELIM);
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
        fd->subscribed_sensors = calloc(MAX_CLIENTS, sizeof(short));

    } else {

        char id[INFO_SIZE],
             to_send[BUFFER_SIZE];

        bzero(to_send, sizeof(to_send));

        get_info(fd->client_info, id, BROKER_SENSOR_ID, DELIM);
        printf("ID: %s\tREAD_SENSOR: %s\n", id, buffer);
        queue_insert(fd->last_reads, buffer);

        for (int i = 3; i < MAX_CLIENTS; i++) {

            if(fd->subscribed_sensors[i] == CLIENT_SUBSCRIBED) {

                snprintf(to_send, sizeof(to_send), "2|%s", buffer);
                send(i, to_send, sizeof(to_send), 0);

                printf("MANDEI %s\n", to_send);
            }
        }
    }

    strcpy(return_buffer, "RECEIVED.");
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

    char broker_settings_file[INFO_SIZE],
         broker_settings[BUFFER_SIZE],
         sensor_port[INFO_SIZE],
         client_port[INFO_SIZE],
         admin_port[INFO_SIZE],
         buffer[BUFFER_SIZE],
         return_buffer[BUFFER_SIZE];

    if (argc < 2) {
        printf("\nUSAGE ERROR.\nUsage: ./broker BROKER_SETTINGS.csv\n\n");
        exit(EXIT_FAILURE);
    }

    strcpy(broker_settings_file, (char*) argv[1]);

    bzero(broker_settings, BUFFER_SIZE);
    read_file_content(broker_settings_file, broker_settings);

    get_info(broker_settings, sensor_port, BROKER_SENSOR_PORT, DELIM);
    get_info(broker_settings, client_port, BROKER_CLIENT_PORT, DELIM);
    get_info(broker_settings, admin_port, BROKER_ADMIN_PORT, DELIM);

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

                        bzero(return_buffer, sizeof(return_buffer));

                        switch(fd->type) {

                            case FD_S:

                                read_sensor(buffer, return_buffer, fd);
                                break;

                            case FD_C:

                                read_client(i, buffer, return_buffer, fds_max, fd, fds);
                                break;

                            case FD_A:

                                read_admin(buffer, return_buffer, fds_max, fd, fds);
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
