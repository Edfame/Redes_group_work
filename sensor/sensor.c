#include "../system_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define SENSOR_SETTINGS "sensor.csv"

#define BUFFER_SIZE 256
#define INFO_SIZE 16
#define ADDRESS 3
#define PORT 4
#define READ_INTERVAL 5
#define ID 4
#define TYPE 5
#define LOCAL 6
#define FIRMWARE_VERSION 7

void read_file_content(char *file_name, char *characters) {

    //Opens the file in read mode.
    FILE *file = fopen(file_name, "r");

    if(file == NULL) {
        printf("No such file \"%s\"\n", file_name);
        exit(EXIT_FAILURE);

    } else {

        printf("Openned: %s\n", file_name);
    }

    char character;

    for(int i = 0; i < BUFFER_SIZE; i++) {

        character = fgetc(file);

       if (character != EOF) {

            characters[i] = character;
        }
    }

    fclose(file);
}

void clearArray(char string[], int length) {
    int i = 0;
    while(i < length)
    {
        string[i]='\0';
        i++;
    }
}

void get_info(char *file_name, char dest[], int step) {
    int i = 0;
    short commaCounter = 0;

    char src[BUFFER_SIZE];
    clearArray(src, BUFFER_SIZE);
    read_file_content(file_name, src);

    while(commaCounter<step)
    {
        if(src[i]==',' || src[i] == '\n')
        {
            commaCounter++;
        }
        i++;
    }

    int j = 0;

    clearArray(dest, INFO_SIZE);
    while(src[i]!=',' && src[i]!='\0')
    {
        dest[j]=src[i];
        j++;
        i++;
    }
}

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

    int sockfd;
    struct sockaddr_in servaddr;

    char address[INFO_SIZE],
        port[INFO_SIZE],
        read_interval[INFO_SIZE],
        sensor_info[INFO_SIZE],
        id[INFO_SIZE],
        type[INFO_SIZE],
        local[INFO_SIZE],
        firmware_version[INFO_SIZE];

    //Get client info from a file.
    get_info(SENSOR_SETTINGS, address, ADDRESS);
    get_info(SENSOR_SETTINGS, port, PORT);
    get_info(SENSOR_SETTINGS, read_interval, READ_INTERVAL);

    sockfd = new_socket();

    servaddr = set_connection_info(address, atoi(port));
    create_connection(sockfd, servaddr);

    //Get sensor info from a file passed in argv[1].
    strcpy(sensor_info, (char*) argv[1]);

    get_info(sensor_info, id, ID);
    get_info(sensor_info, type, TYPE);
    get_info(sensor_info, local, LOCAL);
    get_info(sensor_info, firmware_version, FIRMWARE_VERSION);

    sensor *sensor = new_sensor(atoi(id), type, local, (float) atoi(firmware_version));

    printf("I sent: %d, %s, %s, %f, %d\n",
           sensor->id,
           sensor->type,
           sensor->local,
           sensor->firmware_version,
           sensor->read_value);

    send(sockfd, sensor, sizeof(struct sensor), 0);
    printf("REGISTER MSG SENT.\n");

    /*
    Every X seconds, that comes from YAML file (read_interval), sends a sensor read.
    For now, every read is a radom value.
    */

    //read_interval_int = atoi(read_interval);

    srand(time(NULL));

    for(;;) {

        sleep(atoi(read_interval));
        sensor->read_value = rand() % 500;
        send(sockfd, sensor, sizeof(struct sensor), 0);

        printf("READ SENT: %d\n", sensor->read_value);

    }

    close(sockfd);
    free(sensor);

    return 0;
}
