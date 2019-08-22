 #include "methods.hpp"

void Methods::changeSeparator(const uint32_t sep, std::string &title, array_vec_server_t &vecArray) {
    ReadWrite readWrite = readWrite.getInstance();
    try {
        if (sep != 0 && sep != 1) {
            throw(__CHANGESEPARATOR__WRONG_SEPARATOR_CODE);
        } else {
            readWrite.setSeparator((sep == 0) ? ';' : ',');
            std::for_each(title.begin(), title.end(), [=](char& c)
                {ReadWrite readWrite = readWrite.getInstance();
                if ((c == ',' || c == ';') && c != readWrite.getSeparator()) {
                    c = readWrite.getSeparator();
                }});
            for (auto &serverVec : vecArray) {
                for (auto &server : serverVec) {
                    std::string info = server.getInfo();
                    std::for_each(info.begin(), info.end(), [=](char& c)
                        {ReadWrite readWrite = readWrite.getInstance();
                        if ((c == ',' || c == ';') && c != readWrite.getSeparator()) {
                            c = readWrite.getSeparator();
                        }});
                    server.setInfo(info);
                }
            }
            std::wcout << L"Разделитель был изменён на " << readWrite.getSeparator() << std::endl;
        }
    } catch (Error err) {
        std::wcout << L"Ошибка № " << err << L" Неверный код разделителя" << std::endl;
    }
}

void Methods::changeSleepPeriodH(const uint32_t h) {
    ReadWrite readWrite = readWrite.getInstance();
    try {
        if (h > readWrite.getDefaultPastTimeH()) {
            throw(__CHANGESLEEPPERIODH__WRONG_SLEEP_PERIOD);
        } else {
            SLEEP_PERIODH = h;
        }
    } catch (Error err) {
        std::wcout << L"Ошибка № " << err << L", количество часов для сна, большее " << readWrite.getDefaultPastTimeH() << std::endl;
    }    
}

inline void Methods::moveServer(std::vector<Server> &fromVec, std::vector<Server> &toVec, std::vector<Server>::iterator &it) {
    toVec.push_back(*it);
    fromVec.erase(it);
}

bool Methods::findAndMove(std::vector<Server> &fromVec, std::vector<Server> &toVec, log_map_t &logMap,
                 std::vector<uint32_t>::const_iterator &itId, time_clock_t setTime) {
    bool moved = false;
    auto itServ = std::find_if(fromVec.begin(), fromVec.end(), [=](Server serv) {return serv.getId() == *itId;});
    if (itServ != fromVec.end()) {
        moveServer(fromVec, toVec, itServ);
        logMap[*itId] = setTime;
        moved = true;
    }
    return moved;
}

inline bool Methods::contains(const std::vector<Server> &vec, std::vector<uint32_t>::const_iterator &itId) {
    bool found = false;
    auto itServ = std::find_if(vec.begin(), vec.end(), [=](Server serv) {return serv.getId() == *itId;});
    if (itServ != vec.end()) {
        found = true;
    }
    return found;
}

void Methods::setOffline(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId) {
    for (auto itId = vectorId.begin(); itId != vectorId.end(); itId++) {        //ASK
        bool movedFromRDY = findAndMove(vecArray[0], vecArray[1], logMap, itId, std::chrono::system_clock::now());
        bool foundInBRKN = contains(vecArray[2], itId);
        try {
            if (!movedFromRDY && !foundInBRKN) {
                throw(__SETOFFLINE__NOT_FOUND_SERVER);
            } else if (foundInBRKN) {
                throw(__SETOFFLINE__FOUND_SERVER_IS_BROKEN);
            } else if (movedFromRDY) {
                std::wcout << L"Сервер №" << *itId << L" был отправлен в сон" << std::endl;
            }
        } catch (Error err) {
            switch (err) {
            case __SETOFFLINE__NOT_FOUND_SERVER:
                std::wcout << L"Ошибка №" << err << L", сервер №" << *itId << L" не найден в рабочих и сломанных секторах" << std::endl;
                break;
            case __SETOFFLINE__FOUND_SERVER_IS_BROKEN:
                std::wcout << L"Ошибка № " << err << L", в списке есть сломанный сервер № " << *itId
                          << L", он не был перемещён. Для его перемещения используйте соответствующую команду" << std::endl;
                break;
            default:
                break;
            }
        }
    }
}

void Methods::setOnline(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId) {
    ReadWrite readWrite = readWrite.getInstance();
    for (auto itId = vectorId.begin(); itId != vectorId.end(); itId++) {
        bool movedFromSLP = findAndMove(vecArray[1], vecArray[0], logMap, itId, std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH()));
        bool foundInBRKN = contains(vecArray[2], itId);

        try {
            if (!movedFromSLP && !foundInBRKN) {
                throw(__SETONLINE__NOT_FOUND_SERVER);
            } else if (foundInBRKN) {
                throw(__SETONLINE__FOUND_SERVER_IS_BROKEN);
            } else if (movedFromSLP) {
                std::wcout << L"Сервер №" << *itId << L" был перемещён из спящих серверов в готовые" << std::endl;
            }
        } catch (Error err) {
            switch (err) {
            case __SETONLINE__NOT_FOUND_SERVER:
                std::wcout << L"Ошибка № " << err << L", сервер №" << *itId << L" не найден в спящих и сломанных секторах" << std::endl;
                break;
            case __SETONLINE__FOUND_SERVER_IS_BROKEN:
                std::wcout << L"Ошибка № " << err << L", в списке есть сломанный сервер №" << *itId
                          << L", он не был перемещён. Для его перемещения используйте соответствующую команду" << std::endl;
                break;
            default:
                break;
            }
        }
    }
}

void Methods::setNoResponse(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId) {
    ReadWrite readWrite = readWrite.getInstance();
    for (auto itId = vectorId.begin(); itId != vectorId.end(); itId++) {
        bool movedFromRDY = findAndMove(vecArray[0], vecArray[2], logMap, itId, std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH()));
        bool movedFromSLP = findAndMove(vecArray[1], vecArray[2], logMap, itId, std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH()));
        try {
            if (!movedFromRDY && !movedFromSLP) {
                throw(__SETNORESPONSE__NOT_FOUND_SERVER);
            } else if (movedFromRDY) {
                std::wcout << L"Сервер № " << *itId << L" был перемещён из готовых серверов в сломанные" << std::endl;
            } else if (movedFromSLP) {
                std::wcout << L"Сервер № " << *itId << L" был перемещён из спящих серверов в сломанные" << std::endl;
            }
            //TODO
        } catch (Error err) {
            std::wcout << L"Ошибка № " << err << L", сервер №" << *itId << L" не найден в рабочих и спящих секторах" << std::endl;
        }
    }
}

void Methods::repair(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId) {
    ReadWrite readWrite = readWrite.getInstance();
    for (auto itId = vectorId.begin(); itId != vectorId.end(); itId++) {
        bool wasRepaired = findAndMove(vecArray[2], vecArray[0], logMap, itId, std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH()));
        try {
            if (!wasRepaired) {
                throw(__REPAIR__NOT_FOUND_SERVER);
            } else {
               std::wcout << L"Сервер № " << *itId << L" был перемещён из сломанных серверов в готовые" << std::endl;
            }
        } catch (Error err) {
            std::wcout << L"Ошибка № " << err << L", сервер №" << *itId << L" не найден в сломанных серверах" << std::endl;
        }
    }
}

void Methods::deleteServer(array_vec_server_t &vecArray, log_map_t &logMap, const std::vector<uint32_t> &vectorId) {
    for (auto id : vectorId) {
        auto itServ = std::find_if(vecArray[0].begin(), vecArray[0].end(), [=](Server serv) {return serv.getId() == id;});
        ServerSector serverSector = ONLINE;
        if (itServ == vecArray[0].end()) {
            itServ = std::find_if(vecArray[1].begin(), vecArray[1].end(), [=](Server serv) {return serv.getId() == id;});
            serverSector = OFFLINE;
            if (itServ == vecArray[1].end()) {
                itServ = std::find_if(vecArray[2].begin(), vecArray[2].end(), [=](Server serv) {return serv.getId() == id;});
                serverSector = NO_RESPONCE;
            }
        }

        try {
            if (itServ != vecArray[static_cast<uint8_t>(serverSector)].end()) {
                vecArray[static_cast<uint8_t>(serverSector)].erase(itServ);
                logMap.erase(logMap.find(id));
                std::wcout << L"Сервер № " << id << L" был удалён" << std::endl;
            } else {
                throw(__DELETE__NOT_FOUND_SERVER);
            }
        } catch (Error err) {
            std::wcout << L"Ошибка № " << err << L", сервер №" << id << L" не найден" << std::endl;
        }
    }
}

void Methods::manageOnline(array_vec_server_t &vecArray, const log_map_t &logMap) {
    vecArray[1].erase(std::remove_if(vecArray[1].begin(), vecArray[1].end(), [&](auto &server) {
        if (server.isOnline(logMap)) {
            vecArray[0].push_back(server);
            return true;
        }
        return false;
    }), vecArray[1].end());
}

void Methods::manageOffline(array_vec_server_t &vecArray, const log_map_t &logMap) {
    vecArray[0].erase(std::remove_if(vecArray[0].begin(), vecArray[0].end(), [&](auto &server) {
        if (!server.isOnline(logMap)) {
            vecArray[1].push_back(server);
            return true;
        }
        return false;
    }), vecArray[0].end());
}

void Methods::manageOfflineOnline(array_vec_server_t &vecArray, const log_map_t &logMap) {
    manageOnline(vecArray, logMap);
    manageOffline(vecArray, logMap);
}

void Methods::updateCSV(const std::string &CSVFilename, const std::string &title, array_vec_server_t &vecArray, const log_map_t &logMap) {
    std::ofstream CSVFile(CSVFilename);
    ReadWrite readWrite = readWrite.getInstance();
    readWrite.writeCSV(CSVFile, vecArray, title, logMap);
    CSVFile.close();
}

void Methods::updateLog(const std::string &logFilename, const log_map_t &logMap) {
    std::ofstream logFile(logFilename);
    ReadWrite readWrite = readWrite.getInstance();
    readWrite.writeLog(logFile, logMap);
    logFile.close();
}

void Methods::updateCSVandLOG(const std::string &CSVFilename, const std::string &logFilename, const std::string &title, array_vec_server_t &vecArray, const log_map_t &logMap) {
    updateCSV(CSVFilename, title, vecArray, logMap);
    updateLog(logFilename, logMap);
}

void Methods::addServer(array_vec_server_t &vecArray, std::vector<Server> &toVec, const std::string &title, log_map_t &logMap, bool toSleeping) {
    std::cout << title << std::endl;
    std::string line;
    std::getline(std::cin, line);
    ReadWrite readWrite = readWrite.getInstance();
    Server server = readWrite.getServerFromString(line);
    try {
        for (uint8_t i = 0; i <= 2; i++) {
            auto itVecArray = std::find_if(vecArray[i].begin(), vecArray[i].end(), [=] (Server serv) {return server.getId() == serv.getId();});
            if (itVecArray != vecArray[i].end()) {
                throw(__ADDSERVER__SERVER_ALREADY_EXISTS);
            }
        }
        throw(NO_ERROR);
    } catch (Error err) {
        if (err == __ADDSERVER__SERVER_ALREADY_EXISTS) {
            std::wcout << L"Ошибка №" << err << L", сервер №" << server.getId() << L" уже есть в списке серверов, он не может быть добавлен" << std::endl;
        } else {
            toVec.push_back(server);
            if (toSleeping) {
                logMap[server.getId()] = std::chrono::system_clock::now();
            } else {
                logMap[server.getId()] = std::chrono::system_clock::now() - std::chrono::hours(readWrite.getDefaultPastTimeH());
            }
            std::wcout << L"Сервер №" << server.getId() << L" был добавлен" << std::endl;
        }
    }
}

void Methods::printServerStatus(const array_vec_server_t &vecArray, const std::vector<uint32_t> &vectorId) {
    for (auto itId = vectorId.begin(); itId != vectorId.end(); itId++) {
        bool found = contains(vecArray[0], itId);
        if (found) {
            std::wcout << L"Сервер №" << *itId << L" находится в рабочих" << std::endl;
        } else if ((found = contains(vecArray[1], itId))) {
            std::wcout << L"Сервер №" << *itId << L" находится в спящих" << std::endl;
        } else if ((found = contains(vecArray[2], itId))) {
            std::wcout << L"Сервер №" << *itId << L" находится в сломанных" << std::endl;
        } else {
            std::wcout << L"Сервер №" << *itId << L" не найден" << std::endl;
        }
    }
}

void Methods::updateInfo(const std::string &infoFilename) {
    std::ofstream infoFile(infoFilename);
    ReadWrite readWrite = readWrite.getInstance();
    readWrite.writeInfo(infoFile);
    infoFile.close();
}

void Methods::makeBackup(const std::string &CSVFilenameB, const std::string &logFilenameB,
                         const std::string &infoFilename, const std::string &title,
                         array_vec_server_t &vecArray, const log_map_t &logMap) {
    ReadWrite readWrite = readWrite.getInstance();
    readWrite.setBackUpTime(std::chrono::system_clock::now());
    updateInfo(infoFilename);
    updateCSV(CSVFilenameB, title, vecArray, logMap);
    updateLog(logFilenameB, logMap);
}

void Methods::restore(const std::string &CSVFilenameB, const std::string &logFilenameB, std::string &title, array_vec_server_t &vecArray, log_map_t &logMap) {
    std::ifstream CSVFile(CSVFilenameB);
    std::ifstream logFile(logFilenameB);
    try {
        if (CSVFile.is_open() && logFile.is_open()) {
            array_vec_server_t vecArrayB;
            std::string titleB;
            log_map_t logMapB;
            ReadWrite readWrite = readWrite.getInstance();
            readWrite.readCSV(CSVFile, vecArrayB, titleB);
            readWrite.readLog(logFile, logMapB);
            vecArray = vecArrayB;
            logMap = logMapB;
            title = titleB;
            CSVFile.close();
            logFile.close();
            std::wcout << L"Восстановление из резервной копии было успешно выполнено" << std::endl;
        } else {
            throw (__RESTORE__NOT_FOUND_BACKUP);
        }
    } catch (Error err) {
        std::wcout << L"Ошибка №" << err << L", не найдены бэкапные файлы" << std::endl;
    }

}

void Methods::printSector(const std::vector<Server> &vec, const log_map_t &logMap) {
    std::vector<Server> sortedVec(vec);
    std::sort(sortedVec.begin(), sortedVec.end(), [&](const Server &server1, const Server &server2){
        return (logMap.find(server1.getId())->second < logMap.find(server2.getId())->second) ||
                (logMap.find(server1.getId())->second == logMap.find(server2.getId())->second && server1.getId() < server2.getId());
    });
    std::wcout << L"Всего " << sortedVec.size() << L" серверов в этой группе, отсортированных по дате их последнего сна:" << std::endl;
    for (auto &server : sortedVec) {
        std::cout << server.getId() << std::endl;
    }
}

void Methods::printSessionInfo() {
    ReadWrite readWrite = readWrite.getInstance();
    std::wcout << L"Текущее время сна для серверов сейчас составляет " << SLEEP_PERIODH << L" часов" << std::endl;
    std::wcout << L"Текущий разделитель " << readWrite.getSeparator() << std::endl << L"Если CSV файл открылся в одну строчку в Excel, попробуйте поменять разделитель" << std::endl;
    std::time_t backup_time = std::chrono::system_clock::to_time_t(readWrite.getBackUpTime());
    std::wcout << L"Последний бэкап был сделан ";
    std::cout << std::ctime(&backup_time);
    if (std::chrono::system_clock::now() - readWrite.getBackUpTime() > std::chrono::hours(240)) {
        std::wcout << L"Бэкап не делался больше 10 дней, используйте команду backup, чтобы создать резервную копию" << std::endl;
    }
    std::wcout << L"Для получения информации о существующих командах введите help" << std::endl;
}

void Methods::printHelp() {
    std::cout << std::string(79u, '_') << std::endl;
    std::wcout << L"rdy [num] [num] [num] -- Принудительно разбудить сервер" <<std::endl;
    std::wcout << L"slp [num] [num] [num] -- Переместить сервер в сон" <<std::endl;
    std::wcout << L"brkn [num] [num] [num] -- Переместить сервер к сломанным" << std::endl;
    std::wcout << L"repair [num] [num] [num] -- Переместить сервер из сломанных к готовым к работе" << std::endl;
    std::wcout << L"chsep [num] -- Сменить разделитель в CSV файле (0 = ;   1 = ,)" << std::endl;
    std::wcout << L"chslp [num] -- Установить количество часов, которое будут спать сервера" << std::endl;
    std::wcout << L"update -- Естественно разбудить все спящие сервера и обновить лог и CSV файлы" << std::endl;
    std::wcout << L"addr -- Добавить новый сервер к готовым к работе" << std::endl;
    std::wcout << L"adds -- Добавить новый сервер к спящим" << std::endl;
    std::wcout << L"addb -- Добавить новый сервер к сломанным" << std::endl;
    std::wcout << L"delete [num] [num] [num] -- Безвозвратно удалить сервер" << std::endl;
    std::wcout << L"status [num] [num] [num] -- Вывести информацию о сервере" << std::endl;
    std::wcout << L"worklist -- Cписок рабочих серверов, сортированных по времени последнего сна" << std::endl;
    std::wcout << L"sleeplist  -- Cписок спящих серверов, сортированных по времени последнего сна" << std::endl;
    std::wcout << L"brokenlist -- Cписок сломанных серверов сортированных по времени последнего сна" << std::endl;
    std::wcout << L"info -- Вывести общую информацию о текущем сеансе" << std::endl;
    std::wcout << L"backup -- Создать бэкап версию CSV файла" << std::endl;
    std::wcout << L"restore -- Восстановить CSV файл из бэкапа" << std::endl;
    std::wcout << L"exit -- Сохраниться и выйти из приложения" << std::endl;
    std::cout << std::string(79u, '_') << std::endl;
}
