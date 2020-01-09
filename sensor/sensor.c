#include "../system_config.h"

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
        sensor_settings_file[INFO_SIZE],
        sensor_info_file[INFO_SIZE],
        sensor_info[BUFFER_SIZE],
        id[INFO_SIZE],
        type[INFO_SIZE],
        local[INFO_SIZE],
        firmware_version[INFO_SIZE],
        date[DATE_SIZE],
        buffer[BUFFER_SIZE];

    //Get sensor info file path passed in argv[1].
    if (argc < 3) {
        printf("\nUSAGE ERROR.\nUsage: ./sensor SENSOR_SETTINGS.csv SENSOR_ID.csv\n\n");
        exit(EXIT_FAILURE);
    }
    strcpy(sensor_settings_file, (char*) argv[1]);
    strcpy(sensor_info_file, (char*) argv[2]);

    //Get connection info from a file.
    bzero(settings, BUFFER_SIZE);
    read_file_content(sensor_settings_file, settings);

    //Sorting the info.
    get_info(settings, address, SENSOR_ADDRESS, DELIM);
    get_info(settings, port, SENSOR_PORT, DELIM);
    get_info(settings, read_interval, SENSOR_READ_INTERVAL, DELIM);

    //Creating a new socket for the sensor.
    sockfd = new_socket();

    //Setting connection info.
    servaddr = set_connection_info(address, atoi(port));

    //Establishing the connection.
    create_connection(sockfd, servaddr);

    //Get sensor info from file.
    bzero(sensor_info, BUFFER_SIZE);
    read_file_content(sensor_info_file, sensor_info);

    get_info(sensor_info, id, SENSOR_ID, DELIM);
    get_info(sensor_info, type, SENSOR_TYPE, DELIM);
    get_info(sensor_info, local, SENSOR_LOCAL, DELIM);
    get_info(sensor_info, firmware_version, SENSOR_FIRMWARE_VERSION, DELIM);

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
        time_info = localtime(&now);

        sleep(atoi(read_interval));
        read = rand() % 500;

        snprintf(date, sizeof(date), "%d/%d/%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900);
        snprintf(buffer, sizeof(buffer), "%s,%s,%d,%s,%s", id, date, read, SENSOR_UNIT, firmware_version);

        send(sockfd, buffer, sizeof(buffer), 0);

        printf("READ SENT: %s\n", buffer);

        bzero(buffer, sizeof(buffer));
        //Firmware updated received.
        if(recv(sockfd, buffer, sizeof(buffer), 0) > 0) {
            //TODO Handle update.
            printf("UPDATE: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
