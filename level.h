#ifndef LEVEL_H
#define LEVEL_H

#include "player.h"
#include "enemy.h"
#include <QVector>

class Level
{
public:
    Level();
    ~Level();

    void update(double deltaTime);
    void render(QPainter& painter);

    // Керування гравцем
    void handleKeyPress(int key);
    void handleKeyRelease(int key);

    // Стан гри
    bool isGameOver() const { return gameOver; }
    int getScore() const { return score; }

    void handleResize(int w, int h);
    double getScaleFactor() const;

    int getVirtualWidth() const { return VIRTUAL_WIDTH; }
    int getVirtualHeight() const { return VIRTUAL_HEIGHT; }

    void reset();

private:
    void checkCollisions();
    void spawnEnemy();
    void renderUI(QPainter& painter);

    Player* player;
    QVector<Enemy*> enemies;

    bool gameOver = false;
    int score = 0;

    // Спавн ворогів
    double spawnTimer = 0;
    double spawnInterval = 2.0;

    const int VIRTUAL_WIDTH = 1000;
    const int VIRTUAL_HEIGHT = 600;
    int currentScreenWidth = VIRTUAL_WIDTH;
    int currentScreenHeight = VIRTUAL_HEIGHT;
};

#endif // LEVEL_H
