//
// Created by maxwellzs on 2024/6/21.
//

#include <iostream>
#include "Logger.h"

std::string Fission::time_format;
Fission::SEVERITY Fission::limit_severity;

std::map<Fission::SEVERITY, std::string> Fission::Logger::severityMap = {
        {VERBOSE,   "VERBOSE"},
        {DEBUG,     "DEBUG  "},
        {INFO,      "INFO   "},
        {WARNING,   "WARNING"},
        {FATAL,     "FATAL  "}
};

Fission::Logger::Logger(Fission::SEVERITY s)
: current(s) {
    initialize();
}

Fission::Logger::~Logger() {
    buf += '\n';
    LogManager::getInstance()->addLine(buf);
}

void Fission::Logger::initialize() {
    time_t now = time(nullptr);
    tm* info = localtime(&now);
    size_t tmpLength = time_format.length() * 2 + 1;
    char *tmp = new char[tmpLength];
    memset(tmp, 0x0, tmpLength);
    strftime(tmp, tmpLength, time_format.c_str(), info);
    buf += "[";
    buf += tmp;
    buf += "][" + severityMap[current] + "]";
    buf += " - ";
}

