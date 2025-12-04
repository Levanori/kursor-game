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

    // Пауза
    if (isPaused) {
        painter.fillRect(0, 0, 800, 600, QColor(0, 0, 0, 128));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 32, QFont::Bold));
        painter.drawText(QRect(0, 250, 800, 50), Qt::AlignCenter, "PAUSED");
        painter.setFont(QFont("Arial", 18));
        painter.drawText(QRect(0, 320, 800, 50), Qt::AlignCenter, "Press ESC to continue");
    }
}

void Game::handleKeyPress(int key)
{
    if (key == Qt::Key_Escape) {
        isPaused = !isPaused;
        qDebug() << "Game Paused:" << isPaused;
        return;
    }

    if (!isPaused && currentLevel) {
        currentLevel->handleKeyPress(key);
    }
}

void Game::handleKeyRelease(int key)
{
    if (!isPaused && currentLevel) {
        currentLevel->handleKeyRelease(key);
    }
}
