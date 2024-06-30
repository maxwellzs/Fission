//
// Created by maxwellzs on 2024/6/27.
//

#ifndef FISSION_HAZARDLIST_H
#define FISSION_HAZARDLIST_H

#include <thread>
#include <vector>
#include <set>
#include <list>
#include <atomic>
#include <stdexcept>

namespace Fission {

    const size_t MAX_TOKEN = 128;
    const size_t RETIRE_MAX = 5;

    static std::atomic_size_t token_generator = 0;
    static thread_local size_t localToken = 0xffffffffffffffff;

    static size_t allocateToken() {
        size_t newToken = token_generator.fetch_add(1);
        if(newToken > MAX_TOKEN) throw std::runtime_error("out of token");
        return newToken;
    }


    class HazardList;

    struct HazardChunk {
        void* hold = nullptr;
        std::list<void *> retire;
    };


    class HazardList {
    private:
        std::vector<HazardChunk> chunkArray;
    public:

        HazardList();

        HazardChunk& getChunkAt(size_t token);
        std::vector<HazardChunk>& getChunkArray();
    };

    template<typename T>
    class HazardPointer {
    private:
        HazardList& list;
        T* p;

    public:
        HazardPointer(HazardList& list)
                : list(list) {
        }

        /**
         * automatically retire
         */
        ~HazardPointer() {
            if(p == nullptr) return;

            HazardChunk& pos = list.getChunkAt(localToken);
            pos.hold = nullptr;
            pos.retire.push_back(p);
            if(pos.retire.size() > RETIRE_MAX) {
                std::vector<HazardChunk>& chunks = list.getChunkArray();
                std::set<void *> currentHold;
                size_t m = token_generator.load();
                for (int i = 0; i < m; ++i) {
                    currentHold.insert(chunks[i].hold);
                }

                for (auto it = pos.retire.begin();
                it != pos.retire.end();
                it = pos.retire.erase(it)) {
                    if(!currentHold.count(*it)) {
                        delete (T*)*it;
                    }
                }
            }
        }

        __forceinline T* get() {
            return p;
        }

        bool acquire(std::atomic<T *>& ptr) {
            p = ptr.load();
            if(localToken > MAX_TOKEN) {
                localToken = allocateToken();
            }
            HazardChunk& pos = list.getChunkAt(localToken);
            pos.hold = p;
            // double check
            std::atomic_thread_fence(std::memory_order_acq_rel);
            if(p != ptr.load()) return false;

            return true;
        }

        __forceinline T* operator->() {
            return p;
        }

        __forceinline void relax() {
            p = nullptr;
        }
    };

};

#endif //FISSION_HAZARDLIST_H
