#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

#define ADDRESS 3
#define PORT 4
#define READ_INTERVAL 5

char* read_file_content(char *file_name, char *characters) {

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
    return characters;
}

char* get_info(char *file_name, int field) {

    char content[BUFFER_SIZE];
    read_file_content("test.csv", content);

    char *messageAux;

    messageAux = strtok(content,",\n");

    for(int i = 0; i < PORT; i++) {
        messageAux = strtok(NULL,",\n");
    }

    return messageAux;
}
