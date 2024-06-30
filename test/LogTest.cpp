//
// Created by maxwellzs on 2024/6/15.
//

#include <thread>
#include <gtest/gtest.h>
#include "log/LogQueue.h"
#include "log/Logger.h"

using namespace Fission;


TEST(LoggerTest, TestLoggingQueue) {
    LogQueue lq;
    bool quit = false;
    std::atomic_int cnt = 0;
    std::atomic_int latch = 0;
    int scale = 1e7;
    int threads = 5;
    const std::string msg = "hello world";
    // consumer
    std::vector<std::thread *> pool;
    pool.push_back(new std::thread([&]() -> void {
        while(true) {
            LogBlock* b = lq.yield();
            if(!b && latch == threads) break;
            int c = 0;
            for (LogBlock* i = b; i != nullptr ;) {
                cnt.fetch_add(1);
                LogBlock* tmp = i;
                i = i->getNext();
                delete tmp;
                c++;
            }
            std::cout << "retrieved  " << c << " block(s)" << std::endl;
        }
    }));
    for (int i = 0; i < threads; ++i) {
        pool.push_back(new std::thread([&]()->void {
            for (int j = 0; j < scale; ++j) {
                lq.putData((uint8_t *) msg.c_str(), msg.length() + 1);
            }
            latch.fetch_add(1);
        }));
    }

    for (auto i: pool) {
        i->join();
        delete i;
    }

    EXPECT_EQ(cnt.load() , scale * threads);
}

TEST(LoggerTest, TestPrintObject) {
    LogManager::getInstance()->addDestination(new StdOut)
                            ->addDestination(new FileOut("log.txt"));
    time_format = "%Y-%m-%d|%H:%M:%S";
    limit_severity = DEBUG;

    Object o;
    ((Object *)nullptr)->toString();

    LOG(INFO) << o << 1.1 << 120;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG(INFO) << o << "hello world";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG(INFO) << o;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    LogManager::getInstance()->shutdown();
}