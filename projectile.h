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

    // Скільки шкоди наносить снаряд
    int getDamage() const { return damage; }

    // Чи вийшов за межі екрану
    bool isOutOfBounds(QSizeF screenSize) const;

    // Чи активний снаряд
    bool isActive() const { return active; }
    void deactivate() { active = false; }

private:
    QPixmap sprite;
    QPointF direction;
    double speed;
    int damage = 10;
    bool active = true;

    // Час життя снаряду (щоб не літали вічно)
    double lifetime = 5.0;
    double currentLifetime = 0.0;
};

#endif // PROJECTILE_H
