#ifndef VIRUSVIOLET_H
#define VIRUSVIOLET_H

#include "enemy.h"

class Projectile;

class VirusViolet : public Enemy
{
public:
    VirusViolet(QPointF pos, QSizeF s);

    void update(double deltaTime) override;

    bool canShoot() const;
    QPointF getShootDirection(QPointF targetPos) const;
    void shoot();

    float getShootCooldown() const { return shootCooldown; }
    float getShootTimer() const { return shootTimer; }
    int getHealthCostPerShot() const { return healthCostPerShot; }

protected:
    void updateSprite() override;
    void onDeath() override;

private:
    void loadSprites();

    QPixmap spriteFull;
    QPixmap sprite8;
    QPixmap sprite5;
    QPixmap sprite4;
    QPixmap sprite3;
    QPixmap sprite2;
    QPixmap sprite1;

    float shootCooldown = 2.0f;
    float shootTimer = 0.0f;
    int healthCostPerShot = 5;

    float minShootDistance = 100.0f;
    float maxShootDistance = 400.0f;
};

#endif
