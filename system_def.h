#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 64
#define LOCAL_SIZE 32
#define TYPE_SIZE 3

typedef struct sensor_message {

    char message_type;         //r - register, v - new read value.
    short message_content;     //the value of the read, -1 if there's no read.
    struct sensor *sensor;

} sensor_message;

typedef struct sensor {

    short id;
    char type[TYPE_SIZE];
    char local[LOCAL_SIZE];
    float firmware_version;

} sensor;

sensor_message *new_sensor_message(char message_type, sensor *sensor);
sensor *new_sensor(short id, char type[], char local[], float firmware_version);
