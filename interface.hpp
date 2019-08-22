#ifndef INTERFACE_H
#define INTERFACE_H

#include "methods.hpp"

class Interface {
public:
    static Interface& getInstance() {
        static Interface interface;
        return interface;
    }
    enum Command {
        UNDEFINED = 0,
        SET_ONLINE,
        SET_OFFLINE,
        SET_NO_RESPONSE,
        REPAIR,
        CHANGE_SEPARATOR,
        CHANGE_SLEEP_PER,
        UPDATE,
        ADD_SERVER_ONLINE,
        ADD_SERVER_OFFLINE,
        ADD_SERVER_NO_RESPONSE,
        DELETE_SERVER,
        SERVER_STATUS,
        ONLINE_LIST,
        OFFLINE_LIST,
        NO_RESPONSE_LIST,
        INFO,
        BACKUP,
        RESTORE,
        HELP,
        EXIT
    };

    bool interface();

private:
    Interface() {}

    std::vector<uint32_t> extractIntegerWords(const std::string &line);

    std::pair<Command, std::vector<uint32_t>> parseCommand(const std::string &line);

    void readAndExecuteCommand(const std::string &CSVFilename, const std::string &logFilename,
                               const std::string &infoFilename, array_vec_server_t &vecArray,
                               log_map_t &logMap, std::string &title);
};


#endif // INTERFACE_H
