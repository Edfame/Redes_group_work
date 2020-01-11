#include "../system_config.h"

void print_operations() {

    printf("0 - Lists locals where are sensors of type T.\n1 - Get last read from a local.\n2 - Get a read for a certain date and hour.\n3 - Subscribe to local X.\n");

}

void list_all(char *buffer) {

    char to_print[BUFFER_SIZE],
         info[INFO_SIZE];

    short info_counter = 0;

    bzero(to_print, sizeof(to_print));
    bzero(info, sizeof(info));

    get_info(buffer, info, OPERATION_INDEX, CLIENT_DELIM);

    if((info_counter = atoi(info)) == 0) {

        printf("> \t- %s\n", CLIENT_LOCAL_NOT_FOUND);
        return;
    }

    for (int i = 1; i <= info_counter; i++) {

        get_info(buffer, to_print, i, ADMIN_DELIM);
        printf("\t- %s\n", to_print);
    }
}

int main(int argc, char const *argv[]) {

    int sockfd,
        fds_max,
        valid_operation;

    struct sockaddr_in servaddr;

    fd_set master,
           read_fds;

    char operation,
         subscribed_local[BUFFER_SIZE],
         address[INFO_SIZE],
         port[INFO_SIZE],
         client_settings_file[INFO_SIZE],
         client_settings[BUFFER_SIZE],
         client_info_file[INFO_SIZE],
         client_info[BUFFER_SIZE],
         id[INFO_SIZE],
         nickname[INFO_SIZE],
         buffer[BUFFER_SIZE],
         info[BUFFER_SIZE],
         input[INFO_SIZE];

    //Get Client files.
    if (argc < 3) {
        printf("\nUSAGE ERROR.\nUsage: ./client CLIENT_SETTINGS.csv CLIENT_ID.csv\n\n");
        exit(EXIT_FAILURE);
    }

    strcpy(client_settings_file, (char*) argv[1]);
    strcpy(client_info_file, (char*) argv[2]);

    //Reads connection settings to the array.
    bzero(client_settings, BUFFER_SIZE);
    read_file_content(client_settings_file, client_settings);

    //Sorting the info.
    get_info(client_settings, address, CLIENT_ADDRESS, DELIM);
    get_info(client_settings, port, CLIENT_PORT_CLIENT, DELIM);

    //Creating a new socket for the Admin.
    sockfd = new_socket();

    //Setting connection info.
    servaddr = set_connection_info(address, atoi(port));

    //Establishing the connection.
    create_connection(sockfd, servaddr);

    //Get Admin info from file.
    bzero(client_info, BUFFER_SIZE);
    read_file_content(client_info_file, client_info);

    get_info(client_info, id, CLIENT_ID, DELIM);
    get_info(client_info, nickname, CLIENT_NICKNAME, DELIM);

    //Creates the register message.
    snprintf(buffer, sizeof(buffer), "%s,%s", id, nickname);

    //Sends the register message.
    send(sockfd, buffer, sizeof(buffer), 0);

    //Receives an empty message.
    recv(sockfd, buffer, sizeof(buffer), 0);

    FD_ZERO(&master);

    FD_SET(sockfd, &master);
    FD_SET(STDIN_FILENO, &master);

    fds_max = max(sockfd, STDIN_FILENO);

    bzero(subscribed_local, sizeof(subscribed_local));

    strcpy(subscribed_local, CLIENT_NOT_SUBSCRIBED);

    print_operations();

    for(;;) {

        read_fds = master;

        if(select(fds_max + 1, &read_fds, NULL, NULL, NULL) == -1) {

            perror("Select failed.\n");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i <= fds_max; i++) {

            if (FD_ISSET(i, &read_fds)) {

                if(i == STDIN_FILENO) {

                    valid_operation = 1;

                    scanf(" %c", &operation);

                    switch (operation) {

                        case '0':

                        case '1':

                            scanf(" %s", input);
                            snprintf(buffer, sizeof(buffer), "%c,%s", operation, input);
                            break;

                        case '2':

                            valid_operation = 0;
                            break;

                        case '3':

                            scanf(" %s", input);

                            if(strcmp(subscribed_local, input) == 0) {

                                printf("> Updated subscription.\n");

                            } else if(strcmp(subscribed_local, CLIENT_NOT_SUBSCRIBED) != 0) {

                                printf("> Already subscribed to a local.\n");
                                valid_operation = 0;
                                break;
                            }

                            snprintf(buffer, sizeof(buffer), "%c,%s", operation, input);
                            strcpy(subscribed_local, input);
                            break;

                        default:

                            valid_operation = 0;
                            break;

                    }

                    if(valid_operation) {

                        send(sockfd, buffer, sizeof(buffer), 0);

                    } else {

                        printf("> Invalid Operation.\n");
                        print_operations();

                    }

                } else if(i == sockfd) {

                    if(read(sockfd, buffer, sizeof(buffer)) > 0) {

                        operation = buffer[OPERATION_INDEX];

                        bzero(info, sizeof(info));
                        get_info(buffer, info, INFO_INDEX, CLIENT_DELIM);

                        switch (operation) {

                            case '0':

                                printf("> Locals:\n");
                                list_all(info);
                                break;

                            case '1':

                                printf("> Last read from local:\n");
                                list_all(info);
                                break;

                            case '2':
                                break;

                            case '3':

                                printf("> Subscription info: %s\n", info);
                                break;

                            default:
                                break;
                        }
                    }
                }
            }
            bzero(buffer, sizeof(buffer));
        }

    }

    close(sockfd);
    return 0;
}