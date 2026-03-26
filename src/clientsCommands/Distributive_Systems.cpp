#include "../../headers/clientsCommand.hpp"
#include "../../headers/shareFile.hpp"

int findTotatConnections()
{
    int connections = 0;
    while (ip_list[connections] != NULL)
        connections++;
    return connections;
}

void freeMemory()
{
    for (int i = 0; ip_list[i] != NULL; i++)
    {
        free(ip_list[i]);
        free(ip_status[i]);
    }
    free(ip_list);
    free(ip_status);
}

void *distributiveComputing(void *args)
{
    struct distributiveComputingargs *dis_args = (struct distributiveComputingargs *)args;
    int res = send_file(dis_args->codePath, dis_args->IP, "Code", 0);
    if (res > 0)
    {
        printf("Success");
    }
    return NULL;
}

void handle_distributive_systems()
{
    // Send UDP packet to all the IP's in the network, to know which IP's are actually running the service.
    sendRequest();

    int totalConnections = findTotatConnections();

    // Opening the reciever's endpoint for the IP's connected to the network..
    const char *command = "receiveFile";
    for (int i = 0; ip_list[i] != NULL; i++)
    {
        char *responce = sendToServer(command, ip_list[i]);
    }

    printf("Enter the code file path:");
    char codepath[128];
    if (!fgets(codepath, sizeof(codepath), stdin))
    {
        perror("Failed to read codepath");
        return;
    }
    codepath[strcspn(codepath, "\n")] = 0;

    clear_stdin();

    char trainfilepath[128];

    if (!fgets(trainfilepath, sizeof(trainfilepath), stdin))
    {
        perror("Failed to read trainfilepath");
        return;
    }
    trainfilepath[strcspn(trainfilepath, "\n")] = 0;

    clear_stdin();

    // Make threads equal to totalConnections.

    pthread_t threads[totalConnections];

    for (int i = 0; i < totalConnections; i++)
    {
        struct distributiveComputingargs dis_args;

        dis_args.codePath = codepath;
        dis_args.trainFilePath = trainfilepath;
        dis_args.IP = ip_list[i];

        if (pthread_create(&threads[i], NULL, distributiveComputing, &dis_args) != 0)
        {
            perror("Failed to create distributive computing thread");
            return;
        }
    }

    // Starting each thread..
    for (int i = 0; i < totalConnections; i++)
    {
        pthread_join(threads[i], NULL);
    }

    freeMemory();
}