#include "server.hpp"

Server::Server() {

}

Server::~Server() {

}

Server::Server(uint32_t id, std::string info) : mId(id), mInfo(info) {

}

uint32_t Server::getId() const {
    return mId;
}

std::string Server::getInfo() const {
    return mInfo;
}

void Server::setInfo(const std::string &info) {
    mInfo = info;
}

std::string Server::toString() const {
    return std::to_string(mId) + mInfo;
}

bool Server::isOnline(const log_map_t &logMap) const {
    auto it = logMap.find(mId);
    try {
        if (it == logMap.end()) {
            throw __ISONLINE__NOT_FOUND_SERVER;
        } else {
            time_clock_t now = std::chrono::system_clock::now();
            return now - it->second > std::chrono::hours(SLEEP_PERIODH);
        }
    } catch (Error err) {
        std::wcout << L"Ошибка № " << err << L" Сервер с таким id не найден в логе" << std::endl;
    }
    return false;
}



