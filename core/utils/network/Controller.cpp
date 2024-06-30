//
// Created by maxwellzs on 2024/6/29.
//

#include "Controller.h"

Fission::Controller::Controller(const std::string &ip, uint16_t port)
: listen_ip(inet_addr(ip.c_str())), listen_port(port){
    base = event_base_new();
    if(!base) throw std::runtime_error("cannot create event base");


    createListenSocket();
}

void Fission::Controller::createListenSocket() {
    addr.sin_port = htons(listen_port);
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = listen_ip;

    listener = evconnlistener_new_bind(base, nullptr,
                                       this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                       36, (struct sockaddr*)&addr, sizeof(addr));

    if(!listener) throw std::runtime_error("cannot create listener!");

}

void Fission::Controller::onConnectRoutine(struct evconnlistener *listener, intptr_t fd, struct sockaddr *addr, int len,
                                           void *ptr) {
    Controller* c = (Controller*) ptr;
    sockaddr_in ad = *((sockaddr_in *)addr);
    Connection* connection = new Connection(ad);

    LOG(INFO) << "incoming connection from" << *connection;
}

Fission::Connection::Connection(const sockaddr_in &addr)
: dest_addr(addr) {
    std::stringstream builder;
    builder << inet_ntoa(dest_addr.sin_addr) << ":" << htons(dest_addr.sin_port);
    addr_string = builder.str();
}

Fission::Connection::~Connection() {
    bufferevent_free(bev);
}

void Fission::Connection::bindFileDescriptor(intptr_t fd) {

}

std::string Fission::Connection::toString() const {
    return addr_string;
}

bool Fission::Connection::operator<(const Fission::Object &obj) const {
    uint64_t u1 = (dest_addr.sin_addr.S_un.S_addr << 16) + dest_addr.sin_port;
    const Connection& c2 = reinterpret_cast<const Connection&>(obj);
    uint64_t u2 = (c2.dest_addr.sin_addr.S_un.S_addr << 16) + c2.dest_addr.sin_port;
    return u1 < u2;
}
