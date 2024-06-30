//
// Created by maxwellzs on 2024/6/30.
//

#ifndef FISSION_BUFFERREADER_H
#define FISSION_BUFFERREADER_H

#include <cstdint>
#include <event2/bufferevent.h>

namespace Fission {

    enum ReaderState {
        READER_STATE_IDLE,
        READER_STATE_START,
        READER_STATE_FINISHED
    };

    class BufferReader {
    private:
        uint8_t *data_buf = nullptr;
        size_t buf_size = 1024; // initial size

        size_t writeIndex = 0;

        ReaderState currentState = READER_STATE_IDLE;
    public:
        BufferReader();


    };

};

#endif //FISSION_BUFFERREADER_H
