#include "../headers/threadSafety.h"

SharedData mesh_info = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .flag = 0};

void wait()
{
    pthread_mutex_lock(&mesh_info.lock);
    while (mesh_info.flag == 0)
    {
        pthread_cond_wait(&mesh_info.cond, &mesh_info.lock);
    }
    mesh_info.flag = 0; // Reset the flag for the next command
    pthread_mutex_unlock(&mesh_info.lock);
}

void resume()
{
    pthread_mutex_lock(&mesh_info.lock);
    mesh_info.flag = 1;
    pthread_cond_signal(&mesh_info.cond);
    pthread_mutex_unlock(&mesh_info.lock);
}