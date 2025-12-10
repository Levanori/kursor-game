#include "game.h"
#include <QDebug>

Game::Game() : currentLevel(nullptr), isPaused(false)
{

}

Game::~Game()
{
    delete currentLevel;
}

void Game::initialize()
{
    currentLevel = new Level();
}

void Game::update(double deltaTime)
{
    if (isPaused) {
        return;
    }

    if (currentLevel) {
        currentLevel->update(deltaTime);
    }
}

void Game::render(QPainter& painter)
{
    if (currentLevel) {
        currentLevel->render(painter);
    }
}

void Game::handleKeyPress(int key)
{
    if (key == Qt::Key_Escape) {
        Level* currentLevel = getCurrentLevel();
        if (currentLevel && currentLevel->isGameOver()) {
            // код для рестарту
            return;
        }
        isPaused = !isPaused;
        qDebug() << "Game Paused:" << isPaused;
        return;
    }

    if (currentLevel) {
        currentLevel->handleKeyPress(key);
    }
}

void Game::handleKeyRelease(int key)
{
    if (!isPaused && currentLevel) {
        currentLevel->handleKeyRelease(key);
    }
}

void Game::handleResize(int w, int h)
{
    if (currentLevel) {
        currentLevel->handleResize(w, h);
    }
}
