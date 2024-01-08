#include "listener.h"

void print_int(int num)
{
    printf("%d\n", num);
}

void multiply(double **A, double **B, double **C, int size)
{
    printf("Start multiplying");
    for (int i = 0; i < size; ++i)
    {

        for (int k = 0; k < size; ++k)
        {
            for (int j = 0; j < size; ++j)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
Listener::Listener(const int number_of_descs)
{
    desc_number = number_of_descs;
    const char *fn1 = "fifo1";
    const char *fn2 = "fifo2";
    const char *fn3 = "fifo3";
    fd_tasks = new int[number_of_descs];
    fd_tasks[0] = open(fn1, O_RDWR);
    fd_tasks[1] = open(fn2, O_RDWR);
    fd_tasks[2] = open(fn3, O_RDWR);

    fds = new pollfd[number_of_descs];

    for (int i = 0; i < number_of_descs; ++i)
    {
        fds[i].fd = fd_tasks[i];
        fds[i].events = POLLIN;
    }
}

Listener::~Listener()
{
    for (int i = 0; i < desc_number; ++i)
    {
        close(fd_tasks[i]);
    }
}

void Listener::listen()
{
    printf("Main thread started to listen\n");
    bool stop = false;
    int buf_cnt = 0;
    while (true)
    {
        status = poll(fds, desc_number, -1);
        if (status == -1)
        {
            perror("poll");
        }
        if (!status)
        {
            printf("problem\n");
        }
        for (int i = 0; i < desc_number; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                read(fds[i].fd, buf, sizeof(buf));
                if (buf[0] == 'S')
                    stop = true;
                printf("%s\n", buf);
                ++buf_cnt;
                str = buf;
                printf("buffer was copied to the string\n");
                if (!stop)
                    threadpool->give_task(str);

                memset(buf, 0, sizeof(buf));
                printf("buffer was reseted\n");
            }
        }
        if (stop)
        {
            threadpool->done = true;
            break;
        }
    }
}
