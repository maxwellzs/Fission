//
// Created by maxwellzs on 2024/6/21.
//

#ifndef FISSION_INTERFACE_H
#define FISSION_INTERFACE_H

#include <string>

namespace Fission {

    class Object {
    public:
        __forceinline virtual std::string toString() const {
            return "[object at 0x" + std::to_string((uintptr_t)this) + "]";
        }

        __forceinline virtual bool operator<(const Object& obj) const {
            return this < &obj;
        }


    };

};

#endif //FISSION_INTERFACE_H