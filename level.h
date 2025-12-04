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

    // Розміри екрану
    int screenWidth = 800;
    int screenHeight = 600;
};

#endif // LEVEL_H
