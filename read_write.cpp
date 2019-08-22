#include "read_write.hpp"


void ReadWrite::createLogFromVector(std::ofstream &logFile, std::vector<Server> vecServer) {
    ReadWrite readWrite = readWrite.getInstance();
    std::chrono::time_point<std::chrono::system_clock> past = std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH());
    std::time_t past_time = std::chrono::system_clock::to_time_t(past);
    for (auto& it : vecServer) {
        logFile << std::to_string(it.getId()) << '\t' << std::ctime(&past_time);
    }
}

void ReadWrite::createLog(const std::string &logFilename, array_vec_server_t &vecArray) {
    std::ofstream logFile(logFilename);
    for (auto vec: vecArray)
    {
        createLogFromVector(logFile, vec);
    }
    logFile.close();
}

void ReadWrite::readLog(std::ifstream &logFile, log_map_t &logMap) {
    std::string line;
    while (std::getline(logFile, line)) {
        auto it = std::find_if(line.begin(), line.end(), [](char c) {return c == '\t';});
        std::string stringId(line.begin(), it);
        std::string timeString(it + 1, line.end());

        std::tm tm = {};
        std::stringstream ss(timeString);
        ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
        auto beginSleepTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        logMap[static_cast<uint32_t>(std::stoi(stringId))] = beginSleepTime;
    }
}

Server ReadWrite::getServerFromString(const std::string &line) {
    ReadWrite readWrite = readWrite.getInstance();
    auto it =  std::find(line.begin(), line.end(), readWrite.getSeparator());
    auto pos = static_cast<uint32_t>(it - line.begin());
    std::string stringId = line.substr(0, pos);
    std::string info = line.substr(pos, line.size() - pos);
    return Server(static_cast<uint32_t>(std::stoi(stringId)), info);
}

std::pair<char, uint8_t> ReadWrite::separatorFindCount(const std::string &title) {
    auto it = std::find_if(title.begin(), title.end(), [](char c) {return c == ',' || c == ';';});
    uint8_t separatorCounter = static_cast<uint8_t>(std::count(title.begin(), title.end(), *it));
    return std::pair<char, uint8_t>(*it, separatorCounter);
}

std::string ReadWrite::createSectorNameStringsToRD(const std::string &title, std::string &rdy, std::string &slpng, std::string &brkn) {
    ReadWrite readWrite = readWrite.getInstance();
    auto pair = separatorFindCount(title);
    readWrite.setSeparator(pair.first);
    uint8_t separatorCounter = pair.second;
    rdy = "Ready to use";
    slpng = "Sleeping";
    brkn = "Broken";
    std::string sepString(separatorCounter, readWrite.getSeparator());
    rdy.append(sepString);
    slpng.append(sepString);
    brkn.append(sepString);
    return sepString;
}

void ReadWrite::createSectorNameStringsToWR(const std::string &title, std::string &rdy, std::string &slpng, std::string &brkn) {
    createSectorNameStringsToRD(title, rdy, slpng, brkn);
    rdy.append(1u, '\n');
    slpng.append(1u, '\n');
    brkn.append(1u, '\n');
}

void ReadWrite::readCSV(std::ifstream &CSVFile, array_vec_server_t &vecArray, std::string &title) {
    std::getline(CSVFile, title);
    std::string rdy, slpng, brkn, empty;
    empty = createSectorNameStringsToRD(title, rdy, slpng, brkn);

    std::string line;
    ServerSector serverSector = UNKNOWN;
    while (std::getline(CSVFile, line)) {
        if (line == rdy) {
            serverSector = ONLINE;
            continue;
        } else if (line == slpng) {
            serverSector = OFFLINE;
            continue;
        } else if (line == brkn) {
            serverSector = NO_RESPONCE;
            continue;
        } else if (line == empty) {
            continue;
        }

        if (serverSector >= ONLINE) {
            vecArray[static_cast<uint32_t>(serverSector)].push_back(getServerFromString(line));
        }
    }

    try {
        if (serverSector == UNKNOWN) {
            throw("готовых к работе");
        } else if (serverSector == ONLINE) {
            throw("спящих");
        } else if (serverSector == OFFLINE) {
            throw("сломанных");
        }
    } catch (std::wstring err) {
        std::wcout << L"Ошибка №" << __READCSV__NOT_FOUND_SECTOR << L", отсутствует или неверное название сегмента " << err << L" серверов" << std::endl;
    }
}

void ReadWrite::createInfo(const std::string &infoFilename) {
    ReadWrite readWrite = readWrite.getInstance();
    std::ofstream infoFile(infoFilename);
    std::time_t backupTime = std::chrono::system_clock::to_time_t(readWrite.getBackUpTime());
    infoFile << std::ctime(&backupTime) << SLEEP_PERIODH;
    infoFile.close();
}

void ReadWrite::readInfo(std::ifstream &infoFile) {
    ReadWrite readWrite = readWrite.getInstance();
    std::string timeString;
    std::getline(infoFile, timeString);
    std::tm tm = {};
    std::stringstream ss(timeString);
    ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
    readWrite.setBackUpTime(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
    std::string sleepPeriod;
    std::getline(infoFile, sleepPeriod);
    SLEEP_PERIODH = static_cast<uint32_t>(std::stoi(sleepPeriod));
}

void ReadWrite::writeInfo(std::ofstream &infoFile) {
    ReadWrite readWrite = readWrite.getInstance();
    std::time_t backupTime = std::chrono::system_clock::to_time_t(readWrite.getBackUpTime());
    infoFile << std::ctime(&backupTime) << SLEEP_PERIODH;
}

void ReadWrite::writeLog(std::ofstream &logFile, const log_map_t &logMap) {
    for (auto serverTime : logMap) {
        auto beginSleepTime = std::chrono::system_clock::to_time_t(serverTime.second);
        logFile << std::to_string(serverTime.first) << '\t' << std::ctime(&beginSleepTime);
    }
}

void ReadWrite::writeSector(std::ofstream &CSVFile, const std::vector<Server> &vecServer, const std::string &sectorName) {
    CSVFile << sectorName;
    for (auto server : vecServer) {
        CSVFile << server.toString() << '\n';
    }
}

void ReadWrite::writeCSV(std::ofstream &CSVFile, array_vec_server_t &vecArray, const std::string &title, const log_map_t &logMap) {
    CSVFile << title + '\n';
    std::array<std::string, 3> sectorNames;
    createSectorNameStringsToWR(title, sectorNames[0], sectorNames[1], sectorNames[2]);
    for (uint8_t i = 0; i < 3; i++) {
        std::sort(vecArray[i].begin(), vecArray[i].end(), [&](const Server &server1, const Server &server2){
            return (logMap.find(server1.getId())->second < logMap.find(server2.getId())->second) ||
                    (logMap.find(server1.getId())->second == logMap.find
                     (server2.getId())->second && server1.getId() < server2.getId());
        });
        writeSector(CSVFile, vecArray[i], sectorNames[i]);
    }
}

char ReadWrite::getSeparator() {
    return separator;
}

void ReadWrite::setSeparator(char sep) {
    separator = sep;
}

uint32_t ReadWrite::getDefaultPastTimeH() {
    return defaultPastTimeH;
}

void ReadWrite::setDefaultPastTimeH(uint32_t pastTime) {
    defaultPastTimeH = pastTime;
}

time_clock_t ReadWrite::getBackUpTime() {
    return backUpTime;
}

void ReadWrite::setBackUpTime(time_clock_t backTime) {
    backUpTime = backTime;
}



