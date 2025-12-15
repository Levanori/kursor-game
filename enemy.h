#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include <QPixmap>
#include <QVector>

class Enemy : public GameObject
{
public:
    Enemy(QPointF pos, QSizeF s);
    virtual ~Enemy() = default;

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    void setTarget(QPointF* targetPos);

    virtual void takeDamage(int amount);
    int getDamage() const { return contactDamage; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const;
    bool canBeHit() const { return hitCooldown <= 0; }

    QRectF getBounds() const;

    void knockback(QPointF fromPos, double force = 200.0);

    bool isPlayingDeathAnimation() const { return playingDeathAnimation; }
    bool isDeathAnimationComplete() const { return deathAnimComplete; }

protected:
    virtual void updateSprite() = 0;
    virtual void onDeath();
    void playDeathAnimation();

    QPixmap currentSprite;
    QVector<QPixmap> deathAnimationSprites;
    QPointF* targetPosition = nullptr;

    int health = 50;
    int maxHealth = 50;
    int contactDamage = 20;
    float speed = 80.0f;

    QPointF knockbackVelocity;
    double knockbackTimer = 0;

    double hitCooldown = 0;
    double hitCooldownTime = 0.4;

    bool playingDeathAnimation = false;
    bool deathAnimComplete = false;
    double deathAnimTimer = 0;
    int deathAnimFrame = 0;
    double deathFrameDuration = 0.1;
};

#endif
