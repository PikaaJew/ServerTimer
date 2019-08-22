#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <vector>
#include "server.hpp"

/**
 * [0] = vector of ready servers; [1] = vector of sleeping servers; [2] = vector of broken servers
**/
using array_vec_server_t = std::array<std::vector<Server>, 3>;

enum ServerSector {
    UNKNOWN = -1,
    ONLINE,
    OFFLINE,
    NO_RESPONCE
};

class ReadWrite {
public:
    static ReadWrite& getInstance() {
        static ReadWrite readWrite;
        return readWrite;
    }

    /**
     * creates log file
    **/
    void createLog(const std::string &logFilename, array_vec_server_t &vecArray);

    /**
     * reads the log into map
    **/
    void readLog(std::ifstream &logFile, log_map_t &logMap);

    /**
     * gets a server object from a string
    **/
    Server getServerFromString(const std::string &line);

    /**
     * creates sector names strings to read
    **/
    std::string createSectorNameStringsToRD(const std::string &title, std::string &rdy, std::string &slpng, std::string &brkn);

    /**
     * creates sector names strings to write
    **/
    void createSectorNameStringsToWR(const std::string &title, std::string &rdy, std::string &slpng, std::string &brkn);

    /**
     * reads CSV file into array of Servers and title
    **/
    void readCSV(std::ifstream &CSVFile, array_vec_server_t &vecArray, std::string &title);

    /**
     * creates info file
    **/
    void createInfo(const std::string &infoFilename);

    /**
     * reads info file
    **/
    void readInfo(std::ifstream &infoFile);

    /**
     * writes info file
    **/
    void writeInfo(std::ofstream &infoFile);

    /**
     * writes map into log file
    **/
    void writeLog(std::ofstream &logFile, const log_map_t &logMap);

    /**
     * writes vector of servers into CSV file
    **/
    void writeCSV(std::ofstream &CSVFile, array_vec_server_t &vecArray, const std::string &title, const log_map_t &logMap);

    static char getSeparator();

    static void setSeparator(char sep);

    static uint32_t getDefaultPastTimeH();

    static void setDefaultPastTimeH(uint32_t pastTime);

    static time_clock_t getBackUpTime();

    static void setBackUpTime(time_clock_t backTime);


private:
    ReadWrite() {}

    void createLogFromVector(std::ofstream &logFile, std::vector<Server> vecServer);

    /**
     * determines the separators in title string and then counts all of them
    **/
    std::pair<char, uint8_t> separatorFindCount(const std::string &title);

    void writeSector(std::ofstream &CSVFile, const std::vector<Server> &vecServer, const std::string &sectorName);

    static char separator;
    static uint32_t defaultPastTimeH;
    static time_clock_t backUpTime;

};




#endif // READ_WRITE_H
