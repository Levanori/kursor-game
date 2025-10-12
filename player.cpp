#include "player.h"

Player::Player(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 400.0;
}

void Player::update(double deltaTime)
{
    position.setX(position.x() + speed * deltaTime);
    if (position.x() > 1000) { // заміниться на розширення
        position.setX(0);
    }
}

void Player::render(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::green));
    painter.drawRect(QRectF(position, size));
}
