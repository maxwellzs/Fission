//
// Created by maxwellzs on 2024/6/27.
//

#ifndef FISSION_CONCURRENTQUEUE_H
#define FISSION_CONCURRENTQUEUE_H

#include <atomic>
#include "HazardList.h"
#include "core/Interface.h"

namespace Fission {

    template<typename T>
    class ConcurrentQueue : public Object {
    private:

        template<typename Tp>
        struct Node {
            std::atomic<Node<Tp> *> next = nullptr;
            Tp v;
        };

        std::atomic<Node<T> *> head,tail;

        HazardList hl;

    public:
        ConcurrentQueue() {
            Node<T> *nil = new Node<T>;
            head.store(nil);
            tail.store(nil);
        }

        void pushBack(T val) {
            Node<T> *newNode = new Node<T>;
            newNode->v = val;
            Node<T> *p, *n = nullptr;
            while(true) {
                p = tail.load();
                if(p->next.load()) {
                    tail.compare_exchange_weak(p, p->next.load());
                    continue;
                }
                if(p->next.compare_exchange_weak(n, newNode)) break;
            }
            tail.compare_exchange_strong(p, newNode);
        }

        T popFront() {
            T v;
            Node<T> *p, *n;
            HazardPointer<Node<T>> ph(hl),pn(hl);
            while(true) {
                if(ph.acquire(head)) {
                   p = ph.get();
                   if(!pn.acquire(p->next)) {
                       continue;
                   }
                   n = pn.get();
                   if(n == nullptr) {
                       ph.relax();
                       pn.relax();
                       throw std::runtime_error("pop out of bound");
                   }
                   v = n->v;
                   if(head.compare_exchange_weak(p, n)) {
                       break;
                   }
               }
            }
            pn.relax();
            return v;
        }

    };

};

#endif //FISSION_CONCURRENTQUEUE_H
