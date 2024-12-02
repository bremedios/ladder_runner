//
// Created by bradr on 11/7/24.
//
#include <iostream>
#include <fstream>
#include <list>

#include <bpl/game/GameStateMachine.h>

#include "PlatformerLevel.h"

#include "Debug.h"

PlatformerLevel::PlatformerLevel() = default;

PlatformerLevel::~PlatformerLevel() = default;

const bpl::graphics::SpriteList &PlatformerLevel::GetBombs() const {
    return m_bombs;
} // GetBombs

bool PlatformerLevel::RemoveBomb(bpl::graphics::SpritePtr bomb) {
    m_bombs.remove(bomb);
    m_tiles.remove(bomb);

    return true;
} // RemoveBomb

const bpl::graphics::SpriteList &PlatformerLevel::GetLadders() const {
    return m_ladders;
} // GetLadders

const bpl::graphics::SpriteList &PlatformerLevel::GetFloors() const {
    return m_floors;
} // GetFloors

const bpl::graphics::SpriteList &PlatformerLevel::GetTiles() const {
    return m_tiles;
} // GetTiles

const bpl::graphics::SpriteList &PlatformerLevel::GetTreasure() const {
    return m_treasure;
} // GetTreasure

bpl::game::PlatformerEnemyTokenList &PlatformerLevel::GetEnemies() {
    return m_enemies;
} // GetEnemies

const SDL_Rect& PlatformerLevel::GetPlayerStartPosition() const {
    return m_playerStartLocation;
} // GetPlayerStartPosition

int PlatformerLevel::GetSpriteSize() const {
    return m_spriteSize;
} // GetSpriteSize

const SDL_Rect &PlatformerLevel::GetBounds() const {
    return m_bounds;
} // GetBounds

bool PlatformerLevel::RemoveTreasure(bpl::graphics::SpritePtr treasure) {
    m_treasure.remove(treasure);
    m_tiles.remove(treasure);

    return true;
} // RemoveTreasure

void PlatformerLevel::Clear() {
    m_treasure.clear();
    m_bombs.clear();
    m_tiles.clear();
    m_floors.clear();
    m_ladders.clear();
    m_enemies.clear();
} // Clear

void PlatformerLevel::SetSpriteSize_(int maxDisplayWidth, int maxDisplayHeight) {
    int maxSpriteWidth = 0;
    int maxSpriteHeight = 0;

    maxSpriteWidth = maxDisplayWidth / m_mapWidth;
    maxSpriteHeight = maxDisplayHeight / m_mapHeight;

    m_spriteSize = std::min(maxSpriteWidth, maxSpriteHeight);

    m_bounds.w = m_mapWidth * m_spriteSize;
    m_bounds.h = m_mapHeight * m_spriteSize;
    m_bounds.x = (maxDisplayWidth - m_bounds.w) / 2;
    m_bounds.y = (maxDisplayHeight - m_bounds.h) / 2;

    m_playerStartLocation = SDL_Rect { m_playerStartGridLocation.x * m_spriteSize  + m_bounds.x, m_playerStartGridLocation.y * m_spriteSize + m_bounds.y, 0,0};
} // SetSpriteSize_

bool PlatformerLevel::LoadMap_(const nlohmann::json& json_data, const std::string& map_name) {
    std::list<std::string> rows;

    try {
        rows = json_data[map_name].get<std::list<std::string>>();
    } catch (nlohmann::json::exception& e) {
        ERROR_MSG("Failed to copy map rows: " << e.what());

        return false;
    }

    int height = 0;
    int mapWidth = 0;

    for (std::list<std::string>::iterator it = rows.begin(); it != rows.end(); it++) {
        std::stringstream ss(*it);
        std::string tile;

        int width = 0;
        while (ss >> tile) {
            bpl::graphics::SpritePtr sprite = bpl::graphics::SpritePtr(new bpl::graphics::Sprite(bpl::game::GameStateMachine::getInstance()->GetRenderer(), bpl::game::GameStateMachine::getInstance()->GetFramerate()));

            std::string tileName;
            std::string tileType;

            //
            // 00 is no tile, we add an empty sprite pointer and move to the next.
            //
            if (tile == "00") {
                ++width;
                mapWidth = std::max(mapWidth, width);
                continue;
            }
            if (tile == "PL") {
                ++width;
                m_playerStartGridLocation.x = width;
                m_playerStartGridLocation.y = height;

                m_playerStartLocation = SDL_Rect { m_playerStartGridLocation.x * m_spriteSize  + m_bounds.x, m_playerStartGridLocation.y * m_spriteSize + m_bounds.y, 0,0};

                mapWidth = std::max(mapWidth, width);
                continue;
            }

            //
            //  Get tile name (filename)
            //
            try {
                tileName = json_data["index"][tile]["name"];

            } catch (nlohmann::json::exception& e) {
                ERROR_MSG("Failed to load tile name for tile " << tile << ": " << e.what());

                return false;
            }

            //
            //  Get tile type (filename)
            //
            try {
                tileType = json_data["index"][tile]["type"];

            } catch (nlohmann::json::exception& e) {
                ERROR_MSG("Failed to load tile type for tile " << tile << ": " << e.what());

                return false;
            }

            DEBUG_MSG("Tile Type: " << tileType);

            //
            //  We handle our enemies differently than tiles as they can have different sprites depending on their
            //  state.
            //
            if ("enemy" == tileType) {
                if (!LoadEnemy_(json_data["index"][tile], width, height)) {
                    ERROR_MSG("Failed to load enemy");
                }
                ++width;
                continue;
            }

            if (!sprite->Load(tileName)) {
                ERROR_MSG("Failed to load tile(" << tileName << ")");

                return false;
            }

            //
            //  Add the tiles to what will be rendered.
            //
            m_tiles.push_back(sprite);

            //
            //  Put the tile where it goes
            //
            if ("floor" == tileType) {
                m_floors.push_back(sprite);
            }
            else if ("ladder" == tileType) {
                m_ladders.push_back(sprite);
            }
            else if ("bomb" == tileType) {
                m_bombs.push_back(sprite);
            }
            else if ("treasure" == tileType) {
                m_treasure.push_back(sprite);
            }

            sprite->ScaleToHeight(m_spriteSize);
            sprite->setPosition(width * m_spriteSize + m_bounds.x, height * m_spriteSize + m_bounds.y);

            ++width;
            mapWidth = std::max(mapWidth, width);
        }

        ++height;
    }

    if ((mapWidth != m_mapWidth) || (height != m_mapHeight)) {
        ERROR_MSG("Declared height and width (" <<  m_mapWidth << "x" << m_mapHeight << ") does not match actual (" << mapWidth << "," << height << ")");
        return false;
    }

    return true;
} // LoadMap_

const std::string &PlatformerLevel::GetNextLevel() const {
    return m_nextLevel;
} // GetNextLevel
const std::string& PlatformerLevel::GetName() const {
    return m_levelName;
} //GetName
const std::string& PlatformerLevel::GetFilename() const {
    return m_filename;
} //GetName

bool PlatformerLevel::Load(const std::string &fileName, int maxWidth, int maxHeight, bool scale) {
    Clear();

    m_filename = fileName;

    nlohmann::json json_data;

    std::ifstream f(fileName);

    try {
        json_data = nlohmann::json::parse(f);
    }
    catch(nlohmann::json::exception& e) {
        ERROR_MSG("Failed to load level (" << fileName << "): " << e.what());

        return false;
    }

    try {
        m_levelName = json_data["name"].get<std::string>();
    } catch (nlohmann::json::exception& e) {
        ERROR_MSG("Invalid level name, cannot load level: " << e.what());

        return false;
    }

    try {
        m_nextLevel = json_data["next-level"].get<std::string>();
    } catch (nlohmann::json::exception& e) {
        //
        //  We allow the next level to be missing.
        //
        m_nextLevel = "";
    }

    try {
        m_spriteSize = json_data["sprite-size"].get<int>();
    } catch (nlohmann::json::exception& e) {
        ERROR_MSG("Invalid sprite size, cannot load level: " << e.what());

        return false;
    }

    try {
        m_mapHeight = json_data["map-height"].get<int>();
    } catch (nlohmann::json::exception& e) {
        std::cerr << "Invalid Map Height, cannot load level: " << e.what() << std::endl;

        return false;
    }

    try {
        m_mapWidth = json_data["map-width"].get<int>();
    } catch (nlohmann::json::exception& e) {
        std::cerr << "Invalid Map Height, cannot load level: " << e.what() << std::endl;

        return false;
    }

    //
    //  Once we have loaded the width and height we adjust our configured sprite sizes.
    //
    //  WARNING:
    //      This must be done prior to loading any sprites or tiles for the level or their sizes
    //      could be wrong.
    //
    SetSpriteSize_(maxWidth, maxHeight);

    LoadMap_(json_data, "map");
    LoadMap_(json_data, "map-items");

    return true;
} // Load


bool PlatformerLevel::LoadEnemy_(const nlohmann::json& enemyData, int width, int height) {
    std::map<std::string, std::string> stateMap;

    //
    //  HACK: This should actually be loaded from enemyData but just using the hard-coded values for now.
    //
    stateMap["idle"] = "sprites/dog/idle/sprite.json";
    stateMap["walk-left"] = "sprites/dog/walk-left/sprite.json";
    stateMap["walk-right"] = "sprites/dog/walk-right/sprite.json";
    stateMap["walk-up"] = "sprites/dog/jump/sprite.json";
    stateMap["walk-down"] = "sprites/dog/fall/sprite.json";
    stateMap["fall"] = "sprites/dog/fall/sprite.json";
    stateMap["run-left"] = "sprites/dog/run-left/sprite.json";
    stateMap["run-right"] = "sprites/dog/run-right/sprite.json";
    stateMap["dead"] = "sprites/dog/dead/sprite.json";

    bpl::game::PlatformerEnemyTokenPtr enemy  = std::make_shared<bpl::game::PlatformerEnemyToken>();

    if (!enemy->LoadState(stateMap)) {
        ERROR_MSG("PlayerToken::LoadState(...) failed");

        return false;
    }

    enemy->SetState("idle");
    if (!enemy->SetLadders(GetLadders())) {
        ERROR_MSG("PlayerToken::SetLadders(...) failed");

        return false;
    }
    if (!enemy->SetFloors(GetFloors())) {
        ERROR_MSG("PlayerToken::SetFloors(...) failed");

        return false;
    }
    enemy->SetHeight(GetSpriteSize());
    enemy->SetBounds(GetBounds());

    //
    //  Flip the necessary tokens
    //
    enemy->GetSprite("dead")->setLoop(false);

    DEBUG_MSG("Enemy location: (" << width * m_spriteSize << "," << height * m_spriteSize << ")");
    enemy->SetPosition(width * m_spriteSize + m_bounds.x, height * m_spriteSize + m_bounds.y);

    m_enemies.push_back(enemy);

    return true;
} // LoadEnemy_