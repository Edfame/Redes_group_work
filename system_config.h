#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "queue/queue.h"

/*
 * SENSOR
 */

#define SENSOR_SETTINGS "sensor.csv"

#define ADDRESS 3
#define PORT 4
#define READ_INTERVAL 5
#define ID 4
#define TYPE 5
#define LOCAL 6
#define FIRMWARE_VERSION 7
#define UNIT "µg/m³"
#define READ_AMOUNTS 10

/*
 * ADMIN
 */

#define ADMIN_SETTINGS "admin.csv"

#define ADMIN_ADDRESS 2
#define ADMIN_PORT 3
#define ADMIN_ID 2
#define ADMIN_NICKNAME 3
#define ADMIN_DELIM ";\n"
/*
 * GENERAL
 */

#define BUFFER_SIZE 256
#define INFO_SIZE 16
#define DATE_SIZE 12
#define DELIM ",\n"

typedef enum fd_type {

    NONE,
    FD_S,
    FD_C,
    FD_A

} fd_type;

typedef struct identifier{

    fd_type type;
    char *client_info;
    queue *last_reads;
    int *subscribed_sensors;

} identifier;

identifier *new_identifier(fd_type fd_type);

/*
    General
*/
int new_socket();

struct sockaddr_in set_connection_info(char *hostname, int port);

void create_connection(int sockfd, struct sockaddr_in servaddr);

void read_file_content(char *file_name, char *dest);

void clearArray(char string[], int length);

void get_info(char *src, char *dest, int step);

#endif
