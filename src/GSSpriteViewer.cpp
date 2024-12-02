//
// Created by bradr on 10/28/24.
//

#include <iostream>

#include <bpl/game/GameStateMachine.h>

#include "GSSpriteViewer.h"

bool GSSpriteViewer::Entry() {
    std::cout << "Initializing Font" << std::endl;
    m_font = bpl::graphics::FontPtr(new bpl::graphics::Font("resources/fonts/Roboto-Regular.ttf", 32));

    if (!m_font->Load()) {
        std::cerr << "Failed to load font" << std::endl;

        return false;
    }
/*
    m_spriteFilenames.push_back("sprites/dog/idle/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/walk/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/run/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/slide/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/jump/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/fall/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/hurt/sprite.json");
    m_spriteFilenames.push_back("sprites/dog/dead/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/idle/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/walk/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/run/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/slide/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/jump/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/fall/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/hurt/sprite.json");
    m_spriteFilenames.push_back("sprites/cat/dead/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/bone_1/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/bone_2/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/bone_3/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/bone_4/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_1/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_2/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_3/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_4/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_5/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_6/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_7/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_8/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_9/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_10/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_11/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_12/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_13/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_14/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_15/sprite.json");
    m_spriteFilenames.push_back("sprites/graveyardtiles/floor_16/sprite.json");*/
    m_spriteFilenames.push_back("sprites/ladder/sprite.json");
    m_spriteFilenames.push_back("sprites/bomb/sprite.json");
    m_spriteFilenames.push_back("sprites/yarn/sprite.json");

    //
    //  Load all sprites
    //
    for (auto it = m_spriteFilenames.begin(); it != m_spriteFilenames.end(); it++) {
        bpl::graphics::SpritePtr sprite = bpl::graphics::SpritePtr(new bpl::graphics::Sprite(bpl::game::GameStateMachine::getInstance()->GetRenderer(), bpl::game::GameStateMachine::getInstance()->GetFramerate()));
        sprite->Load(*it);
        sprite->Scale(1);
        sprite->setPosition(5,5);
        sprite->setFramerate(3);

        m_spriteMap[*it] = sprite;
    }

    m_currentSprite = m_spriteFilenames.begin();

    return true;
} // Entry

bool GSSpriteViewer::Exit() {
    m_spriteFilenames.clear();
    m_spriteMap.clear();

    return true;
}

void GSSpriteViewer::RenderStart(bpl::graphics::RendererPtr& renderer) {
    SDL_RenderClear(renderer->getSdlRenderer());

}

void GSSpriteViewer::RenderEnd(bpl::graphics::RendererPtr& renderer) {
    SDL_RenderPresent(renderer->getSdlRenderer());
}

void GSSpriteViewer::Render(bpl::graphics::RendererPtr& renderer) {

    if (0 > SDL_SetRenderDrawColor(renderer->getSdlRenderer(), 50, 0, 50, 0)) {
        std::cerr << "SDL_SetRenderDrawColor failed.  SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    //
    if (0 > SDL_RenderFillRect(renderer->getSdlRenderer(), nullptr)) {
        std::cerr << "SDL_RenderFillRect failed.  SDL_ERROR:" << SDL_GetError() << std::endl;
    }

    SDL_Rect rectSpriteName = {1300, 5, 0,0};
    SDL_Color color = {255, 255, 255, 255};


    if (!m_font->Render(renderer, color, m_spriteMap[*m_currentSprite]->getName(), rectSpriteName, bpl::graphics::Font::Alignment::Left))
    {
        std::cerr << "Render failed." << std::endl;
    }

    //
    //  Fill the image region to show where the graphic actually is
    //
    SDL_Rect spriteImage = {5,5,m_spriteMap[*m_currentSprite]->getWidth(),m_spriteMap[*m_currentSprite]->getHeight()};

    if (0 > SDL_SetRenderDrawColor(renderer->getSdlRenderer(), 255, 255, 255, 0)) {
        std::cerr << "SDL_SetRenderDrawColor failed.  SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    //
    if (0 > SDL_RenderFillRect(renderer->getSdlRenderer(), &spriteImage)) {
        std::cerr << "SDL_RenderFillRect failed.  SDL_ERROR:" << SDL_GetError() << std::endl;
    }

    if (0 > SDL_SetRenderDrawColor(renderer->getSdlRenderer(), 0, 0, 0, 0)) {
        std::cerr << "SDL_SetRenderDrawColor failed.  SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    SDL_Rect floorRect = {spriteImage.x, m_spriteMap[*m_currentSprite]->getFloor(), spriteImage.w, 1};

    if (0 > SDL_RenderFillRect(renderer->getSdlRenderer(), &floorRect)) {
        std::cerr << "SDL_RenderFillRect failed.  SDL_ERROR:" << SDL_GetError() << std::endl;
    }

    m_spriteMap[*m_currentSprite]->Draw();
    m_spriteMap[*m_currentSprite]->DrawColliders(255,0,0);
    m_spriteMap[*m_currentSprite]->DrawCollider(0,0,255);

}

void GSSpriteViewer::Update() {
    // Nothing to do
}

GSSpriteViewer *GSSpriteViewer::getInstance() {
    static GSSpriteViewer instance;

    return &instance;
}

void GSSpriteViewer::Event(SDL_Event &event) {
    if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                case SDLK_q:

                    bpl::game::GameStateMachine::getInstance()->Terminate();
            break;
            case SDLK_RIGHT:
                // move to next sprite
                ++m_currentSprite;

                if (m_currentSprite == m_spriteFilenames.end()) {
                    m_currentSprite = m_spriteFilenames.begin();
                }
            break;
            case SDLK_LEFT:
                // move to next sprite
                --m_currentSprite;

                if (m_currentSprite == m_spriteFilenames.end()) {
                    m_currentSprite = m_spriteFilenames.begin();
                }
            break;
            case SDLK_r:
                // reload sprite
                bpl::graphics::SpritePtr sprite = bpl::graphics::SpritePtr(new bpl::graphics::Sprite(bpl::game::GameStateMachine::getInstance()->GetRenderer(), bpl::game::GameStateMachine::getInstance()->GetFramerate()));
                sprite->Load(*m_currentSprite);
                sprite->Scale(1);
                sprite->setPosition(5,5);
                sprite->setFramerate(3);

                m_spriteMap[*m_currentSprite] = sprite;
                break;
        }
    }
}