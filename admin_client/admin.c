#include "../system_config.h"


int main(int argc, char const *argv[]) {

    int sockfd;

    struct sockaddr_in servaddr;

    char admin_settings[BUFFER_SIZE],
         address[INFO_SIZE],
         port[INFO_SIZE],
         admin_info_file[INFO_SIZE],
         admin_info[BUFFER_SIZE],
         id[INFO_SIZE],
         nickname[INFO_SIZE],
         buffer[BUFFER_SIZE];

    //Get Admin info file path passed in argv[1].
    if (argc < 2) {
        printf("\nUSAGE ERROR.\nUsage: ./admin ADMIN_SETTINGS.csv\n\n");
        exit(EXIT_FAILURE);
    }

    strcpy(admin_info_file, (char*) argv[1]);

    //Reads connection settings to the array.
    clearArray(admin_settings, BUFFER_SIZE);
    read_file_content(ADMIN_SETTINGS, admin_settings);

    //Sorting the info.
    get_info(admin_settings, address, ADMIN_ADDRESS);
    get_info(admin_settings, port, ADMIN_PORT);

    //Creating a new socket for the Admin.
    sockfd = new_socket();

    //Setting connection info.
    servaddr = set_connection_info(address, atoi(port));

    //Establishing the connection.
    create_connection(sockfd, servaddr);

    //Get Admin info from file.
    clearArray(admin_info, BUFFER_SIZE);
    read_file_content(admin_info_file, admin_info);

    get_info(admin_info, id, ADMIN_ID);
    get_info(admin_info, nickname, ADMIN_NICKNAME);

    //Creates the register message.
    snprintf(buffer, sizeof(buffer), "%s,%s", id, nickname);

    //Sends the register message.
    send(sockfd, buffer, sizeof(buffer), 0);

    /*TODO
     * Get from the std input which action the Admin wants to preform.
     */

    return 0;
}