#include "game.h"
#include <QDebug>

Game::Game() : isPaused(false), screenWidth(725), screenHeight(450)
{

}

Game::~Game()
{
    delete gameManager;
}

void Game::initialize()
{
    gameManager = new GameManager();
    gameManager->initialize();
}

void Game::update(double deltaTime)
{
    if (isPaused) {
        return;
    }

    if (gameManager) {
        gameManager->update(deltaTime);
    }
}

void Game::render(QPainter& painter)
{
    if (gameManager) {
        gameManager->render(painter);
    }
}

void Game::handleKeyPress(int key)
{
    if (key == Qt::Key_Escape) {
        GameScene* currentScene = getCurrentScene();
        if (currentScene && currentScene->isGameOver()) {
            gameManager->reset();
            return;
        }
        isPaused = !isPaused;
        clearPlayerKeys();
        qDebug() << "Game Paused:" << isPaused;
        return;
    }

    if (isPaused) {
        return;
    }
    if (gameManager) {
        gameManager->handleKeyPress(key);
    }
}

void Game::handleKeyRelease(int key)
{
    if (!isPaused && gameManager) {
        gameManager->handleKeyRelease(key);
    }
}

void Game::handleResize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    if (gameManager) {
        gameManager->handleResize(w, h);
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
    int rightColX = (w / 2) + 20;
    int startY = h / 2 - 50;

    QRect continueRect(leftColX, startY, buttonW, buttonH);

    QRect restartRect(leftColX, startY + spacing, buttonW, buttonH);

    QRect quitRect(leftColX, startY + 2 * spacing, buttonW, buttonH);

    QRect scale725Rect(rightColX, startY, buttonW, buttonH);

    QRect scale1450Rect(rightColX, startY + spacing, buttonW, buttonH);

    QRect scaleFullRect(rightColX, startY + 2 * spacing, buttonW, buttonH);

    if (continueRect.contains(pos)) {
        isPaused = false;
        clearPlayerKeys();
        qDebug() << "Game Continued";
    } else if (restartRect.contains(pos)) {
        if (gameManager) {
            gameManager->reset();
            isPaused = false;
            clearPlayerKeys();
            qDebug() << "Game Restarted";
        }
    } else if (quitRect.contains(pos)) {
        qDebug() << "Quit Requested";
        emit quitRequested();
    } else if (scale725Rect.contains(pos)) {
        emit requestWindowResize(QSize(725, 450));
        qDebug() << "Resolution set to 725x450";
    } else if (scale1450Rect.contains(pos)) {
        emit requestWindowResize(QSize(1450, 900));
        qDebug() << "Resolution set to 1450x900";
    } else if (scaleFullRect.contains(pos)) {
        emit requestFullscreen();
        qDebug() << "Fullscreen requested";
    }
}

void Game::clearPlayerKeys()
{
    if (gameManager) {
        gameManager->clearPlayerKeys();
    }
}

