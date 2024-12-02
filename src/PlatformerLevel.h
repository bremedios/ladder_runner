//
// Created by Bradley Remedios on 11/7/24.
//

#ifndef PLATFORMERLEVEL_H
#define PLATFORMERLEVEL_H
#include <memory>

#include "nlohmann/json.hpp"

#include <bpl/graphics/Sprite.h>
#include <bpl/game/PlatformerEnemyToken.h>

class PlatformerLevel {
public:
    PlatformerLevel();
    virtual ~PlatformerLevel();

    bool Load(const std::string& fileName, int maxWidth, int maxHeight,bool scale=true);
    void Clear();

    const bpl::graphics::SpriteList& GetBombs() const;
    const bpl::graphics::SpriteList& GetTreasure() const;
    const bpl::graphics::SpriteList& GetLadders() const;
    const bpl::graphics::SpriteList& GetFloors() const;
    const bpl::graphics::SpriteList& GetTiles() const;
    bpl::game::PlatformerEnemyTokenList& GetEnemies();
    const std::string& GetNextLevel() const;
    const std::string& GetName() const;
    const std::string& GetFilename() const;

    const SDL_Rect& GetPlayerStartPosition() const;
    int GetSpriteSize() const;
    const SDL_Rect& GetBounds() const;

    bool RemoveBomb(bpl::graphics::SpritePtr bomb);
    bool RemoveTreasure(bpl::graphics::SpritePtr treasure);
private:

    //
    //  Level data
    //
    std::string m_levelName;
    std::string m_nextLevel;
    std::string m_filename;

    int m_spriteSize = 0;
    int m_mapHeight = 0;
    int m_mapWidth = 0;

    SDL_Rect m_playerStartGridLocation;
    SDL_Rect m_playerStartLocation;
    SDL_Rect m_bounds;

    bpl::graphics::SpriteList m_tiles;
    bpl::graphics::SpriteList m_bombs;
    bpl::graphics::SpriteList m_treasure;
    bpl::graphics::SpriteList  m_ladders;
    bpl::graphics::SpriteList m_floors;
    bpl::game::PlatformerEnemyTokenList m_enemies;

    void SetSpriteSize_(int maxDisplayWidth, int maxDisplayHeight);

    bool LoadMap_(const nlohmann::json& json_data, const std::string& map_name);
    bool LoadEnemy_(const nlohmann::json& enemyData, int width, int height);
};

typedef std::shared_ptr<PlatformerLevel> PlatformerLevelPtr;


#endif //PLATFORMERLEVEL_H