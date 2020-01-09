#include "../system_config.h"

void print_operations() {

    printf("0 - Lists locals where are sensors of type T.\n1 - Get last read from a local; Get a read for a certain date and hour.\n2 - Subscribe to sensor X.\n");

}

void list_all_locals(char *buffer) {

    char to_print[BUFFER_SIZE],
         locals[INFO_SIZE];

    short locals_counter = 0;

    bzero(to_print, sizeof(to_print));
    bzero(locals, sizeof(locals));

    get_info(buffer, locals, 0, CLIENT_DELIM);

    printf("> Locals:\n");

    if((locals_counter = atoi(locals)) == 0) {

        printf("> \t- %s\n", CLIENT_LOCAL_NOT_FOUND);
        return;
    }

    for (int i = 1; i <= locals_counter; i++) {

        get_info(buffer, to_print, i, CLIENT_DELIM);
        printf("\t- %s\n", to_print);
    }
}

int main(int argc, char const *argv[]) {

    int sockfd,
        valid_operation = 1;

    struct sockaddr_in servaddr;

    char operation,
         address[INFO_SIZE],
         port[INFO_SIZE],
         client_settings_file[INFO_SIZE],
         client_settings[BUFFER_SIZE],
         client_info_file[INFO_SIZE],
         client_info[BUFFER_SIZE],
         id[INFO_SIZE],
         nickname[INFO_SIZE],
         buffer[BUFFER_SIZE],
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

    print_operations();

    while (scanf(" %c", &operation) != EOF) {

        valid_operation = 1;

        switch (operation) {

            case '0':

            case '1':

            case '2':

                scanf(" %s", input);
                snprintf(buffer, sizeof(buffer), "%c,%s", operation, input);
                break;

            default:

                valid_operation = 0;

                printf("> Invalid Operation.\n");
                print_operations();
                break;
        }

        send(sockfd, buffer, sizeof(buffer), 0);

        //Receiving: "read_value".
        if(valid_operation && (read(sockfd, buffer, sizeof(buffer)) > 0)) {

            switch (operation) {

                case '0':

                    list_all_locals(buffer);
                    break;

                case '1':

                    printf("> Last read from local: %s\n", buffer);
                    break;

                case '2':

                    printf("> %s\n", buffer);
                    break;

                default:
                    break;
            }
        }
    }

    close(sockfd);
    return 0;
}