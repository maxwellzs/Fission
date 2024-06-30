//
// Created by maxwellzs on 2024/6/22.
//

#include <thread>

#include <gtest/gtest.h>
#include <stack>

#include "core/utils/collections/ConcurrentQueue.h"
#include "core/utils/concurrent/ThreadPool.h"

TEST(BaseTest, TestConcurrentQueue) {

    using namespace Fission;

    size_t scale = 1e7;
    size_t threads = 3;
    std::vector<std::thread *> pool;
    std::atomic_size_t cnt = 0;
    std::atomic_size_t latch = 0;

    ConcurrentQueue<size_t> q;

    /*
     * providers
     */
    for (int i = 0; i < threads; ++i) {
        pool.push_back(new std::thread([&]()-> void {
            for (int j = 1; j <= scale; ++j) {
                q.pushBack(j);
            }
            std::cout << "provider exited" << std::endl;
            latch.fetch_add(1);
        }));
    }

    /*
     * consumers
     */
    for (int i = 0; i < threads; ++i) {
        pool.push_back(new std::thread([&]()-> void {
            while(true) {
                try {
                    size_t n = q.popFront();
                    cnt.fetch_add(n);
                }catch (std::exception& e) {
                    if(latch.load() == threads) break;
                }
            }
        }));
    }

    for(auto i : pool) {
        i->join();
        delete i;
    }

    EXPECT_EQ(cnt.load(), ((scale*scale + scale) >> 1) * threads);
}

TEST(BaseTest, TestThreadPool) {
    using namespace Fission;
    LogManager::getInstance()->addDestination(new StdOut)
            ->addDestination(new FileOut("log.txt"));
    time_format = "%Y-%m-%d|%H:%M:%S";
    limit_severity = DEBUG;

    std::atomic_size_t cnt = 0;

    ThreadPool p;
    class TestTask : public Task{
    private:
        std::atomic_size_t& c;
    public:
        TestTask(std::atomic_size_t & c) : c(c) {
        }

        void execute(ThreadPool &parent) override {
            LOG(INFO) << "task executed";
            c.fetch_add(1);
        }
    };

    for (int i = 0; i < 1000; ++i) {
        p.submit(new TestTask(cnt));
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_EQ(cnt.load(), 1000);
}