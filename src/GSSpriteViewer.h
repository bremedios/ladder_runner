//
// Created by bradr on 10/28/24.
//

#ifndef GSSPRITEVIEWER_H
#define GSSPRITEVIEWER_H

#include <map>

#include <bpl/graphics/Sprite.h>
#include <bpl/game/GameState.h>
#include <bpl/graphics/Renderer.h>
#include <bpl/graphics/Font.h>


class GSSpriteViewer : public bpl::game::GameState{

public:
    static GSSpriteViewer* getInstance();

    virtual ~GSSpriteViewer() = default;

    //
    //  Setup and Teardown routines
    //
    virtual bool Entry();
    virtual bool Exit();

    //
    //  Game handling routines
    //
    virtual void Event(SDL_Event& event);
    virtual void Update();
    void RenderStart(bpl::graphics::RendererPtr& renderer) override;
    virtual void Render(bpl::graphics::RendererPtr& renderer);
    void RenderEnd(bpl::graphics::RendererPtr& renderer) override;
private:
    typedef std::map<std::string,bpl::graphics::SpritePtr> SpriteMap;
    typedef std::list<std::string> SpriteFilenames;

    bpl::graphics::FontPtr m_font;
    SpriteMap m_spriteMap;
    SpriteFilenames m_spriteFilenames;

    SpriteFilenames::iterator m_currentSprite;

    GSSpriteViewer() = default;
};



#endif //GSSPRITEVIEWER_H