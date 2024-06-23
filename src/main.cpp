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
#include "enum.h"
#include "helpers/strHelpers.h"
#include "helpers/mathHelpers.h"

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
string MUOS_FAVORITE_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/info/favourite";
string MUOS_SAVE_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/save/state";
#else
bool debugMode = false;
string MUOS_HISTORY_DIR = "/mnt/mmc/MUOS/info/history";
string MUOS_FAVORITE_DIR = "/mnt/mmc/MUOS/info/favourite";
string MUOS_SAVE_DIR = "/mnt/mmc/MUOS/save/state";
#endif

SDL_Color defaultTextColor = {255, 255, 255, 255};
SDL_Color shadowTextColor = {0, 0, 0, 225};
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *defaultFont = nullptr;
TTF_Font *mdFont = nullptr;
TTF_Font *lgFont = nullptr;
TTF_Font *titleFont = nullptr;
SDL_Joystick *joystick = nullptr;

vector<GameInfoData> recentGameList;
vector<GameInfoData> favoriteGameList;
vector<GameInfoData> currentGameList;

GameInfoData selectedGame;
GameVisualData selectedGameVisual;
Mustard::Renderer *mrenderer = nullptr;
int selectedGameIndex;

AppState appState = APPSTATE_RECENTVIEW;
bool isListView = false;
bool isPictureView = false;
string listViewTitle;
double camX;
double camY;

double approachCamX;
double approachCamY;

int dirXInput = 0;
int dirYInput = 0;

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
    defaultFont = TTF_OpenFont("assets/font/Orbitron-Medium-Uppercase.ttf", 24);
    mdFont = TTF_OpenFont("assets/font/Orbitron-Medium-Uppercase.ttf", 28);
    lgFont = TTF_OpenFont("assets/font/Orbitron-Medium-Uppercase.ttf", 38);
    titleFont = TTF_OpenFont("assets/font/Orbitron-Medium-Uppercase.ttf", 45);
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
    if (isPictureView)
    {
        approachCamX = 0;
        approachCamY = 0;
        string header = "assets/theme/header.png";

        // Draw background text
        if (selectedGame.active)
        {
            drawTextCentered(selectedGame.coreName, defaultFont, renderer, 0 - camX * 0.5, 216 - camY, 640, {255, 255, 255, 80});
        }

        // Draw the image
        if (selectedGameVisual.active)
        {
            string path = selectedGameVisual.filePath;
            auto *textureData = mrenderer->getData(path);
            if (textureData && textureData->width > 32 && textureData->height > 32)
            {
                mrenderer->drawPreserveAspect(path, 320 - camX, 240 - camY, 640, 400, 0, 1);
            }
        }

        // Draw top text / header
        mrenderer->draw(header, 0, 0, 640, 40);
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
    if (isListView)
    {
        approachCamX = 0;
        const int leftMargin = 32;
        const int topMargin = 24;
        const int titleMargin = 60;
        const int lineHeight = 42;
        auto calcY = [](int y, int offset = 0)
        {
            return titleMargin + topMargin + y * lineHeight + offset;
        };

        int selectedY = calcY(selectedGameIndex, 0);
        if (selectedY > 150)
        {
            approachCamY = selectedY - 150;
        }
        else
        {
            approachCamY = 0;
        }

        // Draw title
        drawText(listViewTitle, lgFont, renderer, leftMargin, topMargin - camY, {200, 175, 25, 255});
        for (int i = 0; i < currentGameList.size(); i++)
        {
            string prettyName = currentGameList[i].name;
            int maxLen = 48;
            int textY = calcY(i) - camY;

            // Check if screen position is in range
            if (textY > -35 && textY < 480 + 35)
            {
                int textX = leftMargin;
                if (prettyName.length() > maxLen)
                {
                    prettyName = prettyName.substr(0, maxLen - 2) + "...";
                }
                string text;
                text = prettyName;
                if (i == selectedGameIndex)
                {
                    mrenderer->drawRect(0, textY - 5, 640, lineHeight, {255, 255, 255, 25});
                    drawText(text, defaultFont, renderer, textX, textY, {220, 190, 25, 255});
                }
                else
                {
                    drawText(text, defaultFont, renderer, textX, textY, defaultTextColor);
                }
            }
        }
    }

    // Draw footer
    string footer = "assets/theme/footer.png";
    mrenderer->draw(footer, 0, 440, 640, 40);
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

void setAppState(AppState state)
{
    camY = -25;
    camX = 0;
    approachCamY = 0;
    approachCamX = 0;
    appState = state;
    selectedGameIndex = 0;
    selectedGame = {};
    selectedGameVisual = {};
    listViewTitle = "";
    isListView = false;
    isPictureView = false;
    currentGameList.clear();

    if (appState == APPSTATE_RECENTVIEW)
    {
        isPictureView = true;
        currentGameList = recentGameList;
    }
    else if (appState == APPSTATE_FAVORITELIST)
    {
        isListView = true;
        currentGameList = favoriteGameList;
        // Sort alphabetically
        std::sort(currentGameList.begin(), currentGameList.end(), [](const GameInfoData &a, const GameInfoData &b)
                  { return a.name < b.name; });
        listViewTitle = "Favorites";
    }
    else if (appState == APPSTATE_RECENTLIST)
    {
        isListView = true;
        currentGameList = recentGameList;
        listViewTitle = "Recent Games";
        // Sort alphabetically
        std::sort(currentGameList.begin(), currentGameList.end(), [](const GameInfoData &a, const GameInfoData &b)
                  { return a.name < b.name; });
    }
}

void nextAppState(int i)
{
    setAppState(static_cast<AppState>((appState + i) % 3));
}

void updateAppState()
{

    if (isPictureView)
    {
        if (dirXInput != 0)
        {
            selectedGameIndex += dirXInput;
            camX -= dirXInput * 32;
        }
    }
    if (isListView)
    {
        if (dirYInput != 0)
        {
            selectedGameIndex -= dirYInput;
        }
        if (dirXInput != 0)
        {
            selectedGameIndex += dirXInput;

            int jumpSize = 4;
            if (dirXInput < 0)
            {
                for (int i = 0; i < jumpSize; i++)
                {
                    if (selectedGameIndex > 0)
                    {
                        selectedGameIndex--;
                    }
                }
            }
            else if (dirXInput > 0)
            {
                for (int i = 0; i < jumpSize; i++)
                {
                    if (selectedGameIndex < currentGameList.size() - 1)
                    {
                        selectedGameIndex++;
                    }
                }
            }
        }
    }

    int games = currentGameList.size();
    if (games > 0)
    {
        // Ensure negative numbers wrap around
        selectedGameIndex = (selectedGameIndex + games * 100) % games;
        selectedGame = currentGameList[selectedGameIndex];
    }
    else
    {
        selectedGameIndex = 0;
        selectedGame = {};
    }

    selectedGameVisual = {};
    if (selectedGame.active && isPictureView)
    {
        selectedGameVisual = loadGameVisualData(selectedGame, MUOS_SAVE_DIR);
    }
}

void startSDLPhase()
{
    initSDL();

    setAppState(APPSTATE_RECENTVIEW);

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

            if ((event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == RGBUTTON_B) ||
                (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == RGBUTTON_SELECT) ||
                (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_B))
            {
                nextAppState(1);
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

        dirXInput = 0;
        dirYInput = 0;
        // Handle Directional input
        {
            int desiredXIndex = 0;
            int desiredYIndex = 0;
            if (SDL_JoystickGetHat(joystick, 0) == RGPAD_RIGHT || keyboardState[SDL_SCANCODE_RIGHT])
            {
                desiredXIndex = 1;
            }
            else if (SDL_JoystickGetHat(joystick, 0) == RGPAD_LEFT || keyboardState[SDL_SCANCODE_LEFT])
            {
                desiredXIndex = -1;
            }
            if (SDL_JoystickGetHat(joystick, 0) == RGPAD_DOWN || keyboardState[SDL_SCANCODE_DOWN])
            {
                desiredYIndex = -1;
            }
            else if (SDL_JoystickGetHat(joystick, 0) == RGPAD_UP || keyboardState[SDL_SCANCODE_UP])
            {
                desiredYIndex = 1;
            }

            if (desiredXIndex != 0 || desiredYIndex != 0)
            {
                // Tetris Auto-repeat
                if (dasTimer <= 0.0)
                {
                    dirXInput += desiredXIndex;
                    dirYInput += desiredYIndex;
                }
                else if (dasTimer > 0.2666)
                {
                    dirXInput += desiredXIndex;
                    dirYInput += desiredYIndex;
                    dasTimer -= 0.1;
                }
                dasTimer += deltaTime;
            }
            else
            {
                dasTimer = 0.0;
            }
        }

        updateAppState();

        // Shutdown if MENU or ESCAPE is pressed
        if (sdlTime > 0.1 && (SDL_JoystickGetButton(joystick, RGBUTTON_MENU) || keyboardState[SDL_SCANCODE_ESCAPE]))
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
        }

        // Update at 60 FPS if possible. Min Delay of 4
        SDL_Delay(4);
        while (SDL_GetTicks() - lastTicks < 16)
        {
            SDL_Delay(1);
        }
        uint32_t currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - lastTicks) / 1000.0;
        lastTicks = currentTicks;
        sdlTime += deltaTime;

        camY = lerp(camY, approachCamY, 0.1);
        camY = clamp(camY, approachCamY - 200, approachCamY + 200);

        camX = lerp(camX, approachCamX, 0.1);
        camX = clamp(camX, approachCamX - 200, approachCamX + 200);
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
        recentGameList = loadGameListAtPath(MUOS_HISTORY_DIR);
        favoriteGameList = loadGameListAtPath(MUOS_FAVORITE_DIR);
        currentGameList = recentGameList;

        // Trim list to 10 games
        // if (recentGameList.size() > 10)
        // {
        //     recentGameList.resize(10);
        // }

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
