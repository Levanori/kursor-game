#ifndef ENEMY_H
#define ENEMY_H

#include "gameobject.h"
#include <QPixmap>

class Enemy : public GameObject
{
public:
    Enemy(QPointF pos, QSizeF s);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    // Встановлення цілі для переслідування
    void setTarget(QPointF* targetPos);

    // Бойова система
    void takeDamage(int amount);
    int getDamage() const { return contactDamage; }
    int getHealth() const { return health; }
    bool isDead() const { return health <= 0; }
    bool canBeHit() const { return hitCooldown <= 0; }

    // Колізії
    QRectF getBounds() const;

    // Knockback
    void knockback(QPointF fromPos, double force = 200.0);

private:
    QPixmap sprite;
    QPointF* targetPosition = nullptr;

    // Характеристики
    int health = 50;
    int maxHealth = 50;
    int contactDamage = 20;

    // Knockback
    QPointF knockbackVelocity;
    double knockbackTimer = 0;

    // Захист від мульти-ударів
    double hitCooldown = 0;
    double hitCooldownTime = 0.4; // мінімальний час між ударами
};

#endif // ENEMY_H
