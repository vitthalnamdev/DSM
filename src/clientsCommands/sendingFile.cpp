#include "../../headers/clientsCommand.hpp"
#include "../../headers/shareFile.hpp"
#include "../../headers/Status_codes.hpp"

void handle_send_file()
{

    printf("Enter the I.P that you want to share the file with: ");
    char IP[16];
    if (!fgets(IP, sizeof(IP), stdin))
    {
        perror("Failed to read IP address");
        return;
    }

    IP[strcspn(IP, "\n")] = 0;

    char filename[256];

    printf("Enter the file path that you want to share: ");

    if (!fgets(filename, sizeof(filename), stdin))
    {
        perror("Failed to read the filename.");
        return;
    }

    filename[strcspn(filename, "\n")] = 0;

    printf("Sharing file %s with %s...\n", filename, IP);

   

    int file_sent = send_file(filename , IP);

    if(file_sent > 0){
        printf("File has been send successfully");
    }
    else{
        printf("There is an error in sending file");
    }
}