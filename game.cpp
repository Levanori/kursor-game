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
        clearPlayerKeys();
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
    int spacing = 80;

    int leftColX = (w / 2) - buttonW - 20;
    int rightColX = (w / 2) + 20; // Права колонка
    int startY = h / 2 - 50;
    // кнопка "Continue"
    QRect continueRect(leftColX, startY, buttonW, buttonH);
    // кнопка "Restart"
    QRect restartRect(leftColX, startY + spacing, buttonW, buttonH);
    // кнопка "Quit"
    QRect quitRect(leftColX, startY + 2 * spacing, buttonW, buttonH);
    // 725x450
    QRect scale725Rect(rightColX, startY, buttonW, buttonH);
    // 1450x900
    QRect scale1450Rect(rightColX, startY + spacing, buttonW, buttonH);
    // Fullscreen
    QRect scaleFullRect(rightColX, startY + 2 * spacing, buttonW, buttonH);

    Level* currentLevel = getCurrentLevel();
    // перевірка, на яку кнопку клікнув користувач
    if (continueRect.contains(pos)) {
        isPaused = false;
        clearPlayerKeys();
        qDebug() << "Game Continued";
    } else if (restartRect.contains(pos)) {
        if (currentLevel) {
            currentLevel->reset();
            isPaused = false;
            clearPlayerKeys();
            qDebug() << "Game Restarted";
        }
    } else if (quitRect.contains(pos)) {
        qDebug() << "Quit Requested";
        emit quitRequested();
    } else if (scale725Rect.contains(pos)) {
        emit requestWindowResize(QSize(725, 450));
        isPaused = false;
        clearPlayerKeys();
        qDebug() << "Resolution set to 725x450";
    } else if (scale1450Rect.contains(pos)) {
        emit requestWindowResize(QSize(1450, 900));
        isPaused = false;
        clearPlayerKeys();
        qDebug() << "Resolution set to 1450x900";
    } else if (scaleFullRect.contains(pos)) {
        emit requestFullscreen();
        isPaused = false;
        clearPlayerKeys();
        qDebug() << "Fullscreen requested";
    }
}

void Game::clearPlayerKeys()
{
    if (currentLevel) {
        currentLevel->clearPlayerKeys();
    }
}

