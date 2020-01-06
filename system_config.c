#include "system_config.h"

identifier *new_identifier(fd_type fd_type) {

    identifier *new_identifier = malloc(sizeof(struct identifier));

    new_identifier->type = fd_type;
    new_identifier->client_info = NULL;
    new_identifier->last_reads = NULL;
    new_identifier->subscribed_sensors = NULL;

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

void read_file_content(char *file_name, char *dest) {

    //Opens the file in read mode.
    FILE *file = fopen(file_name, "r");

    if(file == NULL)
    {
        printf("No such file \"%s\"\n", file_name);
        exit(EXIT_FAILURE);

    } else {
        printf("Opened: %s\n", file_name);
    }

    char buffer[BUFFER_SIZE];

    while(fscanf(file, "%s", buffer)!=EOF)
    {
        strcat(buffer,",");
        strcat(dest,buffer);

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

void get_info(char *src, char *dest, int step)
{

    char src_cpy[BUFFER_SIZE],
        *token;

    clearArray(src_cpy, BUFFER_SIZE);
    strcpy(src_cpy, src);
    token = strtok(src_cpy, ",\n");

    int counter = 0;

    while(counter<step)
    {
        token = strtok(NULL, ",\n");
        counter++;
    }

    strcpy(dest,token);
}
