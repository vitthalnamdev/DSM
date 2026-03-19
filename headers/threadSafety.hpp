#ifndef THREAD_SAFETY_H
#define THREAD_SAFETY_H

#include <pthread.h>

typedef struct
{
    pthread_mutex_t lock; // The lock
    pthread_cond_t cond;  // The signaling mechanism (The "Doorbell")
    int flag;             // Your status flag
} SharedData;

extern SharedData mesh_info;

struct commands_args
{
    SharedData *mesh_info;
};

void wait();

void resume();


#endif // THREAD_SAFETY_H