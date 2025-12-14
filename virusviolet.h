#ifndef VIRUSVIOLET_H
#define VIRUSVIOLET_H

#include "enemy.h"

// Forward declaration для проджектайлів
class Projectile;

class VirusViolet : public Enemy
{
public:
    VirusViolet(QPointF pos, QSizeF s);

    void update(double deltaTime) override;

    // Перевіряє чи може стріляти і чи треба
    bool canShoot() const;
    QPointF getShootDirection(QPointF targetPos) const;
    void shoot();

    // Налаштування стрільби
    float getShootCooldown() const { return shootCooldown; }
    float getShootTimer() const { return shootTimer; }
    int getHealthCostPerShot() const { return healthCostPerShot; }

protected:
    void updateSprite() override;
    void onDeath() override;

private:
    void loadSprites();

    // Спрайти для різних станів HP
    QPixmap spriteFull;   // Повне HP
    QPixmap sprite8;
    QPixmap sprite5;
    QPixmap sprite4;
    QPixmap sprite3;
    QPixmap sprite2;
    QPixmap sprite1;

    // Параметри стрільби
    float shootCooldown = 2.0f;    // Кулдаун між пострілами (секунди)
    float shootTimer = 0.0f;        // Таймер для кулдауну
    int healthCostPerShot = 5;      // Скільки HP коштує постріл

    // Мінімальна відстань для стрільби
    float minShootDistance = 100.0f;
    float maxShootDistance = 400.0f;
};

#endif // VIRUSVIOLET_H
