//
// Created by maxwellzs on 2024/6/21.
//

#ifndef FISSION_LOGGER_H
#define FISSION_LOGGER_H

#include <map>
#include <type_traits>
#include "core/Interface.h"
#include "log/LogManager.h"

#define LOG(SEV) if(SEV >= limit_severity )Fission::Logger(SEV)

namespace Fission {

    enum SEVERITY {
        VERBOSE     = 0,
        DEBUG       = 1,
        INFO        = 2,
        WARNING     = 3,
        FATAL       = 4
    };

    extern std::string time_format;
    extern SEVERITY limit_severity;

    /**
     * a logger is the impl for inputting a line of text to the destination
     * after the line is finished, the logger will append a return character
     * and deconstruct, appending the data to the global queue
     */
    class Logger : public Object {
    private:
        static std::map<SEVERITY, std::string> severityMap;
        SEVERITY current;

        std::string buf;

        void initialize();
    public:
        explicit Logger(SEVERITY s);
        ~Logger();

        __forceinline Logger& operator<<(const std::string& str) {
            buf += str;
            return *this;
        }

        __forceinline Logger& operator<<(const char str[]) {
            buf += std::string(str);
            return *this;
        }

        template<class ObjectType>
        __forceinline Logger& operator<<(const ObjectType& obj) {
            static_assert(std::is_base_of<Object, ObjectType>(), "printable object must be the a subclass of Object");
            buf += obj.toString();
            return *this;
        }

#define BASE_TYPE(T) \
        __forceinline Logger& operator<<(const T val) { \
            buf += std::to_string(val);           \
            return *this;                         \
        }

        BASE_TYPE(short)
        BASE_TYPE(int)
        BASE_TYPE(long)
        BASE_TYPE(unsigned short)
        BASE_TYPE(unsigned int)
        BASE_TYPE(unsigned long)
        BASE_TYPE(float)
        BASE_TYPE(double)
    };

};

#endif //FISSION_LOGGER_H
