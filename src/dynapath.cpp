#include <iostream>
#include <fstream>
#include <string>

// looks through retroarch config file and pulls out the savestate and savefile directory
// useful if changed from the default location
// currently outputs to a log.txt file

void pathvar() {
    std::ifstream configFile("/mnt/mmc/MUOS/retroarch.cfg");
    std::ofstream logFile("log.txt");

    if (configFile.is_open() && logFile.is_open()) {
        std::string line;
        std::string targetWord1 = "savefile_directory";
        std::string targetWord2 = "savestate_directory";
        std::string path1, path2;

        while (std::getline(configFile, line)) {
            if (line.find(targetWord1) == 0) {
                // Extract path value from the line (assuming the path is surrounded by double quotes)
                path1 = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);
            }
            if (line.find(targetWord2) == 0) {
                // Extract path value from the line (assuming the path is surrounded by double quotes)
                path2 = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);
            }
        }

        if (!path1.empty() && !path2.empty()) {
            // Output the path values to the log file
            logFile << path1 << std::endl;
            logFile << path2 << std::endl;

            std::cout << "Paths extracted and logged successfully." << std::endl;
        } else {
            std::cout << "No lines starting with \"" << targetWord1 << "\" and \"" << targetWord2 << "\" found in the config file." << std::endl;
        }
    } else {
        std::cout << "Failed to open config file or log file." << std::endl;
    }

    configFile.close();
    logFile.close();
}
