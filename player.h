#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include <QPixmap>
#include <QVector>
#include <QSet>

class Player : public GameObject
{
public:
    Player(QPointF pos, QSizeF s);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    void setKeyPressed(int key, bool pressed);

    void takeDamage(int amount);
    int getCpuLoad() { return cpuLoad; }
    int getMaxCpuLoad() { return maxCpuLoad; }
    void setCpuLoad(int value);
    bool isCrashed() { return cpuLoad >= maxCpuLoad; }
    void reset();
    void increaseLoad(int amount);
    void decreaseLoad(int amount);

    bool isInvincible() { return invincibilityTimer > 0; }

    void attack();
    bool isAttacking() const { return attacking; }
    QRectF getAttackBounds() const;

    QRectF getBounds() const;

    QPointF* getPositionPtr() { return &position; }

    void setPosition(const QPointF& newPos) { position = newPos; }

    void clearKeys();
private:
    void updateSprite();

    QPixmap sprite;
    QVector<QPixmap> cursorSprites;
    QSet<int> pressedKeys;

    int cpuLoad = 0;
    int maxCpuLoad = 100;
    double invincibilityTimer = 0;
    double invincibilityDuration = 1.0;

    bool attacking = false;
    double attackTimer = 0;
    double attackDuration = 0.2;
    double attackCooldown = 0;
    double attackCooldownTime = 0.5;
};

#endif

