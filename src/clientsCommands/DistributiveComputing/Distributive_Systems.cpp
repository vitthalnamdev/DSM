#include "../../../headers/clientsCommand.hpp"
#include "../../../headers/shareFile.hpp"
#include "../../../headers/threadSafety.hpp"
#include "../../../headers/Status_codes.hpp"

off_t *train_offset;
int_fast64_t *train_chunk_size;

char reducerPath[128] = "/home/vitthal/dsm/src/clientsCommands/DistributiveComputing/Reducers/sum.cpp";

// ---------------------- UTIL ----------------------

int findTotalConnections()
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
    free(train_offset);
    free(train_chunk_size);
}

int_fast64_t getFileSize(const char *filePath)
{
    struct stat st;
    if (stat(filePath, &st) == 0)
        return st.st_size;

    perror("Failed to get file size");
    return -1;
}

// ---------------------- SAFE LINE ALIGNMENT ----------------------

int_fast64_t adjustToNextLine(int fd, int_fast64_t pos, int_fast64_t fileSize)
{
    if (pos >= fileSize)
        return fileSize;

    const size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];

    int_fast64_t start = pos;

    while (pos < fileSize && pos - start < BUF_SIZE * 4) // LIMIT scanning
    {
        ssize_t bytesRead = pread(fd, buffer, BUF_SIZE, pos);
        if (bytesRead <= 0)
            break;

        for (ssize_t i = 0; i < bytesRead; i++)
        {
            if (buffer[i] == '\n')
                return pos + i + 1;
        }

        pos += bytesRead;
    }

    // fallback (don’t let it eat whole file)
    return start;
}

// ---------------------- CORE LOGIC ----------------------

void divideFileIntoChunks(const char *trainfilepath, int totalConnections)
{
    int fd = open(trainfilepath, O_RDONLY);
    if (fd < 0)
    {
        perror("File open failed");
        return;
    }

    int_fast64_t fileSize = getFileSize(trainfilepath);
    if (fileSize <= 0)
    {
        perror("Invalid file size");
        close(fd);
        return;
    }

    // ✅ Compute total weight correctly
    int totalWeight = 0;
    for (int i = 0; i < totalConnections; i++)
    {
        totalWeight += (i == 0) ? 3 : 1;
    }

    int_fast64_t offset = 0;

    for (int i = 0; i < totalConnections; i++)
    {
        int currWeight = (i == 0) ? 3 : 1;

        int_fast64_t ideal_chunk = (fileSize * currWeight) / totalWeight;
        int_fast64_t tentative_end = offset + ideal_chunk;
        int_fast64_t adjusted_end;

        if (i == totalConnections - 1)
        {
            // last node gets remaining
            adjusted_end = fileSize;
        }
        else
        {
            adjusted_end = adjustToNextLine(fd, tentative_end, fileSize);

            // safeguard: don’t exceed too much
            if (adjusted_end <= offset || adjusted_end > tentative_end + ideal_chunk)
            {
                adjusted_end = tentative_end;
            }
        }

        int_fast64_t final_chunk = adjusted_end - offset;

        train_offset[i] = offset;
        train_chunk_size[i] = final_chunk;

        offset = adjusted_end;
    }

    close(fd);
}

// ---------------------- THREAD HANDLING ----------------------

void *send_file_thread(send_file_args fargs)
{
    int result;

    if (fargs.use_chunk)
    {
        result = send_file(fargs.filePath, fargs.IP, fargs.tag,
                           fargs.iscmdSendFile,
                           fargs.offset,
                           fargs.chunk_size);
    }
    else
    {
        result = send_file(fargs.filePath, fargs.IP, fargs.tag,
                           fargs.iscmdSendFile);
    }

    if (result < 0)
    {
        fprintf(stderr, "Failed to send %s file to %s\n", fargs.tag, fargs.IP);
    }

    return NULL;
}

void *distributiveComputingOverNetwork(void *args)
{
    auto *dis_args = static_cast<distributiveComputingargs *>(args);

    char *res = sendToServer("distributiveComputing", dis_args->IP);

    if (strcmp(res, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]) != 0)
    {
        printf("Connection failed for IP: %s\n", dis_args->IP);
        return NULL;
    }

    printf("Network → Offset: %ld, Size: %ld\n",
           dis_args->train_offset,
           dis_args->train_chunk_size);

    const bool iscmdSendFile = false;

    send_file_args code_args{
        dis_args->codePath,
        dis_args->IP,
        "code",
        iscmdSendFile};

    send_file_args reducer_args{
        reducerPath,
        dis_args->IP,
        "reducer",
        iscmdSendFile};

    send_file_args train_args{
        dis_args->trainFilePath,
        dis_args->IP,
        "train",
        iscmdSendFile,
        dis_args->train_offset,
        dis_args->train_chunk_size,
        true};

    std::thread t1(send_file_thread, code_args);
    std::thread t2(send_file_thread, reducer_args);
    std::thread t3(send_file_thread, train_args);

    t1.join();
    t2.join();
    t3.join();

    return NULL;
}

void *distributiveComputingLocal(void *args)
{
    auto *dis_args = (distributiveComputingargs *)args;

    // TODO: execute locally on chunk
    printf("LOCAL EXEC → IP: %s, Offset: %ld, Size: %ld\n",
           dis_args->IP,
           dis_args->train_offset,
           dis_args->train_chunk_size);

    return NULL;
}

// ---------------------- MAIN HANDLER ----------------------

void handle_distributive_systems()
{
    sendRequest();

    int totalConnections = findTotalConnections();

    train_offset = (off_t *)malloc(sizeof(off_t) * totalConnections);
    train_chunk_size = (int_fast64_t *)malloc(sizeof(int_fast64_t) * totalConnections);

    char codepath[128], trainfilepath[128];

    printf("Enter code path: ");
    fgets(codepath, sizeof(codepath), stdin);
    codepath[strcspn(codepath, "\n")] = 0;

    printf("Enter train file path: ");
    fgets(trainfilepath, sizeof(trainfilepath), stdin);
    trainfilepath[strcspn(trainfilepath, "\n")] = 0;

    divideFileIntoChunks(trainfilepath, totalConnections);

    // Debug output
    for (int i = 0; i < totalConnections; i++)
    {
        printf("IP: %s | Offset: %ld | Chunk: %ld\n",
               ip_list[i], train_offset[i], train_chunk_size[i]);
    }

    std::thread threads[totalConnections];


    for (int i = 0; i < totalConnections; i++)
    {
        auto *dis_args = (distributiveComputingargs *)malloc(sizeof(distributiveComputingargs));

        dis_args->codePath = codepath;
        dis_args->trainFilePath = trainfilepath;
        dis_args->IP = ip_list[i];
        dis_args->train_offset = train_offset[i];
        dis_args->train_chunk_size = train_chunk_size[i];

        if (i == 0)
            threads[i] = std::thread(distributiveComputingLocal, dis_args);
        else
            threads[i] = std::thread(distributiveComputingOverNetwork, dis_args);
    }

    for (int i = 0; i < totalConnections; i++)
        threads[i].join();

    freeMemory();
}