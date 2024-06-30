//
// Created by maxwellzs on 2024/6/27.
//

#include "HazardList.h"

Fission::HazardList::HazardList()
: chunkArray(MAX_TOKEN) {

}

Fission::HazardChunk &Fission::HazardList::getChunkAt(size_t token) {
    return chunkArray[token];
}

std::vector<Fission::HazardChunk> &Fission::HazardList::getChunkArray() {
    return chunkArray;
}

