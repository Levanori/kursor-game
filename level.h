#ifndef LEVEL_H
#define LEVEL_H

#include "player.h"
#include "enemy.h"
#include "virusgreen.h"
#include "virusviolet.h"
#include "projectile.h"
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

    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY);

    void clearPlayerKeys() { player->clearKeys(); }
private:
    void checkCollisions();
    void spawnEnemy();
    void updateProjectiles(double deltaTime);
    void spawnProjectile(VirusViolet* shooter);

    Player* player;
    QVector<Enemy*> enemies;
    QVector<Projectile*> projectiles;

    const double LOAD_PER_SECOND = 2.0;
    const int LOAD_PER_KILL = 5;
    double accumulatedLoad = 0.0; // щоб враховувалося автоматичне навантаження
    bool gameOver = false;
    int score = 0;

    // Спавн ворогів
    double spawnTimer = 0;
    double spawnInterval = 2.0;


    const int VIRTUAL_WIDTH = 725;
    const int VIRTUAL_HEIGHT = 450;
    int currentScreenWidth = VIRTUAL_WIDTH;
    int currentScreenHeight = VIRTUAL_HEIGHT;
};

#endif // LEVEL_H
