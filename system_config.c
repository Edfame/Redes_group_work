#include "system_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256
#define INFO_SIZE 16

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
