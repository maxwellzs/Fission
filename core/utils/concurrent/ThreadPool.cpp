//
// Created by maxwellzs on 2024/6/22.
//

#include "ThreadPool.h"

Fission::ThreadPool::ThreadPool()
: threads(getCoreCount()) {
    createWorker();
}

void Fission::ThreadPool::createWorker() {
    for (int i = 0; i < threads; ++i) {
        workerPool.push_back(new std::thread([this]()->void {
            while(!stopped) {
                Task* task;
                try {
                    task = taskQueue.popFront();
                }catch (std::runtime_error& e) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
                try {
                    task->execute(*this);
                }catch (std::exception& e) {
                    LOG(WARNING) << "uncaught exception in thread pool : " << e.what();
                }
                delete task;
            }
        }));
    }
}

void Fission::ThreadPool::submit(Fission::Task *newTask) {
    taskQueue.pushBack(newTask);
}

void Fission::ThreadPool::join() {
    for (auto t: workerPool) {
        t->join();
    }
}

void Fission::ThreadPool::shutdown() {
    stopped = true;
    join();
}

Fission::ThreadPool::~ThreadPool() {
    shutdown();
    for (auto t : workerPool) {
        delete t;
    }
    LOG(INFO) << "pool shutting down";
}

size_t Fission::getCoreCount() {
#ifndef __linux__
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCS_CONF);
#endif
}
