//
// Created by maxwellzs on 2024/6/21.
//

#ifndef FISSION_LOGMANAGER_H
#define FISSION_LOGMANAGER_H

#include <vector>
#include <thread>
#include <memory>
#include "core/Interface.h"
#include "log/LogQueue.h"

namespace Fission {

    class LogDestination : public Object {
    public:
        virtual void writeLine(const std::string& str) = 0;

        virtual void flush() = 0;
    };

    class StdOut : public LogDestination {
    public:
        void flush() override;

        void writeLine(const std::string &str) override;
    };

    class FileOut : public LogDestination {
    private:
        FILE* dest;

        ~FileOut();
    public:
        FileOut(const std::string& fileName);

        void flush() override;

        void writeLine(const std::string &str) override;
    };

    class LogManager : public Object {
    private:
        static LogManager *instance;
        bool stopped = false;
        std::thread *logWorker;
        std::vector<LogDestination *> destinationList;

        LogQueue globalQueue;

        __forceinline void flushLog(LogBlock* firstBlock) {
            if(!firstBlock) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                return;
            }
            for (LogBlock *i = firstBlock; i; ) {
                for (auto j: destinationList) {
                    if(j) j->writeLine(std::string((char *)i->load()));
                }

                for (auto j : destinationList)
                    if(j) j->flush();

                auto temp = i;
                i = i->getNext();
                delete temp;
            }
        }

        LogManager();
        ~LogManager();
    public:
        static LogManager* getInstance();

        void shutdown();
        void addLine(const std::string &text);
        LogManager* addDestination(LogDestination *newDest);

        std::vector<std::string> yield();
    };

};

#endif //FISSION_LOGMANAGER_H
