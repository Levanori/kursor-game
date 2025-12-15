#include "projectile.h"
#include <QPainter>

Projectile::Projectile(QPointF pos, QPointF dir, double spd)
    : direction(dir)
    , speed(spd)
{
    position = pos;
    size = QSizeF(16, 16);
    sprite = QPixmap(":/sprites/assets/virus_violet/bombshell.png");

    if (!sprite.isNull()) {
        sprite = sprite.scaled(size.toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void Projectile::update(double deltaTime)
{
    if (!active) return;

    position += direction * speed * deltaTime;

    currentLifetime += deltaTime;
    if (currentLifetime >= lifetime) {
        active = false;
    }
}

void Projectile::render(QPainter &painter)
{
    if (!active) return;

    if (!sprite.isNull()) {
        painter.drawPixmap(position.toPoint(), sprite);
    } else {

        painter.setBrush(Qt::magenta);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(position.x(), position.y(), size.width(), size.height());
    }
}

bool Projectile::isOutOfBounds(QSizeF screenSize) const
{
    return position.x() < -size.width() ||
           position.y() < -size.height() ||
           position.x() > screenSize.width() + size.width() ||
           position.y() > screenSize.height() + size.height();
}
