#include "interface.hpp"

char ReadWrite::separator = ' ';
uint32_t ReadWrite::defaultPastTimeH = 1000;
time_clock_t ReadWrite::backUpTime = std::chrono::system_clock::now() - std::chrono::hours(ReadWrite::defaultPastTimeH);

std::vector<uint32_t> Interface::extractIntegerWords(const std::string &line)
{
    std::stringstream ss;
    ss << line;
    std::vector<uint32_t> intDataVec;
    while (!ss.eof()) {
        std::string readLine;
        uint32_t found;
        ss >> readLine;
        if (std::stringstream(readLine) >> found)
            intDataVec.push_back(found);
        readLine = "";
    }
    return intDataVec;
}

std::pair<Interface::Command, std::vector<uint32_t> > Interface::parseCommand(const std::string &line) {
    std::vector<uint32_t> intDataVec;
    Command command = UNDEFINED;
    auto it = std::find_if(line.begin(), line.end(), [](char c){return c == ' ' || c == '\n';});
    uint32_t pos = static_cast<uint32_t>(it - line.begin());
    std::string commandLine = line.substr(0, pos);

    std::array<std::string, 20> commandsList{"rdy", "slp", "brkn", "repair", "chsep", "chslp", "update", "addr",
                                               "adds", "addb", "delete", "status", "worklist", "sleeplist", "brokenlist",
                                               "info", "backup", "restore", "help", "exit"};
    auto itCommandLine = std::find(commandsList.begin(), commandsList.end(), commandLine);
    if (itCommandLine != commandsList.end()) {
        command = static_cast<Command>(itCommandLine - commandsList.begin() + 1);
    }

    it = std::find_if(line.begin(), line.end(), [](char c){return isdigit(c);});     //ASK
    if (it != line.end()) {
        std::string dataStr = line.substr(pos + 1, line.length() - pos);
        intDataVec = extractIntegerWords(dataStr);
    }


    return std::make_pair(command, intDataVec);
}

void Interface::readAndExecuteCommand(const std::string &CSVFilename, const std::string &logFilename, const std::string &infoFilename, array_vec_server_t &vecArray, log_map_t &logMap, std::string &title) {
    Methods& methods = Methods::getInstance();
    Command command = UNDEFINED;
    while (command != EXIT) {
        std::string line;
        std::getline(std::cin, line);
        auto pair = parseCommand(line);
        command = pair.first;
        std::vector<uint32_t> intDataVec = pair.second;
        switch (command) {
        case UNDEFINED:
            std::wcout << L"Ошибка, неверная команда" << std::endl;
            std::wcout << L"Для получения информации о существующих командах введите help" << std::endl;
            break;
        case SET_ONLINE:
            methods.setOnline(vecArray, logMap, intDataVec);
            break;
        case SET_OFFLINE:
            methods.setOffline(vecArray, logMap, intDataVec);
            break;
        case SET_NO_RESPONSE:
            methods.setNoResponse(vecArray, logMap, intDataVec);
            break;
        case REPAIR:
            methods.repair(vecArray, logMap, intDataVec);
            break;
        case CHANGE_SEPARATOR:
            if (intDataVec.empty()) {
                std::wcout << L"Ошибка, неверный входной формат команды chsep" << std::endl;
            } else {
                methods.changeSeparator(intDataVec[0], title, vecArray);
            }
            break;
        case CHANGE_SLEEP_PER:
            if (intDataVec.empty()) {
                std::wcout << L"Ошибка, неверный входной формат команды chslp" << std::endl;
            } else {
                methods.changeSleepPeriodH(intDataVec[0]);
                methods.manageOfflineOnline(vecArray, logMap);
                std::wcout << L"Количество часов для сна, необходимых серверам, теперь составляет " << SLEEP_PERIODH << std::endl;
                std::wcout << L"Все сервера, проспавшие больше " << SLEEP_PERIODH << L" часов, были разбужены" << std::endl;
                std::wcout << L"Все сервера, проспавшие меньше " << SLEEP_PERIODH << L" часов, были отправлены в сон" << std::endl;
            }
            break;
        case UPDATE:
            methods.manageOfflineOnline(vecArray, logMap);
            methods.updateLog(logFilename, logMap);
            methods.updateInfo(infoFilename);
            std::wcout << L"Все спящие больше " << SLEEP_PERIODH << L" часов сервера были разбужены" << std::endl;
            std::wcout << L"CSV, лог и инфо файлы были обновлены" << std::endl;
            break;
        case ADD_SERVER_ONLINE:
            std::wcout << L"Чтобы добавить новый готовый сервер, введите данные сервера, в формате следующего заголовка:" << std::endl;
            methods.addServer(vecArray, vecArray[0], title, logMap, false);
            break;
        case ADD_SERVER_OFFLINE:
            std::wcout << L"Чтобы добавить новый спящий сервер, введите данные сервера, в формате следующего заголовка:" << std::endl;
            std::wcout << L"Текущее время будет взято как время ухода в сон" << std::endl;
            methods.addServer(vecArray, vecArray[1], title, logMap, true);
            break;
        case ADD_SERVER_NO_RESPONSE:
            std::wcout << L"Чтобы добавить новый сломанный сервер, введите данные сервера, в формате следующего заголовка:" << std::endl;
            methods.addServer(vecArray, vecArray[2], title, logMap, false);
            break;
        case DELETE_SERVER:
            methods.deleteServer(vecArray, logMap, intDataVec);
            break;
        case SERVER_STATUS:
            methods.printServerStatus(vecArray, intDataVec);
            break;
        case ONLINE_LIST:
            methods.printSector(vecArray[0], logMap);
            break;
        case OFFLINE_LIST:
            methods.printSector(vecArray[1], logMap);
            break;
        case NO_RESPONSE_LIST:
            methods.printSector(vecArray[2], logMap);
            break;
        case INFO:
            methods.printSessionInfo();
            break;
        case BACKUP:
            methods.makeBackup("Server_backup.csv", "log_backup.txt", infoFilename, title, vecArray, logMap);
            std::wcout << L"Бэкап сделан, инфо файл был обновлён" << std::endl;
            break;
        case RESTORE:
            methods.restore("Server_backup.csv", "log_backup.txt", title, vecArray, logMap);
            break;
        case HELP:
            methods.printHelp();
            break;
        case EXIT:
            std::wcout << L"Работа завершена, все изменения успешно сохранены" << std::endl;
            std::wcout << L"Для закрытия приложения нажмите <Enter>..." << std::endl;
            break;
        default:
            break;
        }
        std::cout << std::endl;
        methods.updateCSVandLOG(CSVFilename, logFilename, title, vecArray, logMap);
    }


}

bool Interface::interface() {
    //Setting default variables
    ReadWrite readWrite = readWrite.getInstance();

    //Reading CSV
    std::string CSVFilename = "Server.csv";
    std::ifstream CSVFile(CSVFilename);
    if (!CSVFile.is_open()) {
        std::wcout << L"Ошибка, не найден файл ";
        std::cout << CSVFilename << std::endl;
        system("pause");
        return false;
    }
    array_vec_server_t vecArray;
    std::string title;

    readWrite.readCSV(CSVFile, vecArray, title);

    //Reading (creating) log
    std::string logFilename = "log.txt";
    std::ifstream logFile(logFilename);
    if (!logFile.is_open()) {
        readWrite.createLog(logFilename, vecArray);
        logFile.open(logFilename);
    }
    log_map_t logMap;
    readWrite.readLog(logFile, logMap);
    CSVFile.close();
    logFile.close();

    //Reading (creating) info file
    std::string infoFilename = "info.txt";
    std::ifstream infoFile(infoFilename);
    if (!infoFile.is_open()) {
        readWrite.createInfo(infoFilename);
        infoFile.open(infoFilename);
    }
    readWrite.readInfo(infoFile);

    //Updating
    Methods& methods = Methods::getInstance();
    methods.manageOfflineOnline(vecArray, logMap);
    methods.updateCSV(CSVFilename, title, vecArray, logMap);

    //Greetings
    std::wcout << L"Добро пожаловать в программу ServerTimer!" << std::endl;
    methods.printSessionInfo();
    std::cout << std::string(79u, '_') << std::endl;

    //reading and executing commands
    readAndExecuteCommand(CSVFilename, logFilename, infoFilename, vecArray, logMap, title);

    //Updating
    methods.updateInfo(infoFilename);
    methods.manageOfflineOnline(vecArray, logMap);
    methods.updateCSVandLOG(CSVFilename, logFilename, title, vecArray, logMap);
    return true;
}
