//
// Created by maxwellzs on 2024/6/29.
//

#ifndef FISSION_CONTROLLER_H
#define FISSION_CONTROLLER_H

#include <stdexcept>
#include <cstdint>
#include <sstream>
#include <set>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include "core/Interface.h"
#include "core/utils/network/BufferReader.h"
#include "log/Logger.h"

#ifndef __linux__
#include <winsock2.h>
#else

#endif

namespace Fission {

    class Connection : public Object {
    private:
        sockaddr_in dest_addr;
        std::string addr_string;

        bufferevent* bev = nullptr;
    public:
        std::string toString() const override;

        ~Connection();
        Connection(const sockaddr_in &addr);

        void bindFileDescriptor(evutil_socket_t fd);

        bool operator<(const Object &obj) const override;
    };

    struct ConnectionSortCriterion : public Object {
        bool operator() (const Connection* c1, const Connection* c2) const {
           return *c1 < *c2;
        }
    };

    class Controller : public Object {
    private:
        uint32_t listen_ip;
        uint16_t listen_port;
        sockaddr_in addr;

        event_base* base;
        evconnlistener* listener;

        std::set<Connection *, ConnectionSortCriterion> connection_pool;

        void createListenSocket();

        static void onConnectRoutine(struct evconnlistener *listener,
                                     evutil_socket_t fd,
                                     struct sockaddr *addr,
                                     int len, void *ptr);
    public:
        /**
         * create the controller on the given port
         * @param ip the ip address
         * @param port the port
         */
        Controller(const std::string& ip, uint16_t port);
    };

};

#endif //FISSION_CONTROLLER_H
