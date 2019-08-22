#ifndef MOVE_H
#define MOVE_H

#include "read_write.hpp"

class Methods {
public:
    static Methods& getInstance() {
        static Methods methods;
        return methods;
    }

    /**
     * changes the actual separator
    **/
    void changeSeparator(const uint32_t sep, std::string &title, array_vec_server_t &vecArray);

    /**
     * changes the period to sleep in hours
    **/
    void changeSleepPeriodH(const uint32_t h);

    /**
     * sets server offline
    **/
    void setOffline(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId);

    /**
     * sets servers online
    **/
    void setOnline(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId);

    /**
     * sets servers no response
    **/
    void setNoResponse(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId);

    /**
     * sets not responsing server online
    **/
    void repair(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId);

    /**
     * deletes server
    **/
    void deleteServer(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId);

    /**
     * calles sheduledOnline and sheduledOffline
    **/
    void manageOfflineOnline(array_vec_server_t &vecArray, const log_map_t &logMap);

    /**
     * updates CSV file
    **/
    void updateCSV(const std::string &CSVFilename, const std::string &title, array_vec_server_t &vecArray, const log_map_t &logMap);

    /**
     * updates log file
    **/
    void updateLog(const std::string &logFilename, const log_map_t &logMap);

    /**
     * writes actual info to the CSV and Log files
    **/
    void updateCSVandLOG(const std::string &CSVFilename, const std::string &logFilename, const std::string &title,
                         array_vec_server_t &vecArray, const log_map_t &logMap);

    /**
     * reads a server from cin and adds it to the selected vector
    **/
    void addServer(array_vec_server_t &vecArray, std::vector<Server> &toVec, const std::string &title, log_map_t &logMap, bool toSleeping);

    /**
     * prints actual status of the server
    **/
    void printServerStatus(const array_vec_server_t &vecArray, const std::vector<uint32_t> &vectorId);

    /**
     * updates info file
    **/
    void updateInfo(const std::string &infoFilename);

    /**
     * makes a backup CSV file
    **/
    void makeBackup(const std::string &CSVFilenameB, const std::string &logFilenameB, const std::string &infoFilename,
                    const std::string &title, array_vec_server_t &vecArray, const log_map_t &logMap);

    /**
     * restores CSV file from backup
    **/
    void restore(const std::string &CSVFilenameB, const std::string &logFilenameB, std::string &title, array_vec_server_t &vecArray, log_map_t &logMap);


    /**
     * prints the vector
    **/
    void printSector(const std::vector<Server> &vec, const log_map_t &logMap);

    /**
     * prints info of session of the program
    **/
    void printSessionInfo();

    /**
     * prints help
    **/
    void printHelp();


private:
    Methods() {}

    /**
     * moves a server from one vector to another
    **/
    inline void moveServer(std::vector<Server> &fromVec, std::vector<Server> &toVec, std::vector<Server>::iterator &it);

    /**
     * finds a server and moves it from one vector to another
    **/
    bool findAndMove(std::vector<Server> &fromVec, std::vector<Server> &toVec, log_map_t &logMap,
                     std::vector<uint32_t>::const_iterator &itId, time_clock_t setTime);

    /**
     * determines if server contains in the vector
    **/
    inline bool contains(const std::vector<Server> &vec, std::vector<uint32_t>::const_iterator &itId);

    /**
     * make servers online due to the sleep time period
    **/
    void manageOnline(array_vec_server_t &vecArray, const log_map_t &logMap);

    /**
     * make servers offline due to the changed sleep time period
    **/
    void manageOffline(array_vec_server_t &vecArray, const log_map_t &logMap);
};




#endif // MOVE_H
