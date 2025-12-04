#ifndef GAME_H
#define GAME_H

#include <QKeyEvent>
#include "level.h"

class Game
{
public:
    Game();
    ~Game();

    void initialize();
    void update(double deltaTime);
    void render(QPainter& painter);
    void handleKeyPress(int key);
    void handleKeyRelease(int key);

private:
    Level* currentLevel;
    bool isPaused;
};

#endif // GAME_H
