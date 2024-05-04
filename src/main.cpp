#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "font.h"
#include "gameInfo.h"
#include "mrenderer.h"

using namespace std;
namespace fs = std::filesystem;

bool needExit = false;
bool needShutdown = false;
int swWidth = 640;
int swHeight = 480;
int hwWidth = 640;
int hwHeight = 480;

string ROM_GO = "/tmp/rom_go";

#ifdef DEBUG
bool debugMode = true;
string MUOS_HISTORY_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/info/history";
string MUOS_SAVE_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/save/state";
#else
bool debugMode = false;
string MUOS_HISTORY_DIR = "/mnt/mmc/MUOS/info/history";
string MUOS_SAVE_DIR = "/mnt/mmc/MUOS/save/state";
#endif

SDL_Color defaultTextColor = {255, 255, 255, 255};
SDL_Color shadowTextColor = {0, 0, 0, 225};
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *defaultFont = nullptr;
TTF_Font *titleFont = nullptr;
SDL_Joystick *joystick = nullptr;

vector<GameInfoData> gameList;
GameInfoData selectedGame;
GameVisualData selectedGameVisual;
Mustard::Renderer *mrenderer = nullptr;
int selectedGameIndex;

enum RGPad
{
    RGPAD_UP = 1,
    RGPAD_RIGHT = 2,
    RGPAD_DOWN = 4,
    RGPAD_LEFT = 8,
    RGPAD_CENTER = 0,
};

enum RGButton
{
    RGBUTTON_VOL_DOWN = 1,
    RGBUTTON_VOL_UP = 2,
    RGBUTTON_A = 3,
    RGBUTTON_B = 4,
    RGBUTTON_Y = 5,
    RGBUTTON_X = 6,
    RGBUTTON_L1 = 7,
    RGBUTTON_R1 = 8,
    RGBUTTON_SELECT = 9,
    RGBUTTON_START = 10,
    RGBUTTON_MENU = 11,
    RGBUTTON_L2 = 13,
    RGBUTTON_R2 = 14,
};

void initSDL()
{

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    // Connect joystick 0
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);

    window = SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, hwWidth, hwHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer)
    {
        if (mrenderer != nullptr)
        {
            delete mrenderer;
        }
        mrenderer = new Mustard::Renderer(renderer);
    }
    TTF_Init();
    defaultFont = TTF_OpenFont("assets/font/Allerta-Regular.ttf", 22);
    titleFont = TTF_OpenFont("assets/font/Allerta-Regular.ttf", 45);
}

// Clear buffers
void startRender()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void renderBlackScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}
// Render screen
void renderGameSwitcher()
{

    bool drawPicture = true;
    bool drawList = false;

    if (drawPicture)
    {
        string bg = "assets/theme/bg.png";
        string header = "assets/theme/header.png";
        string footer = "assets/theme/footer.png";
        mrenderer->draw(bg, 0, 0, 640, 480);
        if (selectedGameVisual.active)
        {
            // Draw the image
            string path = selectedGameVisual.filePath;
            mrenderer->drawPreserveAspect(path, 320, 240, 640, 400, 0, 1);
        }

        mrenderer->draw(header, 0, 0, 640, 40);
        mrenderer->draw(footer, 0, 440, 640, 40);
        if (selectedGame.active)
        {
            string prettyName = selectedGame.name;
            int maxLen = 48;
            if (prettyName.length() > maxLen)
            {
                prettyName = prettyName.substr(0, maxLen - 2) + "...";
            }
            drawTextCentered(prettyName, defaultFont, renderer, 2, 7, 640, shadowTextColor);
            drawTextCentered(prettyName, defaultFont, renderer, 0, 4, 640, defaultTextColor);
        }
    }
    if (drawList)
    {
        string text = "Game Switcher\nRoms: " + to_string(gameList.size()) + "\n";
        for (int i = 0; i < gameList.size(); i++)
        {
            string prettyName = gameList[i].name;
            int maxLen = 40;
            if (prettyName.length() > maxLen)
            {
                prettyName = prettyName.substr(0, maxLen - 2) + "...";
            }
            if (i == selectedGameIndex)
            {
                text += "> " + prettyName + "\n";
            }
            else
            {
                text += prettyName + "\n";
            }
        }
        drawTextWrapped(text, defaultFont, renderer, 4, 12, 640, defaultTextColor);
    }
}

// Blit a color to the screen. Can be transparent
void renderColor(SDL_Color color)
{
    SDL_Rect rect = {
        0,
        0,
        swWidth,
        swHeight,
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}
// Update the screen
void applyRender()
{
    SDL_RenderPresent(renderer);
}

void updateSelectedGame()
{
    int games = gameList.size();
    if (games > 0)
    {
        // Ensure negative numbers wrap around
        selectedGameIndex = (selectedGameIndex + games * 100) % games;
        selectedGame = gameList[selectedGameIndex];
    }
    else
    {
        selectedGameIndex = 0;
        selectedGame = {};
    }

    selectedGameVisual = {};
    if (selectedGame.active)
    {
        selectedGameVisual = loadGameVisualData(selectedGame, MUOS_SAVE_DIR);
    }
}

void startSDLPhase()
{
    initSDL();

    selectedGameIndex = 0;

    startRender();
    renderGameSwitcher();
    applyRender();

    // Wait 100 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SDL_Event event;
    bool startNextPhase = false;
    double dasTimer = 0.0;
    double deltaTime = 0.0;
    double shutoffHoldTimer = 0.0;
    double sdlTime = 0.0;
    uint32_t lastTicks = SDL_GetTicks();

    // SDL Update loop
    while (!startNextPhase)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                needExit = true;
                startNextPhase = 1;
                break;
            }
        }

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        // Exit if the user presses X
        if (SDL_JoystickGetButton(joystick, RGBUTTON_X) || keyboardState[SDL_SCANCODE_X])
        {
            needExit = true;
            startNextPhase = 1;
        }

        // Select a game when A or SPACE is pressed
        if (SDL_JoystickGetButton(joystick, RGBUTTON_A) || keyboardState[SDL_SCANCODE_SPACE])
        {
            startNextPhase = 1;
        }

        // Handle Directional input
        {
            int desiredDeltaIndex = 0;
            if (SDL_JoystickGetHat(joystick, 0) == RGPAD_RIGHT || keyboardState[SDL_SCANCODE_RIGHT])
            {
                desiredDeltaIndex = 1;
            }
            else if (SDL_JoystickGetHat(joystick, 0) == RGPAD_LEFT || keyboardState[SDL_SCANCODE_LEFT])
            {
                desiredDeltaIndex = -1;
            }

            if (desiredDeltaIndex != 0)
            {
                // Tetris Auto-repeat
                if (dasTimer <= 0.0)
                {
                    selectedGameIndex += desiredDeltaIndex;
                }
                else if (dasTimer > 0.2666)
                {
                    selectedGameIndex += desiredDeltaIndex;
                    dasTimer -= 0.1;
                }
                dasTimer += deltaTime;
            }
            else
            {
                dasTimer = 0.0;
            }
        }

        updateSelectedGame();

        // Shutdown if MENU or ESCAPE is pressed
        if (sdlTime > 0.15 && (SDL_JoystickGetButton(joystick, RGBUTTON_MENU) || keyboardState[SDL_SCANCODE_ESCAPE]))
        {
            // drawTextCentered("Hold to Power Off...", defaultFont, renderer, 0, 200, 640, defaultTextColor);
            if (shutoffHoldTimer > 0.34)
            {
                renderColor({0, 0, 0, 32});
            }
            else
            {

                renderColor({0, 0, 0, 5});
            }

            shutoffHoldTimer += deltaTime;
            if (SDL_JoystickGetButton(joystick, RGBUTTON_SELECT))
            {
                // If select is held, exit immediately
                shutoffHoldTimer += 1;
            }

            if (shutoffHoldTimer > 0.9)
            {
                needExit = true;
                needShutdown = true;
                startNextPhase = 1;
            }
            applyRender();
            SDL_Delay(30);
        }
        else
        {
            shutoffHoldTimer = 0;
            // Render and wait for next frame
            startRender();
            renderGameSwitcher();
            applyRender();
            SDL_Delay(30);
        }

        uint32_t currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - lastTicks) / 1000.0;
        lastTicks = currentTicks;
        sdlTime += deltaTime;

        if (startNextPhase == 1)
        {
            break;
        }
    }
}

void cleanupSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(defaultFont);
    TTF_CloseFont(titleFont);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    cout << "Starting game switcher" << endl;

    for (int i = 0; i < 999; i++)
    {
        gameList = loadGameListAtPath(MUOS_HISTORY_DIR);

        // Trim list to 10 games
        if (gameList.size() > 10)
        {
            gameList.resize(10);
        }

        startSDLPhase();

        if (needShutdown)
        {
            printf("User has triggerd a shutdown....\n");
            startRender();
            renderBlackScreen();
            applyRender();
            // Ensure everything is saved before shutting down
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            sync();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            cleanupSDL();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (!debugMode)
            {
                reboot(RB_POWER_OFF);
            }
            return 1;
        }
        else if (needExit)
        {
            printf("User has triggered an exit...\n");
            startRender();
            renderBlackScreen();
            applyRender();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            cleanupSDL();
            return 1;
        }

        if (selectedGame.active)
        {
            try
            {
                string launchPath = fs::current_path().string() + "/assets/sh/mylaunch.sh";
                printf("Launch Path: %s\n", launchPath.c_str());
                printf("Name: %s\n", selectedGame.name.c_str());

                printf("Writing Game Info\n");
                string historyPath = MUOS_HISTORY_DIR + "/" + selectedGame.name + ".cfg";

                if (!debugMode)
                {
                    writeGameInfo(historyPath, selectedGame);
                }
                printf("Finished writing Game Info\n\n");
                printf("Proceeding to game...\n");

                renderColor({0, 0, 0, 180});
                // drawTextCentered("Launching Game...", titleFont, renderer, 0, 200, 640, {255, 255, 255, 45});
                applyRender();

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                cleanupSDL();
                sync();
                std::this_thread::sleep_for(std::chrono::milliseconds(25));

                string romPath = selectedGame.drive + selectedGame.folder + "/" + selectedGame.fileName;
                string cmd = "";

                // Check if /mnt/mmc/MUOS/.retroarch/retroarch.cfg exists
                if (fs::exists("/mnt/mmc/MUOS/.retroarch/retroarch.cfg"))
                {
                    // V10
                    cmd = "/mnt/mmc/MUOS/retroarch -c \"/mnt/mmc/MUOS/.retroarch/retroarch.cfg\" -L \"/mnt/mmc/MUOS/core/" + selectedGame.core + "\" \"" + romPath + "\"";
                }
                else
                {
                    // V11
                    cmd = "retroarch -c \"/mnt/mmc/MUOS/retroarch/retroarch.cfg\" -L \"/mnt/mmc/MUOS/core/" + selectedGame.core + "\" \"" + romPath + "\"";
                }
                printf("Executing Command: %s\n", cmd.c_str());
                // Execute the command when not in debug mode
                if (!debugMode)
                {
                    system(cmd.c_str());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
                sync();
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error when luanching game. Exiting" << std::endl;
                std::cerr << e.what() << std::endl;

                cleanupSDL();
                return 1;
            }
        }
    }
    return 0;
}
