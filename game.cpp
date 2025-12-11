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
            currentLevel->reset();
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
    screenWidth = w;
    screenHeight = h;
    if (currentLevel) {
        currentLevel->handleResize(w, h);
    }
}

void Game::handleMouseClick(const QPoint &pos)
{
    if (!isPaused) return;

    int w = screenWidth;
    int h = screenHeight;

    int buttonW = 300;
    int buttonH = 50;
    int buttonX = (w - buttonW) / 2;
    int startY = h / 2 - 50;
    int spacing = 80;

    // кнопка "Continue"
    QRect continueRect(buttonX, startY, buttonW, buttonH);
    // кнопка "Restart"
    QRect restartRect(buttonX, startY + spacing, buttonW, buttonH);
    // кнопка "Quit"
    QRect quitRect(buttonX, startY + 2 * spacing, buttonW, buttonH);

    // перевірка, на яку кнопку клікнув користувач
    if (continueRect.contains(pos)) {
        isPaused = false;
        qDebug() << "Game Continued";
    } else if (restartRect.contains(pos)) {
        Level* currentLevel = getCurrentLevel();
        if (currentLevel) {
            currentLevel->reset();
            isPaused = false;
            qDebug() << "Game Restarted";
        }
    } else if (quitRect.contains(pos)) {
        qDebug() << "Quit Requested";
        emit quitRequested();
    }
}

