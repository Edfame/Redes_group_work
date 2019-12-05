#ifndef SYSTEM_H_
#define SYSTEM_H_

/*
    General
*/
int new_socket();

/*
    Sensors
*/
#define LOCAL_SIZE 32
#define TYPE_SIZE 3

typedef struct sensor {

    short id;
    char type[TYPE_SIZE];
    char local[LOCAL_SIZE];
    float firmware_version;
    short read_value;

} sensor;

sensor *new_sensor(short id, char type[], char local[], float firmware_version);

#endif
