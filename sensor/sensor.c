#include "../system_config.h"

struct sockaddr_in set_connection_info(char *hostname, int port) {

    struct sockaddr_in servaddr;
    struct hostent *server;

    server = gethostbyname(hostname);

    //Checking if the hostname provided was a valid one.
    if(server == NULL) {
        fprintf(stderr, "ERROR, no such host.\n");
        exit(1);
    }

    /*
        Copying the necessary info from "server" to "servaddr".
    */
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    bcopy((char *) server->h_addr, (char *) &servaddr.sin_addr.s_addr, server->h_length);

    return servaddr;
}

void create_connection(int sockfd, struct sockaddr_in servaddr) {

    //if the connection to the server doesn't succeed.
    if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {

        perror(">Connection to the server failed.\nAborted.\n");
        exit(2);

    //if the connection to the server succeeds.
    } else {

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(servaddr.sin_addr), addr, INET_ADDRSTRLEN);
        printf(">Connection established with: %s.\n", addr);
    }
}

int main(int argc, char const *argv[]) {

    int sockfd,
        read;
    struct sockaddr_in servaddr;

    time_t now;
    struct tm *time_info;

    char settings[BUFFER_SIZE],
        address[INFO_SIZE],
        port[INFO_SIZE],
        read_interval[INFO_SIZE],
        sensor_info_file[INFO_SIZE],
        sensor_info[BUFFER_SIZE],
        id[INFO_SIZE],
        type[INFO_SIZE],
        local[INFO_SIZE],
        firmware_version[INFO_SIZE],
        date[DATE_SIZE],
        buffer[BUFFER_SIZE];

    //Get client info from a file.
    clearArray(settings, BUFFER_SIZE);
    read_file_content(SENSOR_SETTINGS, settings);

    get_info(settings, address, ADDRESS);
    get_info(settings, port, PORT);
    get_info(settings, read_interval, READ_INTERVAL);

    sockfd = new_socket();

    servaddr = set_connection_info(address, atoi(port));
    create_connection(sockfd, servaddr);

    //Get sensor info from a file passed in argv[1].
    strcpy(sensor_info_file, (char*) argv[1]);

    clearArray(sensor_info, BUFFER_SIZE);
    read_file_content(sensor_info_file, sensor_info);

    get_info(sensor_info, id, ID);
    get_info(sensor_info, type, TYPE);
    get_info(sensor_info, local, LOCAL);
    get_info(sensor_info, firmware_version, FIRMWARE_VERSION);

    //Creates the register message.
    snprintf(buffer, sizeof(buffer), "%s,%s,%s,%s", id, type, local, firmware_version);

    //Sends the register message.
    send(sockfd, buffer, sizeof(buffer), 0);

    srand(time(NULL));
    /*
    Every X seconds (read_interval), sends a sensor read.
    For now, every read is a radom value.
    */
    for(;;) {

        time(&now);
        time_info = localtime(&now);

        sleep(atoi(read_interval));
        read = rand() % 500;

        snprintf(date, sizeof(date), "%d/%d/%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900);
        snprintf(buffer, sizeof(buffer), "%s,%s,%d,%s,%s", id, date, read, UNIT, firmware_version);

        send(sockfd, buffer, sizeof(buffer), 0);

        printf("READ SENT: %s", buffer);

    }

    close(sockfd);
    return 0;
}
