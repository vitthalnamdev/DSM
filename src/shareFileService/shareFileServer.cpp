#include "../../headers/serverService.hpp"
#include "../../headers/shareFile.hpp"
#include "../../headers/Status_codes.hpp"

#define PORT 8080
#define BLOCK_SIZE_ 65536

void create_dirs(const char *path)
{
    char temp[512];
    snprintf(temp, sizeof(temp), "%s", path);

    for (char *p = temp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = '\0';
            mkdir(temp, 0755);
            *p = '/';
        }
    }
    mkdir(temp, 0755);
}

// Receive exactly len bytes from socket
int recv_all(int sock, void *buf, size_t len)
{
    char *ptr = static_cast<char *>(buf);

    while (len > 0)
    {
        ssize_t bytes = recv(sock, ptr, len, 0);

        // Stop on socket error or close
        if (bytes <= 0)
            return false;

        ptr += bytes;
        len -= bytes;
    }

    return true;
}

// Receive file metadata and file data
void receive_file(int sock, u_int8_t askclientShareFile)
{
    uint32_t namelen;

    uint32_t folderlen;

    // Receive folder length
    if (!recv_all(sock, &folderlen, sizeof(folderlen)))
    {
        std::cerr << "Failed to receive folder length\n";
        return;
    }

    // Validate folder length
    if (folderlen == 0 || folderlen >= 256)
    {
        std::cerr << "Invalid folder length\n";
        return;
    }

    char folder[256];

    // Receive folder name
    if (!recv_all(sock, folder, folderlen))
    {
        std::cerr << "Failed to receive folder name\n";
        return;
    }

    folder[folderlen] = '\0';

    if (!recv_all(sock, &namelen, sizeof(namelen)))
    {
        std::cerr << "Failed to receive filename length\n";
        return;
    }

    // Validate filename length
    if (namelen == 0 || namelen >= 256)
    {
        std::cerr << "Invalid filename length: " << namelen << "\n";
        return;
    }

    char filename[256];

    // Read filename
    if (!recv_all(sock, filename, namelen))
    {
        std::cerr << "Failed to receive filename\n";
        return;
    }

    // Add null terminator
    filename[namelen] = '\0';

    uint64_t filesize;

    // Read file size
    if (!recv_all(sock, &filesize, sizeof(filesize)))
    {
        std::cerr << "Failed to receive filesize\n";
        return;
    }

    char cwd[1024];

    // Print server working directory
    if (getcwd(cwd, sizeof(cwd)) && askclientShareFile)
    {
        std::cout << "Server working directory: " << cwd << "\n";
    }

    // Create base folder
    mkdir("received_files", 0755);

    // Security check
    if (strstr(folder, ".."))
    {
        std::cerr << "Invalid folder path\n";
        return;
    }

    // Build folder path
    char folder_path[512];
    snprintf(folder_path, sizeof(folder_path), "received_files/%s", folder);

    // Create directories recursively
    create_dirs(folder_path);

    // Extract base filename
    const char *base = strrchr(filename, '/');
    base = (base != nullptr) ? base + 1 : filename;

    // Build final file path
    char outpath[512];
    int ret = snprintf(outpath, sizeof(outpath), "%s/%s", folder_path, base);

    if (ret < 0 || ret >= (int)sizeof(outpath))
    {
        std::cerr << "Path too long\n";
        return;
    }

    // Open output file
    int file = open(outpath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file < 0)
    {
        perror("open");
        std::cerr << "Tried to create file at: " << outpath << "\n";
        return;
    }

    char buffer[BLOCK_SIZE_];
    uint64_t received = 0;

    TransferStats stats;
    ProgressUI ui;

    if (askclientShareFile)
    {
        stats.start(filesize);
    }

    // Receive file content
    while (received < filesize)
    {
        size_t need = filesize - received;

        // Limit read size to buffer size
        if (need > sizeof(buffer))
            need = sizeof(buffer);

        // Read file chunk from socket
        ssize_t bytes = recv(sock, buffer, need, 0);
        if (bytes <= 0)
        {
            std::cerr << "Socket closed or recv failed while receiving file data\n";
            break;
        }

        // Write chunk to file
        ssize_t written = write(file, buffer, bytes);
        if (written != bytes)
        {
            perror("write");
            close(file);
            return;
        }

        received += bytes;
        if (askclientShareFile)
        {
            stats.update(bytes);
            ui.render(stats);
        }
    }

    if (askclientShareFile)
    {
        ui.done();
    }

    // Close file after writing
    close(file);

    // Confirm full file received
    if (received == filesize)
    {
        if (askclientShareFile)
        {
            std::cout << "File received successfully: " << outpath
                      << " (" << filesize << " bytes)\n";
        }

        const char *msg = "SUCCESS";

        // Send success reply to client
        send(sock, msg, strlen(msg), 0);
    }
    else
    {
        std::cout << "Incomplete file received: " << outpath
                  << " (" << received << "/" << filesize << " bytes)\n";
    }
    OPEN_RECEIVE_FILE_CONNECTION = 0; // Reset the flag after handling the file transfer
}

bool checkChar(char choice)
{
    return (choice == 'a' || choice == 'A' || choice == 'r' || choice == 'R');
}

void handle_share_file_server(int sock)
{
    if (OPEN_RECEIVE_FILE_CONNECTION == 0)
    {
        send(sock, STATUS_MESSAGES[RECEIVER_NO_OPEN_CONNECTION],
             strlen(STATUS_MESSAGES[RECEIVER_NO_OPEN_CONNECTION]), 0);
        return;
    }
    else
    {
        send(sock, STATUS_MESSAGES[SUCCESS],
             strlen(STATUS_MESSAGES[SUCCESS]), 0);
    }

    uint8_t askClientShareFile;
    if (!recv_all(sock, &askClientShareFile, sizeof(askClientShareFile)))
    {
        std::cerr << "Failed to receive data from the client\n";
        return;
    }

    if (!askClientShareFile)
    {
        receive_file(sock, askClientShareFile);
        return;
    }

    std::string input;
    char choice = '\0';
    while (true)
    {
        std::cout << client_ip
                  << " wants to send you a file. (A/R): ";

        std::getline(std::cin, input);

        if (input.empty())
            continue;

        choice = input[0];

        if (checkChar(choice))
            break;

        std::cout << "Invalid input. Please enter A or R.\n";
    }

    if (choice == 'a' || choice == 'A')
    {
        send(sock, STATUS_MESSAGES[SUCCESS],
             strlen(STATUS_MESSAGES[SUCCESS]), 0);

        receive_file(sock, askClientShareFile);
    }
    else
    {
        send(sock, STATUS_MESSAGES[REJECT_CONNECTION],
             strlen(STATUS_MESSAGES[REJECT_CONNECTION]), 0);
    }
}