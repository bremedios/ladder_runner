//
// Created by Bradley Remedios on 10/28/24.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <ctime>
#include <cstdlib>

#include "SDL2/SDL_ttf.h"

#include <bpl/game/GameStateMachine.h>
#include <bpl/game/GameWindow.h>
#include "MainMenu.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define FRAME_RATE 60
#define FULLSCREEN false

int main(int argc, char *argv[]) {
    std::ofstream log("/root/log.txt");

    log << "Starting Ladder Runner" << std::endl;

    std::string currentpath = std::filesystem::current_path();

    log << "Current path: " << currentpath << std::endl;

#if 0
    if (currentpath != "/userdata/roms/tools/ladder_runner") {
        std::filesystem::current_path(std::filesystem::path("userdata/roms/tools/ladder_runner"));
    }
#endif
    bool fullscreen = FULLSCREEN;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--fullscreen")) {
            fullscreen = true;
        }
        else if (!strcmp(argv[i], "--windowed")) {
            fullscreen = false;
        }
    }

    srand(time(nullptr));

    std::cout << "Ladder Runner" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO)< 0) {
        std::cerr << "SDL Could not initialize.  SDL ERROR: " << SDL_GetError() << std::endl;

        return -1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "SDL TTF Could not initialize.  SDL ERROR: " << SDL_GetError() << std::endl;

        SDL_Quit();
        return -1;
    }

    //
    //  Setup the first state for the state machine and execute the state machine
    //
    if (!bpl::game::GameStateMachine::getInstance()->Create("Ladder Runner", WINDOW_WIDTH, WINDOW_HEIGHT,FRAME_RATE,fullscreen)) {
        std::cerr << "Failed to create game state machine.  Exiting." << std::endl;

        TTF_Quit();
        SDL_Quit();

        return -1;
    }

    bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
    bpl::game::GameStateMachine::getInstance()->Run();
    bpl::game::GameStateMachine::getInstance()->Shutdown();

    std::cout << "Shutting down SDL" << std::endl;

    TTF_Quit();
    SDL_Quit();

    return 0;
} // main