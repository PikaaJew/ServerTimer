#ifndef SERVER_H
#define SERVER_H

#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>

inline uint32_t SLEEP_PERIODH = 48;

using time_clock_t = std::chrono::time_point<std::chrono::system_clock>;
using log_map_t = std::map<uint32_t, time_clock_t>;

enum Error {
    NO_ERROR,
    __ISONLINE__NOT_FOUND_SERVER,
    __READCSV__NOT_FOUND_SECTOR,
    __REPAIR__NOT_FOUND_SERVER,
    __DELETE__NOT_FOUND_SERVER,
    __SETNORESPONSE__NOT_FOUND_SERVER,
    __SETOFFLINE__NOT_FOUND_SERVER,
    __SETONLINE__NOT_FOUND_SERVER,
    __SETOFFLINE__FOUND_SERVER_IS_BROKEN,
    __SETONLINE__FOUND_SERVER_IS_BROKEN,
    __CHANGESEPARATOR__WRONG_SEPARATOR_CODE,
    __CHANGESLEEPPERIODH__WRONG_SLEEP_PERIOD,
    __ADDSERVER__SERVER_ALREADY_EXISTS,
    __RESTORE__NOT_FOUND_BACKUP
};

class Server
{
public:

    Server();

    ~Server();

    Server(uint32_t id, std::string info);

    uint32_t getId() const;

    std::string getInfo() const;

    void setInfo(const std::string &info);

    /**
     * makes a string of the object to be put into CSV file
    **/
    std::string toString() const;

    /**
     * checks if this server should be awaken
    **/
    bool isOnline(const log_map_t &logMap) const;

private:
    uint32_t mId;
    std::string mInfo;
};

#endif // SERVER_H
