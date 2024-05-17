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
    // Not sure what number does in v11
    std::string number;
    std::string drive;
    std::string folder;
    std::string fileName;
};

/*
// Example gameinfo txt file:
Final Fantasy III (USA) (Rev 1)
snes9x2005_libretro.so
Nintendo SNES-SFC
0
/mnt/sdcard/roms/
TinyBest/SFC
Final Fantasy III (USA) (Rev 1).zip
*/

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