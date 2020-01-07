#include "../system_config.h"

void print_operations() {

    printf("0 - Get X's last read.\n1 - List all the sensors registered on the system.\n2 - Send firmware update file to Y type sensors.\n3 - Deactivate sensor with ID X.\n");
}

void list_all_sensors(int sockfd, char operation) {

    char buffer[BUFFER_SIZE],
         to_print[BUFFER_SIZE];

    clearArray(buffer, BUFFER_SIZE);
    snprintf(buffer, sizeof(buffer), "%c", operation);

    send(sockfd, buffer, sizeof(buffer), 0);

    recv(sockfd, buffer, sizeof(buffer), 0);

    for (int i = 0; i < strlen(buffer); ++i) {
        get_info(buffer, to_print, i, ADMIN_DELIM);
        printf("\t%s\n", to_print);
    }
}

void get_last_read(int sockfd, char op, char *buffer) {

    printf("%s's last read: ", buffer);

    snprintf(buffer, sizeof(buffer), "%c,%s", op, buffer);

    //Sending: "operation,ID".
    send(sockfd, buffer, sizeof(buffer), 0);

    //Receiving: "read_value".
    recv(sockfd, buffer, sizeof(buffer), 0);

    printf("%s\n", buffer);
}

int main(int argc, char const *argv[]) {

    int sockfd;

    struct sockaddr_in servaddr;

    char operation,
         admin_settings[BUFFER_SIZE],
         address[INFO_SIZE],
         port[INFO_SIZE],
         admin_info_file[INFO_SIZE],
         admin_info[BUFFER_SIZE],
         id[INFO_SIZE],
         nickname[INFO_SIZE],
         buffer[BUFFER_SIZE];

    //Get Admin info file path passed in argv[1].
    if (argc < 2) {
        printf("\nUSAGE ERROR.\nUsage: ./admin ADMIN_SETTINGS.csv\n\n");
        exit(EXIT_FAILURE);
    }

    strcpy(admin_info_file, (char*) argv[1]);

    //Reads connection settings to the array.
    clearArray(admin_settings, BUFFER_SIZE);
    read_file_content(ADMIN_SETTINGS, admin_settings);

    //Sorting the info.
    get_info(admin_settings, address, ADMIN_ADDRESS, DELIM);
    get_info(admin_settings, port, ADMIN_PORT_CLIENT, DELIM);

    //Creating a new socket for the Admin.
    sockfd = new_socket();

    //Setting connection info.
    servaddr = set_connection_info(address, atoi(port));

    //Establishing the connection.
    create_connection(sockfd, servaddr);

    //Get Admin info from file.
    clearArray(admin_info, BUFFER_SIZE);
    read_file_content(admin_info_file, admin_info);

    get_info(admin_info, id, ADMIN_ID, DELIM);
    get_info(admin_info, nickname, ADMIN_NICKNAME, DELIM);

    //Creates the register message.
    snprintf(buffer, sizeof(buffer), "%s,%s", id, nickname);

    //Sends the register message.
    send(sockfd, buffer, sizeof(buffer), 0);

    /*TODO
     * Get from the std input which action the Admin wants to preform.
     */
    print_operations();

    while (scanf("%c", &operation) != EOF) {

        switch (operation) {

            case '0':

                scanf(" %s", buffer);
                get_last_read(sockfd, operation, buffer);
                break;

            case '1':

                list_all_sensors(sockfd, operation);
                break;

            case '2':

                break;

            case '3':
                break;
            case '4':
                break;

            default:

                printf("> Invalid Operation.\n");
                print_operations();
                break;
        }
    }

    close(sockfd);
    return 0;
}