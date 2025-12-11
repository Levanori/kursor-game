#ifndef GAME_H
#define GAME_H

#include <QKeyEvent>
#include "level.h"

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

    Level* getCurrentLevel() const { return currentLevel; }
    bool isitPaused() const { return isPaused; }

signals:
    void quitRequested();
    void requestWindowResize(QSize newSize);
    void requestFullscreen();

private:
    Level* currentLevel;
    bool isPaused;
    int screenWidth;
    int screenHeight;
};

#endif // GAME_H

