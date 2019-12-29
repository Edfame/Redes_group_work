#ifndef SYSTEM_H_
#define SYSTEM_H_

#define BUFFER_SIZE 256
#define INFO_SIZE 16
#define DATE_SIZE 12
#define UNIT "µg/m³"

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

void read_file_content(char *file_name, char *dest);

void clearArray(char string[], int length);

void get_info(char *src, char *dest, int step);

#endif
