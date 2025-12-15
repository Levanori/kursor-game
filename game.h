#ifndef GAME_H
#define GAME_H

#include <QKeyEvent>
#include "gamemanager.h"

class Game : public QObject
{
    Q_OBJECT
public:
    Game();
    ~Game();

    void initialize();
    void update(double deltaTime);
    void render(QPainter& painter);
    void handleKeyPress(int key);
    void handleKeyRelease(int key);

    void handleResize(int w, int h);
    void handleMouseClick(const QPoint& pos);

    GameManager* getGameManager() const { return gameManager; }
    GameScene* getCurrentScene() const { return gameManager ? gameManager->getCurrentScene() : nullptr; }
    bool isitPaused() const { return isPaused; }
    int getScreenWidth() const { return screenWidth; }
    int getScreenHeight() const { return screenHeight; }

    void clearPlayerKeys();
signals:
    void quitRequested();
    void requestWindowResize(QSize newSize);
    void requestFullscreen();
    void restartRequested();

private:
    GameManager* gameManager;
    bool isPaused;
    int screenWidth;
    int screenHeight;
};

#endif

