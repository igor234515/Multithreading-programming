#include "threadpool.h"
void print_much()
{
    std::cout<< "PRINT TASK\n";
}

Threadpool::Threadpool(const int number_of_threads) : done(false),
                                                      thread_number(number_of_threads),
                                                      task_queues(number_of_threads),
                                                      cvs(number_of_threads)
{

    for (int i = 0; i < number_of_threads; ++i)
    {
        threads.push_back(
            std::thread(&Threadpool::doWork, this, i));
        printf("thread %d added to the pool\n", i);
    }
}

Threadpool::~Threadpool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        done = true;
    }
    for (int i = 0; i < thread_number; ++i)
    {
        cvs[i].notify_all();
    }
    for (int i = 0; i < thread_number; ++i)
    {
        threads[i].join();
    }
}
void Threadpool::doWork(int threadID)
{

    while (!done)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cvs[threadID].wait(lock, [this, threadID]
                           { return !task_queues[threadID].empty() || done || task_sum > 0; });

            if (!task_queues[threadID].empty())
            {
                task = std::move(task_queues[threadID].front());
                task_queues[threadID].pop();
                task_sum -= 1;
            }
        }

        if (task)
        {
            task();
        }
        else
        {
            steal_work(threadID);
        }
    }
}
void Threadpool::steal_work(int thiefID)
{
    size_t victim_id_1 = random_thread();
    size_t victim_id_2 = random_thread();
    size_t victim_id = 0;

    while (victim_id_1 == victim_id_2)
    {
        victim_id_2 = random_thread();
    }

    std::unique_lock<std::mutex> lock(queue_mutex);
    if (task_queues[victim_id_1].size() > task_queues[victim_id_2].size())
    {
        victim_id = victim_id_1;
    }
    else
    {
        victim_id = victim_id_2;
    }

    if (!task_queues[victim_id].empty())
    {
        auto stolen_task = std::move(task_queues[victim_id].front());
        task_queues[victim_id].pop();
        task_queues[thiefID].push(stolen_task);
        std::cout << "Thread " << thiefID << " stole a task from Thread " << victim_id << "\n---\n";
    }
}
int Threadpool::random_thread()
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, thread_number-1);
    int ret = distribution(generator);
    return ret;
}
void Threadpool::  give_task(std::string parameter)
{
    int thread_1 = random_thread();
    int thread_2 = random_thread();
    while (thread_1 == thread_2)
    {
        thread_2 = random_thread();
    }

    int thread_give;

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (task_queues[thread_1].size() < task_queues[thread_2].size())
        {
            thread_give = thread_1;
        }
        else
        {
            thread_give = thread_2;
        }
        if (parameter == "HARD\n")
        {
            task_queues[thread_give].push([&]
                                          { print_much(); });
        }
        printf("Task was given to %d thread\n", thread_give);
        task_sum += 1;
    }
    cvs[thread_give].notify_one();//wait with timeout
}
