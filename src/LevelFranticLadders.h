//
// Created by Bradley Remedios on 10/28/24.
//

#ifndef LEVEL_FRANTIC_LADDERS_H
#define LEVEL_FRANTIC_LADDERS_H
#include "nlohmann/json.hpp"

#include <bpl/game/GameWindow.h>
#include <bpl/game/PlatformerToken.h>
#include <bpl/game/Timer.h>

#include "PlatformerLevel.h"
#include "PlayerLife.h"

class LevelFranticLadders : public bpl::game::GameWindow{
public:
    static LevelFranticLadders* getInstance();

    LevelFranticLadders();
    virtual ~LevelFranticLadders() = default;

    //
    //  Setup and Teardown routines
    //
    virtual bool Entry();
    virtual bool Exit();

    virtual void SetStartingLevel(const std::string& level);
    //
    //  Game handling routines
    //
    virtual void Update();
    virtual void Render(bpl::graphics::RendererPtr& renderer);

private:
    void UpdateControlInputs_();
    void UpdatePlayerMovement_();
    void UpdatePlayerCollisions_();
    void UpdatePlayerHealthState_();
    void UpdateOverlays_();

    bool LoadPlayer_();

    std::string m_startingLevel = "resources/levels/cnd_level1.json";

    PlatformerLevel m_level;

    bpl::game::PlatformerToken m_playerToken;

    //int m_runSpeed = 20;
    //int m_walkSpeed = 10;

    //
    //  Input states
    //
    bool m_inputDown=false;
    bool m_inputUp = false;
    bool m_inputLeft = false;
    bool m_inputRight = false;
    bool m_inputLshift = false;
    bool m_inputRshift = false;
    bool m_run = false;
    PlayerLife m_playerLife;
    int m_startTimeDuration = 5000;
    int m_deathTimeDuration = 3000;
    bpl::game::Timer m_startTimer;
    bpl::game::Timer m_deathTimer;
    bpl::graphics::ui::WindowPtr m_overlay;

    bool    m_enableColliders=false;
};

#endif //LEVEL_FRANTIC_LADDERS_H