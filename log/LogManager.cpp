//
// Created by maxwellzs on 2024/6/21.
//

#include <stdexcept>
#include "LogManager.h"

Fission::LogManager *Fission::LogManager::instance = nullptr;

Fission::LogManager *Fission::LogManager::getInstance() {
    return instance ? instance : instance = new LogManager();
}

void Fission::LogManager::addLine(const std::string &text) {
    globalQueue.putData((uint8_t *)text.c_str(), text.length() + 1);
}

std::vector<std::string> Fission::LogManager::yield() {
    std::vector<std::string> res;
    for (LogBlock *i = globalQueue.yield(); i;) {

        res.emplace_back((char *) i->load());

        LogBlock *t = i;
        i = i->getNext();
        delete t;
    }
    return res;
}

Fission::LogManager::LogManager() {
    signal(SIGSEGV, [](int sig)->void{
        fprintf(stderr, "logger caught SIGSEGV \r\n");
        LogManager::getInstance()->shutdown();
        fflush(stderr);
    });

    logWorker = new std::thread([this]() ->void {
        fprintf(stdout, "logger thread started \r\n");
        while(!stopped) {
            flushLog(globalQueue.yield());
        }
        fprintf(stdout, "logger thread exited \r\n");
    });
}

Fission::LogManager::~LogManager() {
    stopped = true;
    logWorker->join();
    delete logWorker;

    // flush the remain log
    LogBlock* h = globalQueue.yield();
    while(h) {
        flushLog(h);
        h = globalQueue.yield();
    }

    // close each destination
    for(auto i : destinationList) delete i;
}


void Fission::LogManager::shutdown() {
    delete instance;
}

Fission::LogManager *Fission::LogManager::addDestination(Fission::LogDestination *newDest) {
    destinationList.push_back(newDest);
    return this;
}

void Fission::StdOut::writeLine(const std::string &str) {
    fprintf(stdout, "%s", str.c_str());
}

void Fission::StdOut::flush() {
    fflush(stdout);
}

Fission::FileOut::FileOut(const std::string &fileName) {
    dest = fopen(fileName.c_str(), "a");
    if(!dest) throw std::runtime_error("cannot open log files");
}

void Fission::FileOut::writeLine(const std::string &str) {
    fprintf(dest, "%s", str.c_str());
}

void Fission::FileOut::flush() {
    fflush(dest);
}

Fission::FileOut::~FileOut() {
    fclose(dest);
}
