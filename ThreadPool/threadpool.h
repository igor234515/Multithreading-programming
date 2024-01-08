#pragma once
#ifndef THRDPOOL_H
#define THRDPOOL_H
#include "thread_safe_queue.h"
#include <thread>
#include <vector>
#include <functional>
#include <random>
#include <chrono>
#include <condition_variable>

class Threadpool
{
private:
    std::vector<std::queue<std::function<void()>>> task_queues;
    std::vector<std::thread> threads;
    std::mutex queue_mutex;
    std::vector<std::condition_variable> cvs;
    int task_sum;
    int thread_number;
    void doWork(int threadID);
    void steal_work(int thiefID);
    int random_thread();

public:
    bool done;
    void give_task(std::string parameter);
    Threadpool(const int number_of_threads);
    ~Threadpool();
};

#endif
