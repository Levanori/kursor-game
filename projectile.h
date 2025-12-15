#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "gameobject.h"
#include <QPixmap>

class Projectile : public GameObject
{
public:
    Projectile(QPointF pos, QPointF direction, double speed = 200.0);

    void update(double deltaTime) override;
    void render(QPainter &painter) override;

    int getDamage() const { return damage; }

    bool isOutOfBounds(QSizeF screenSize) const;

    bool isActive() const { return active; }
    void deactivate() { active = false; }

private:
    QPixmap sprite;
    QPointF direction;
    double speed;
    int damage = 10;
    bool active = true;

    double lifetime = 5.0;
    double currentLifetime = 0.0;
};

#endif
