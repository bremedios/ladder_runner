//
// Created by Bradley Remedios on 11/15/24.
//

#ifndef PLAYERLIFE_H
#define PLAYERLIFE_H

class PlayerLife {
public:
    PlayerLife() = default;
    ~PlayerLife() = default;

    void Reset();
    void LoseLife(int lives=1);
    void GainLife(int lives=1);
    bool IsGameOver();
    int GetLivesCount();
private:
    int m_maxLives = 3;
    int m_resetLives = 3;
    int m_currentLives = 0;
}; // PlayerLife

#endif //PLAYERLIFE_H
