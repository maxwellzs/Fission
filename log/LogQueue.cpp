//
// Created by maxwellzs on 2024/6/15.
//

#include "LogQueue.h"

Fission::LogBlock::LogBlock(size_t blockSize)
:  rawSize(blockSize), rawData(new uint8_t[blockSize]) {

}

Fission::LogBlock::~LogBlock() {
    delete [] rawData;
}

void Fission::LogQueue::putData(uint8_t *dat, size_t size) {
    // make block and add to the queue
    LogBlock *newBlock = new LogBlock(size);
    static LogBlock *nullBlock = nullptr;
    LogBlock *originalFirst;
    newBlock->store(dat, size);
    while(true) {
        originalFirst = first;
        if(first.compare_exchange_weak(originalFirst, newBlock)) {
            if(originalFirst) {
                originalFirst->setNext(newBlock);
            } else {
                last.store(newBlock);
            }
            break;
        }
    }
}

Fission::LogBlock *Fission::LogQueue::yield() {
    LogBlock *originalLast = last;
    LogBlock *originalFirst = first;
    while(!last.compare_exchange_weak(originalLast, nullptr, std::memory_order_acq_rel))
        originalLast = last;
    while(!first.compare_exchange_weak(originalFirst,nullptr,std::memory_order_acq_rel)) {
        originalFirst = first;
    }
    if(originalFirst) originalFirst->setNext(nullptr);
    return originalLast;
}

