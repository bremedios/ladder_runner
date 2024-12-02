//
// Created by bradr on 10/28/24.
//

#include <iostream>
#include <fstream>

#include <time.h>
#include <stdlib.h>

#include "LevelFranticLadders.h"

#include "MainMenu.h"
#include <bpl/graphics/collider/Collider.h>
#include <bpl/graphics/collider/Ladder.h>
#include <bpl/graphics/draw/ops/Text.h>
#include <bpl/controls/Input.h>
#include "nlohmann/json.hpp"

#include <bpl/game/Score.h>
#include <bpl/game/GameStateMachine.h>
#include <bpl/sys/Stopwatch.h>

#define DEBUG_PLAYER_MOVEMENT   1

LevelFranticLadders::LevelFranticLadders()
    : bpl::game::GameWindow("resources/ui/frantic_ladders/frantic-ladders.json", "frantic-ladders") {

    m_playerLife.Reset();
} // LevelFranticLadders

bool LevelFranticLadders::LoadPlayer_() {
    std::map<std::string, std::string> stateMap;

    stateMap["idle"] = "sprites/cat/idle/sprite.json";
    stateMap["walk-left"] = "sprites/cat/walk-left/sprite.json";
    stateMap["walk-right"] = "sprites/cat/walk-right/sprite.json";
    stateMap["walk-up"] = "sprites/cat/jump/sprite.json";
    stateMap["walk-down"] = "sprites/cat/fall/sprite.json";
    stateMap["fall"] = "sprites/cat/fall/sprite.json";
    stateMap["run-left"] = "sprites/cat/run-left/sprite.json";
    stateMap["run-right"] = "sprites/cat/run-right/sprite.json";
    stateMap["dead"] = "sprites/cat/dead/sprite.json";

    if (!m_playerToken.LoadState(stateMap)) {
        std::cerr << "PlayerToken::LoadState(...) failed" << std::endl;

        return false;
    }

    m_playerToken.SetState("idle");
    if (!m_playerToken.SetLadders(m_level.GetLadders())) {
        std::cerr << "PlayerToken::SetLadders(...) failed" << std::endl;
    }

    if (!m_playerToken.SetFloors(m_level.GetFloors())) {
        std::cerr << "PlayerToken::SetFloors(...) failed" << std::endl;
    }

    m_playerToken.SetHeight(m_level.GetSpriteSize());
    m_playerToken.SetBounds(m_level.GetBounds());

    //
    //  Flip the necessary tokens
    //
    m_playerToken.GetSprite("dead")->setLoop(false);

    m_playerToken.SetStartPosition(m_level.GetPlayerStartPosition().x, m_level.GetPlayerStartPosition().y);
    m_playerToken.GotoStartPosition();

    return true;
} // LoadPlayer_

bool LevelFranticLadders::Entry() {
    bpl::sys::Stopwatch swGameWindow;

    swGameWindow.Start();
    if (!bpl::game::GameWindow::Entry()) {
        std::cerr << "LevelFranticLadders::Entry: GameWindow::Entry failed" << std::endl;
        return false;
    }
    swGameWindow.Stop();

    int windowWidth = 0;
    int windowHeight = 0;

    bpl::game::GameStateMachine::getInstance()->GetWindowSize(windowWidth, windowHeight);

    bpl::sys::Stopwatch swLevel;

    swLevel.Start();
    //
    //  Load Level
    //
    if (!m_level.Load(m_startingLevel, windowWidth, windowHeight)) {
        std::cerr << "Failed to load level (" << m_startingLevel << ")" << std::endl;

        return false;
    }
    swLevel.Stop();

    bpl::game::Score::getInstance().StartLevel(0);

    bpl::sys::Stopwatch swLoadPlayer;

    swLoadPlayer.Start();
    if (!LoadPlayer_()) {
        std::cerr << "Failed to load player token" << std::endl;

        return false;
    }
    swLoadPlayer.Stop();

    m_startTimer.Start(m_startTimeDuration);

    m_overlay = std::make_shared<bpl::graphics::ui::Window>("resources/ui/frantic_ladders/frantic-ladders-overlay.json");

    if (!m_overlay->Load(bpl::game::GameStateMachine::getInstance()->GetRenderer())) {
        std::cerr << "Failed to load overlay" << std::endl;

        return false;
    }

    std::cout << "GameWindow::Load() Took " << swGameWindow.getMilliseconds() << "ms" << std::endl;
    std::cout << "Level Load() Took " << swLevel.getMilliseconds() << "ms" << std::endl;
    std::cout << "LoadPlayer_() Took " << swLoadPlayer.getMilliseconds() << "ms" << std::endl;

    bpl::game::Score::getInstance().AddPointMap("treasure", 100);
    bpl::game::Score::getInstance().AddPointMap("death", -500);

    return true;
} // Entry

void LevelFranticLadders::SetStartingLevel(const std::string &level) {
    m_startingLevel = level;
} // SetStartingLevel

bool LevelFranticLadders::Exit() {
    bool result = true;

    m_level.Clear();

    if (!bpl::game::GameWindow::Exit()) {
        std::cerr << "LevelFranticLadders::Exit: GameWindow::Exit() failed" << std::endl;

        result = false;
    }

    m_overlay->Destroy();

    return result;
} // Exit

void LevelFranticLadders::Render(bpl::graphics::RendererPtr& renderer) {
    bpl::game::GameWindow::Render(renderer);

    for (auto& it : m_level.GetTiles()) {
        //
        //  A nullptr is a tile that has no data.
        //
        if (nullptr == it.get()) {
            continue;
        }
        it->Draw();

        if (m_enableColliders) {
            it->DrawColliders(255,0,0);
        }
    }

    m_playerToken.GetCurrentSprite()->Draw();

    if (m_enableColliders) {
        m_playerToken.GetCurrentSprite()->DrawCollider(255,0,0);
    }

    for (auto& it : m_level.GetEnemies()) {
        it->GetCurrentSprite()->Draw();

        if (m_enableColliders) {
            it->GetCurrentSprite()->DrawColliders(255,0,0);
        }
    }

    m_overlay->Render(renderer);

    SDL_RenderPresent(renderer->getSdlRenderer());
} // Render

void LevelFranticLadders::UpdatePlayerCollisions_() {
    std::list<bpl::graphics::SpritePtr> removeTreasures;
    std::list<bpl::graphics::SpritePtr> removeBombs;

    //
    //  Treasure collisions
    //
    for (const auto& it : m_level.GetTreasure()) {
        if (bpl::graphics::collider::Collider::IsColliding(it->GetCollider(), m_playerToken.GetCurrentSprite()->GetColliders())) {
            removeTreasures.push_back(it);
        }
    }

    //
    //  Bomb collisions
    //
    for (const auto& it : m_level.GetBombs()) {
        if (bpl::graphics::collider::Collider::IsColliding(it->GetCollider(), m_playerToken.GetCurrentSprite()->GetColliders())) {
            removeBombs.push_back(it);
            m_playerToken.AdjustHealth(-1 * m_playerToken.GetHealth());
        }
    }

    //
    //  Enemy collisions
    //
    for (const auto& it : m_level.GetEnemies()) {
        if (bpl::graphics::collider::Collider::IsColliding(it->GetCurrentSprite()->GetCollider(), m_playerToken.GetCurrentSprite()->GetColliders())) {
            m_playerToken.AdjustHealth(-1 * m_playerToken.GetHealth());
        }
    }

    for (auto& it : removeTreasures) {
        bpl::game::Score::getInstance().Achievement("treasure");
        m_level.RemoveTreasure(it);
    }

    for (auto& it : removeBombs) {
        m_level.RemoveBomb(it);
    }
} // UpdatePlayerCollisions_

void LevelFranticLadders::UpdatePlayerHealthState_() {
    //  player has died
    if (m_playerToken.GetHealth() <= 0) {
        m_playerLife.LoseLife();

        if (m_playerLife.IsGameOver()) {
            m_playerToken.SetState("dead");
        }else {

            //  bring the player back to life
            m_playerToken.SetHealth(m_playerToken.GetMaxHealth());
            m_playerToken.SetState("idle");
            m_playerToken.GotoStartPosition();

            //  start the death timer so the player has a chance to prepare.
            m_deathTimer.Start(m_deathTimeDuration);
        }
    }
} // UpdatePlayerHealthState_

void LevelFranticLadders::UpdateControlInputs_() {
    //
    //  Get joystick input.
    //
    m_inputDown = bpl::controls::Input::getInstance()->KeyDown(bpl::controls::KeyCode::INPUT_DOWN);
    m_inputUp = bpl::controls::Input::getInstance()->KeyDown(bpl::controls::KeyCode::INPUT_UP);
    m_inputLeft = bpl::controls::Input::getInstance()->KeyDown(bpl::controls::KeyCode::INPUT_LEFT);
    m_inputRight = bpl::controls::Input::getInstance()->KeyDown(bpl::controls::KeyCode::INPUT_RIGHT);

    m_run = bpl::controls::Input::getInstance()->KeyDown(bpl::controls::KeyCode::INPUT_BUTTON_R2);

    if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_KEY_W)) {
        std::string nextLevel = m_level.GetNextLevel();

        if (nextLevel.empty()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }

        SetStartingLevel(nextLevel);

        if (!Entry()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }
        m_playerLife.Reset();
    }

    if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_KEY_R)) {
        std::string nextLevel = "resources/levels/level_01.json";

        if (nextLevel.empty()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }

        SetStartingLevel(nextLevel);

        if (!Entry()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }
        m_playerLife.Reset();
    }

    if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_KEY_E)) {
        std::string nextLevel = m_level.GetFilename();

        if (nextLevel.empty()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }

        SetStartingLevel(nextLevel);

        if (!Entry()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        }
        m_playerLife.Reset();
    }
} // UpdateControlInputs_

void LevelFranticLadders::UpdateOverlays_() {
    if (m_playerLife.IsGameOver()) {
        bpl::graphics::draw::ops::OpPtr ptr = m_overlay->getJsonLoader().GetOpMap()["game-over"];

        m_overlay->getJsonLoader().GetOpMap()["resume-in"]->setVisible(false);
        ptr->setVisible(true);

        m_playerToken.SetState("dead");
        return;
    }

    // handle the start and death timers.
    if (!m_startTimer.IsExpired()) {
        bpl::graphics::draw::ops::TextPtr textPtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Text>(m_overlay->getJsonLoader().GetOpMap()["resume-in"]);

        textPtr->setValue(std::to_string(m_startTimer.SecondsLeft()));

        textPtr->setVisible(true);
    }
    else if (!m_deathTimer.IsExpired()) {
        bpl::graphics::draw::ops::TextPtr textPtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Text>(m_overlay->getJsonLoader().GetOpMap()["resume-in"]);

        textPtr->setValue(std::to_string(m_deathTimer.SecondsLeft()));

        textPtr->setVisible(true);
    }
    else {
        m_overlay->getJsonLoader().GetOpMap()["resume-in"]->setVisible(false);
    }

    switch  (m_playerLife.GetLivesCount()) {
        case 0:
            m_overlay->getJsonLoader().GetOpMap()["health-1"]->setVisible(false);
            m_overlay->getJsonLoader().GetOpMap()["health-2"]->setVisible(false);
            m_overlay->getJsonLoader().GetOpMap()["health-3"]->setVisible(false);
        break;
        case 1:
            m_overlay->getJsonLoader().GetOpMap()["health-1"]->setVisible(true);
            m_overlay->getJsonLoader().GetOpMap()["health-2"]->setVisible(false);
            m_overlay->getJsonLoader().GetOpMap()["health-3"]->setVisible(false);
        break;
        case 2:
            m_overlay->getJsonLoader().GetOpMap()["health-1"]->setVisible(true);
            m_overlay->getJsonLoader().GetOpMap()["health-2"]->setVisible(true);
            m_overlay->getJsonLoader().GetOpMap()["health-3"]->setVisible(false);
        break;
        case 3:
            m_overlay->getJsonLoader().GetOpMap()["health-1"]->setVisible(true);
            m_overlay->getJsonLoader().GetOpMap()["health-2"]->setVisible(true);
            m_overlay->getJsonLoader().GetOpMap()["health-3"]->setVisible(true);
        break;

    }

    {
        bpl::graphics::draw::ops::TextPtr textPtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Text>(m_overlay->getJsonLoader().GetOpMap()["level"]);

        textPtr->setValue(m_level.GetName());

        textPtr->setVisible(true);
    }

    {
        bpl::graphics::draw::ops::TextPtr textPtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Text>(m_overlay->getJsonLoader().GetOpMap()["score"]);

        textPtr->setValue(std::to_string(bpl::game::Score::getInstance().getScore()));

        textPtr->setVisible(true);
    }

} // UpdateOverlays_

void LevelFranticLadders::Update() {
    int y=0;
    int x=0;

    //  If the start timer has not expired, we do nothing.
    if (!m_startTimer.IsExpired()) {
        UpdateOverlays_();
        return;
    }

    //  If the game is over, we don't need to do anything but update the overlays.
    if (m_playerLife.IsGameOver()) {
        // We need to handle this.
        UpdateOverlays_();

        return;
    }

    //  If we are in the death timer, nothing to do but put the player back to the start and wait.
    if (!m_deathTimer.IsExpired()) {
        UpdateOverlays_();
        return;
    }

    UpdateControlInputs_();

    //
    //  Do Player movement
    //
    if (m_inputDown && !m_inputUp)
        y = 1;
    else if (!m_inputDown && m_inputUp)
        y = -1;
    if (m_inputLeft && !m_inputRight)
        x = -1;
    else if (!m_inputLeft && m_inputRight)
        x = 1;

    if (!m_playerToken.Move(x, y)) {
        m_playerToken.SetState("idle");
    }

    //
    //  Enemies perform actions, this may or may not result in them moving.
    //
    for (auto it : m_level.GetEnemies()) {
        it->Action();
    }

    UpdatePlayerCollisions_();

    UpdatePlayerHealthState_();

    //  Check for the win condition for the level
    if (m_level.GetTreasure().empty()) {
        std::string nextLevel = m_level.GetNextLevel();

        //  Check for the win condition for the game.
        if (nextLevel.empty()) {
            //
            //  Failed, go to the main menu.
            //
            bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
        } else {
            SetStartingLevel(nextLevel);

            if (!Entry()) {
                //
                //  Failed, go to the main menu.
                //
                bpl::game::GameStateMachine::getInstance()->SetNextState(MainMenu::getInstance());
            }
        }
    }

    UpdateOverlays_();
} // Update

LevelFranticLadders *LevelFranticLadders::getInstance() {
    static LevelFranticLadders instance;

    return &instance;
} // getInstance