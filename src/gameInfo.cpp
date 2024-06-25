
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "gameInfo.h"
#include "helpers/strHelpers.h"

using namespace std;

GameInfoData loadGameInfo(string filePath)
{
    GameInfoData game;

    std::ifstream file(filePath);
    if (file.is_open())
    {
        game.active = true;
        std::string line;
        int lineCount = 0;
        while (std::getline(file, line))
        {
            switch (lineCount)
            {
            case 0:
                game.name = line;
                break;
            case 1:
                game.core = line;
                break;
            case 2:
                game.coreName = line;
                break;
            case 3:
                game.number = line;
                break;
            case 4:
                game.drive = line;
                break;
            case 5:
                game.folder = line;
                break;
            case 6:
                game.fileName = line;
                break;
            default:
                break;
            }
            lineCount++;
        }
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }

    return game;
}

GameInfoData writeGameInfo(string filePath, GameInfoData game)
{
    ofstream file(filePath);
    if (file.is_open())
    {
        file << game.name << std::endl;
        file << game.core << std::endl;
        file << game.coreName << std::endl;
        file << game.number << std::endl;
        file << game.drive << std::endl;
        file << game.folder << std::endl;
        file << game.fileName << std::endl;

        // Ensure data is synced to disk immediately
        file.flush();
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }
    return game;
}

vector<GameInfoData> loadGameListAtPath(string folderPath)
{
    vector<filesystem::directory_entry> entries;
    vector<GameInfoData> games;

    // Check if directory exists
    if (filesystem::exists(folderPath))
    {
        for (const auto &entry : filesystem::directory_iterator(folderPath))
        {
            if (entry.path().extension() == ".cfg")
            {
                entries.push_back(entry);
            }
        }

        std::sort(entries.begin(), entries.end(), [](const filesystem::directory_entry &a, const filesystem::directory_entry &b)
                  { return filesystem::last_write_time(a) > filesystem::last_write_time(b); });

        for (const auto &entry : entries)
        {
            if (entry.path().extension() == ".cfg")
            {
                GameInfoData gameInfo = loadGameInfo(entry.path());
                if (strEndsWith(gameInfo.core, "libretro.so"))
                {
                    games.push_back(gameInfo);
                }
            }
        }
    }
    return games;
}

GameVisualData loadGameVisualData(GameInfoData game, string folderPath)
{
    GameVisualData visualData;
    if (game.active && game.coreName.length() > 0 && game.fileName.length() > 0)
    {
        string partialMatchFolder;
        string exactMatchFolder;
        vector<filesystem::directory_entry> subDirectories;
        for (const auto &entry : filesystem::directory_iterator(folderPath))
        {
            if (entry.is_directory())
            {
                subDirectories.push_back(entry);
            }
        }

        string normalizedCore = game.core;
        vector<string> coreSplit = strSplit(game.core, '_');
        if (coreSplit.size() > 1)
        {
            coreSplit.pop_back();
            normalizedCore = strJoin(coreSplit, "");
        }
        normalizedCore = strReplaceAll(normalizedCore, " ", "");
        normalizedCore = strReplaceAll(normalizedCore, "-", "");
        normalizedCore = strReplaceAll(normalizedCore, "_", "");
        normalizedCore = strToUpper(normalizedCore);

        // printf("Searching for core: %s\n", normalizedCore.c_str());
        for (const auto &entry : subDirectories)
        {
            string foldername = entry.path().filename();
            string normalizedFolderName = strReplaceAll(strToUpper(foldername), " ", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, " ", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, "-", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, "_", "");

            if (normalizedCore == normalizedFolderName)
            {
                exactMatchFolder = entry.path().string();
                // printf("Found Exact Match: %s\n", normalizedFolderName.c_str());
                break;
            }
            else if (strStartsWith(normalizedFolderName, normalizedCore))
            {
                partialMatchFolder = entry.path().string();
                // printf("Partial Match: %s\n", normalizedFolderName.c_str());
            }
        }

        string subFolderPath = exactMatchFolder.length() > 0 ? exactMatchFolder : partialMatchFolder;

        if (subFolderPath.length() > 0)
        {
            vector<filesystem::directory_entry> screenShots;

            // Check if directory exists
            if (filesystem::exists(subFolderPath))
            {
                for (const auto &entry : filesystem::directory_iterator(subFolderPath))
                {
                    std::string extension = entry.path().extension().string();
                    std::string fileName = entry.path().filename().string();
                    if (extension == ".png")
                    {
                        screenShots.push_back(entry);
                    }
                }
                std::sort(screenShots.begin(), screenShots.end(), [](const filesystem::directory_entry &a, const filesystem::directory_entry &b)
                          { return filesystem::last_write_time(a) > filesystem::last_write_time(b); });

                if (screenShots.size() > 0)
                {
                    visualData.active = true;
                    std::string gameFileName = game.fileName;
                    std::size_t dotIndex = gameFileName.find(".");
                    if (dotIndex != std::string::npos)
                    {
                        gameFileName = gameFileName.substr(0, dotIndex);
                    }
                    for (const auto &entry : screenShots)
                    {
                        std::string fileName = entry.path().filename().string();
                        std::size_t dotIndex = fileName.find(".");
                        if (dotIndex != std::string::npos)
                        {
                            fileName = fileName.substr(0, dotIndex);
                        }
                        if (fileName == gameFileName)
                        {
                            visualData.filePath = entry.path().string();
                            break;
                        }
                    }
                }
            }
        }
    }
    return visualData;
}