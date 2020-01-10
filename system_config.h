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
 * BROKER
 */
#define BROKER_SENSOR_PORT 3
#define BROKER_SENSOR_ID 0
#define BROKER_CLIENT_PORT 4
#define BROKER_ADMIN_PORT 5
#define MAX_CLIENTS 200

/*
 * SENSOR
 */
#define SENSOR_ADDRESS 3
#define SENSOR_PORT 4
#define SENSOR_READ_INTERVAL 5
#define SENSOR_ID 4
#define SENSOR_TYPE 5
#define SENSOR_LOCAL 6
#define SENSOR_FIRMWARE_VERSION 7
#define SENSOR_UNIT "µg/m³"

/*
 * ADMIN
 */
#define ADMIN_ADDRESS 2
#define ADMIN_PORT_CLIENT 3
#define ADMIN_ID 2
#define ADMIN_NICKNAME 3
#define ADMIN_DELIM ";\n"
#define ADMIN_SENSOR_NOT_FOUND "Sensor not found."

/*
 * CLIENT
 */
#define CLIENT_ADDRESS 2
#define CLIENT_PORT_CLIENT 3
#define CLIENT_ID 2
#define CLIENT_NICKNAME 3
#define CLIENT_LOCAL_NOT_FOUND "Local not found."
#define CLIENT_SENSOR_TYPE 1
#define CLIENT_SENSOR_LOCAL 2
#define CLIENT_DELIM "|\n"
#define CLIENT_SUBSCRIBED 1
#define CLIENT_UNSUBSCRIBED 0
#define CLIENT_SUBSCRIBED_MSG "Subscribed."

/*
 * GENERAL
 */
#define OPERATION_INDEX 0
#define INFO_INDEX 1
#define BUFFER_SIZE 256
#define INFO_SIZE 16
#define DATE_SIZE 12
#define DELIM ",\n"

int max(int first_number, int second_number);

int max3(int first_number, int second_number, int third_number);

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
    short *subscribed_sensors;

} identifier;

identifier *new_identifier(fd_type fd_type);

/*
    General
*/
int new_socket();

struct sockaddr_in set_connection_info(char *hostname, int port);

void create_connection(int sockfd, struct sockaddr_in servaddr);

void read_file_content(char *file_name, char *dest);

void get_info(char *src, char *dest, int step, char *delim);

#endif
