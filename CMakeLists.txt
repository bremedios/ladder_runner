cmake_minimum_required(VERSION 3.28)

project(ladder_runner)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL2_ttf)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)


include_directories(${SDL_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS} ../libbpl_graphics/libbpl_graphics/include ../libbpl_game/libbpl_game/include ../libbpl_controls/libbpl_controls/include ../libbpl_sys/libbpl_sys/include)

add_executable(ladder_runner src/ladder_runner.cpp src/MainMenu.cpp
        src/GSSpriteViewer.cpp
        src/GSSpriteViewer.h
        src/LevelFranticLadders.cpp
        src/LevelFranticLadders.h
        src/PlatformerLevel.cpp
        src/PlatformerLevel.h
        src/PlayerLife.cpp
        src/PlayerLife.h
        src/Debug.h)

target_link_libraries(ladder_runner PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image bpl_graphics bpl_game bpl_controls bpl_storage bpl_sys)



install(TARGETS ladder_runner DESTINATION usr/bin)