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

    // Встановлення цілі для переслідування
    void setTarget(QPointF* targetPos);

    // Бойова система
    virtual void takeDamage(int amount);
    int getDamage() const { return contactDamage; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const;
    bool canBeHit() const { return hitCooldown <= 0; }

    // Колізії
    QRectF getBounds() const;

    // Knockback
    void knockback(QPointF fromPos, double force = 200.0);

    // Для анімації смерті
    bool isPlayingDeathAnimation() const { return playingDeathAnimation; }
    bool isDeathAnimationComplete() const { return deathAnimComplete; }

protected:
    virtual void updateSprite() = 0;  // Чиста віртуальна - кожен підклас реалізує свою
    virtual void onDeath();
    void playDeathAnimation();

    QPixmap currentSprite;
    QVector<QPixmap> deathAnimationSprites;  // Спрайти для анімації смерті
    QPointF* targetPosition = nullptr;

    // Характеристики
    int health = 50;
    int maxHealth = 50;
    int contactDamage = 20;
    float speed = 80.0f;

    // Knockback
    QPointF knockbackVelocity;
    double knockbackTimer = 0;

    // Захист від мульти-ударів
    double hitCooldown = 0;
    double hitCooldownTime = 0.4;

    // Анімація смерті
    bool playingDeathAnimation = false;
    bool deathAnimComplete = false;
    double deathAnimTimer = 0;
    int deathAnimFrame = 0;
    double deathFrameDuration = 0.1;  // Тривалість кожного кадру анімації
};

#endif // ENEMY_H
