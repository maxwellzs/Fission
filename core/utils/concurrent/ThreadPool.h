//
// Created by maxwellzs on 2024/6/22.
//

#ifndef FISSION_THREADPOOL_H
#define FISSION_THREADPOOL_H

#include <vector>
#include <thread>
#include "core/utils/collections/ConcurrentQueue.h"
#include "log/Logger.h"
#include "core/Interface.h"

#ifndef __linux__
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Fission {

    static size_t getCoreCount();

    class ThreadPool;

    /**
     * the task that can be submitted to the executor
     */
    class Task : public Object {
    public:
        virtual ~Task() = default;
        virtual void execute(ThreadPool& parent) = 0;
    };

    /**
     * a thread pool is a concurrent task executor
     * a task can be submitted to the pool
     */
    class ThreadPool : public Object {
    private:
        std::vector<std::thread *> workerPool;

        bool stopped = false;
        size_t threads;
        ConcurrentQueue<Task *> taskQueue;

        void createWorker();
    public:
        ~ThreadPool();
        ThreadPool();

        void submit(Task* newTask);

        void join();
        void shutdown();
    };

};

#endif //FISSION_THREADPOOL_H
