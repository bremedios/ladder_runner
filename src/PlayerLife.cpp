//
// Created by Bradley Remedios on 11/15/24.
//
#include <cmath>
#include "PlayerLife.h"

void PlayerLife::Reset() {
    m_currentLives = m_resetLives;
} // Reset

void PlayerLife::LoseLife(int lives) {
    m_currentLives = std::max(-1, m_currentLives - lives);
} // Lose:Life

void PlayerLife::GainLife(int lives) {
    m_currentLives = std::max(m_maxLives, m_currentLives + lives);
} // GainLife

bool PlayerLife::IsGameOver() {
    return m_currentLives < 0;
} // IsGameOver

int PlayerLife::GetLivesCount() {
    return std::max(0, m_currentLives);
} // GetLivesCount
