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

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 16));
    QString stateText = isPaused ? "Пауза" : "Продовження";
    painter.drawText(600, 30, stateText);
}

void Game::handleKeyPress(int key) {
    if (key == Qt::Key_Escape) {
        isPaused = !isPaused;
        qDebug() << "Game Paused:" << isPaused;
    }
}
