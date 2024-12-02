//
// Created by Bradley Remedios on 10/28/24.
//

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <bpl/graphics/Sprite.h>
#include <bpl/graphics/Renderer.h>
#include <bpl/graphics/Font.h>

#include <bpl/graphics/ui/Window.h>

#include <bpl/game/GameWindow.h>

class MainMenu  : public bpl::game::GameWindow{
public:
    static MainMenu* getInstance();

    ~MainMenu() override = default;

    void SetSelection(const std::string& selection);

    void Update() override;
private:
    enum Option {
        Play,
        PlayerTest,
        ShowSprites,
        Quit
    };

    Option m_option = Play;

    MainMenu();
};



#endif //MAIN_MENU_H