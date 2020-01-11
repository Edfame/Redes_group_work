#include "../system_config.h"

void print_operations() {

    printf("0 - Get X's last read.\n1 - List all the sensors registered on the system.\n2 - Send firmware update file to Y type sensors.\n3 - Deactivate sensor with ID X.\n");
}

void list_all_sensors(char *buffer) {

    char to_print[BUFFER_SIZE],
         sensors[INFO_SIZE];

    short sensors_counter = 0;

    bzero(to_print, sizeof(to_print));
    bzero(sensors, sizeof(sensors));

    get_info(buffer, sensors, 0, ADMIN_DELIM);

    printf("> Sensors:\n");

    if((sensors_counter = atoi(sensors)) == 0) {

        printf("> \t- %s\n", ADMIN_SENSOR_NOT_FOUND);
        return;
    }

    for (int i = 1; i <= sensors_counter; i++) {

        get_info(buffer, to_print, i, ADMIN_DELIM);
        printf("\t- %s\n", to_print);
    }
}

int read_update_file(char *file_name, char *dest) {

    //Opens the file in read mode.
    FILE *file = fopen(file_name, "r");

    if(file == NULL)
    {
        printf("No such file \"%s\"\n", file_name);
        return 0;

    } else {
        printf("Opened: %s\n", file_name);
    }

    char buffer[BUFFER_SIZE];

    while(fscanf(file, "%s", buffer)!=EOF)
    {
        strcat(dest,buffer);
    }

    fclose(file);

    return 1;
}

int main(int argc, char const *argv[]) {

    int sockfd,
        valid_operation = 1;

    struct sockaddr_in servaddr;

    char operation,
         admin_settings[BUFFER_SIZE],
         address[INFO_SIZE],
         port[INFO_SIZE],
         admin_settings_file[INFO_SIZE],
         admin_info_file[INFO_SIZE],
         admin_info[BUFFER_SIZE],
         id[INFO_SIZE],
         nickname[INFO_SIZE],
         buffer[BUFFER_SIZE],
         input[INFO_SIZE],
         file_content[BUFFER_SIZE],
         sensor_type[INFO_SIZE];

    //Get Admin files.
    if (argc < 3) {
        printf("\nUSAGE ERROR.\nUsage: ./admin ADMIN_SETTINGS.csv ADMIN_ID.csv\n\n");
        exit(EXIT_FAILURE);
    }

    strcpy(admin_settings_file, (char*) argv[1]);
    strcpy(admin_info_file, (char*) argv[2]);

    //Reads connection settings to the array.
    bzero(admin_settings, BUFFER_SIZE);
    read_file_content(admin_settings_file, admin_settings);

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
    bzero(admin_info, BUFFER_SIZE);
    read_file_content(admin_info_file, admin_info);

    get_info(admin_info, id, ADMIN_ID, DELIM);
    get_info(admin_info, nickname, ADMIN_NICKNAME, DELIM);

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

                scanf(" %s", input);
                snprintf(buffer, sizeof(buffer), "%c,%s", operation, input);
                break;

            case '1':

                snprintf(buffer, sizeof(buffer), "%c", operation);
                break;

            case '2':

                bzero(file_content, sizeof(file_content));
                bzero(sensor_type, sizeof(sensor_type));

                scanf(" %s %s", input, sensor_type);
                valid_operation = read_update_file(input, file_content);

                snprintf(buffer, sizeof(buffer) + sizeof(sensor_type) + 2, "%c,%s_%s", operation, file_content, sensor_type);
                break;

            case '3':

                scanf(" %s", input);
                snprintf(buffer, sizeof(buffer), "%c,%s", operation, input);
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
        //Receiving: "read_value".
        if(valid_operation && (read(sockfd, buffer, sizeof(buffer)) > 0)) {
            //recv(sockfd, buffer, sizeof(buffer), 0);
            switch (operation) {

                case '0':

                    printf("> Last read: %s\n", buffer);
                    break;

                case '1':

                    list_all_sensors(buffer);
                    break;

                case '2':

                case '3':

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