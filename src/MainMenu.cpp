//
// Created by Bradley Remedios on 10/28/24.
//

#include <iostream>

#include "MainMenu.h"

#include "LevelFranticLadders.h"
#include "GSSpriteViewer.h"
#include <bpl/graphics/draw/ops/Image.h>
#include <bpl/graphics/draw/ops/Text.h>
#include <bpl/game/GameStateMachine.h>
#include <bpl/controls/Input.h>

#define FONT_INTRO "fonts/Roboto-Regular.ttf"

MainMenu::MainMenu()
    : GameWindow("resources/ui/main-menu.json", "main-menu")
{

}

void MainMenu::Update() {
    //
    //  Handle Selection of Key up / down
    //
    if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_DOWN)) {
        switch (m_option) {
            case Option::Play:
                m_option = Option::Quit;
            break;
            case Option::Quit:
                m_option = Option::Play;
            break;
        }
    } else if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_UP)) {
        switch (m_option) {
            case Option::Play:
                m_option = Option::Quit;
            break;
            case Option::Quit:
                m_option = Option::Play;
            break;
        }
    } else if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_RIGHT) || bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_ENTER)){
        switch (m_option) {
            case Option::Play:
                LevelFranticLadders::getInstance()->SetStartingLevel("resources/levels/level_01.json");
                bpl::game::GameStateMachine::getInstance()->SetNextState(LevelFranticLadders::getInstance());
            break;
            case Option::Quit:
                bpl::game::GameStateMachine::getInstance()->Terminate();
            break;
            default:
                break;
        }
    } else if (bpl::controls::Input::getInstance()->KeyPressed(bpl::controls::KeyCode::INPUT_ESCAPE)) {
        bpl::game::GameStateMachine::getInstance()->Terminate();
    }

    bpl::controls::Input::getInstance()->ClearPressedKeys();
    //
    //  Update graphic positions
    //
    switch(m_option) {
        case Option::Play:
            SetSelection("play");
        break;
        case Option::Quit:
            SetSelection("quit");
        break;
    }
} // Update

void MainMenu::SetSelection(const std::string& selection) {
    if (m_window->getJsonLoader().GetOpMap().contains(selection)) {
        bpl::graphics::draw::ops::OpPtr ptr = m_window->getJsonLoader().GetOpMap()[selection];

        bpl::graphics::draw::ops::TextPtr textPtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Text>(ptr);

        SDL_Rect pos = textPtr->getRegion();

        int x = 0;
        int y = 0;

        ptr = m_window->getJsonLoader().GetOpMap()["ladder-selection"];
        bpl::graphics::draw::ops::ImagePtr imagePtr = std::dynamic_pointer_cast<bpl::graphics::draw::ops::Image>(ptr);

        SDL_Rect imageDest = imagePtr->getDestination();
        SDL_Rect textDest = textPtr->getRegion();

        if (imageDest.h > textDest.h) {
            imageDest.y = textDest.y - (imageDest.h - textDest.h) / 2;
        }
        else {
            imageDest.y = textDest.y + (textDest.h - imageDest.h) / 2;
        }

        imagePtr->setDestination(imageDest);
    }
    else {
        std::cerr << "No play element found" << std::endl;
    }
} // SetSelection

MainMenu *MainMenu::getInstance() {
    static MainMenu instance;

    return &instance;
} // getInstance