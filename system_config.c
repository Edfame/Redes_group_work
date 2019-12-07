#include "system_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

identifier *new_identifier(fd_type fd_type) {

    identifier *new_identifier = malloc(sizeof(struct identifier));

    new_identifier->type = fd_type;

    return new_identifier;
}

int new_socket() {

    int new_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(new_socket < 0) {

        perror(">Socket creation failed.\nAborted.\n");
        exit(1);

    } else {

        printf(">Socket creation succeeded.\n");

    }

    return new_socket;
}

sensor *new_sensor(short id, char type[], char local[], float firmware_version) {

    sensor *new_sensor = malloc(sizeof(struct sensor));

    new_sensor->id = id;
    strcpy(new_sensor->type, type);
    strcpy(new_sensor->local, local);
    new_sensor->firmware_version = firmware_version;

    return new_sensor;
}
