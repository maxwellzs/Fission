//
// Created by maxwellzs on 2024/6/15.
//

#ifndef FISSION_LOGQUEUE_H
#define FISSION_LOGQUEUE_H

#include <atomic>
#include <cstring>
#include <core/Interface.h>

namespace Fission {

    /**
     * a log block is a text block that holds
     * a segment of data
     */
    class LogBlock : public Object {
    private:
        LogBlock* next = nullptr;
        uint8_t* rawData;
        size_t rawSize = 0;
    public:
        /**
         * create a log block with the size of @param blockSize
         * @param blockSize the create size
         */
        LogBlock(size_t blockSize);
        ~LogBlock();

        __forceinline void setNext(LogBlock *newNext) {
            next = newNext;
        }

        __forceinline void store(uint8_t *dat, size_t s) {
            memcpy(rawData, dat, s);
        }

        __forceinline uint8_t *load() {
            return rawData;
        }

        __forceinline LogBlock* getNext() {
            return next;
        }
    };

    /**
     * a log queue is a queue that store block
     * this queue provide synchronized methods for data storage
     */
    class LogQueue : public Object {
    private:
        std::atomic<LogBlock *> first = nullptr, last = nullptr;

    public:
        void putData(uint8_t *dat, size_t size);
        LogBlock* yield();


    };

};

#endif //FISSION_LOGQUEUE_H
