#ifndef LISTNR_H
#define LISTNR_H

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

class Listener
{
private:
    int status;
    char buf[10];
    std::string str;
    int* fd_tasks;
    int desc_number;
    struct pollfd *fds;
    bool up_data_flag;
    Threadpool *threadpool;

    
public:
    Listener(const int number_of_descs);
    void listen();
    void attachThreadpool(Threadpool *thrdp){threadpool = thrdp;}
    ~Listener();
};

#endif