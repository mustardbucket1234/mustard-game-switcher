#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <string>

struct GameInfoData
{
    bool active = false;
    std::string name;
    std::string core;
    std::string coreName;
    std::string drive;
    std::string folder;
    std::string fileName;
};

struct GameVisualData
{
    bool active = false;
    std::string filePath;
};

GameInfoData loadGameInfo(std::string filePath);
GameInfoData writeGameInfo(std::string filePath, GameInfoData game);
std::vector<GameInfoData> loadGameListAtPath(std::string folderPath);
GameVisualData loadGameVisualData(GameInfoData game, std::string folderPath);

#endif