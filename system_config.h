#ifndef SYSTEM_H_
#define SYSTEM_H_

typedef enum fd_type {

    NONE,
    FD_S,
    FD_C,
    FD_A

} fd_type;

typedef struct identifier{

    fd_type type;

} identifier;

identifier *new_identifier(fd_type fd_type);

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
