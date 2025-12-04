#include "enemy.h"
#include <QtMath>
#include <QDebug>

Enemy::Enemy(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 80.0;

    // Завантаження спрайту
    sprite = QPixmap(":/sprites/assets/virus.svg");
    if (sprite.isNull()) {
        qDebug() << "Failed to load virus sprite!";
    }
}

void Enemy::setTarget(QPointF* targetPos)
{
    targetPosition = targetPos;
}

void Enemy::update(double deltaTime)
{
    if (isDead()) return;

    // Оновлення cooldown на отримання шкоди
    if (hitCooldown > 0) {
        hitCooldown -= deltaTime;
    }

    // Обробка knockback
    if (knockbackTimer > 0) {
        knockbackTimer -= deltaTime;
        position += knockbackVelocity * deltaTime;
        knockbackVelocity *= 0.9; // Затухання
        return;
    }

    // Рух до цілі (гравця)
    if (targetPosition) {
        double dx = targetPosition->x() - position.x();
        double dy = targetPosition->y() - position.y();
        double distance = qSqrt(dx * dx + dy * dy);

        if (distance > 5) {
            double dirX = dx / distance;
            double dirY = dy / distance;

            position.setX(position.x() + dirX * speed * deltaTime);
            position.setY(position.y() + dirY * speed * deltaTime);
        }
    }
}

void Enemy::render(QPainter& painter)
{
    if (isDead()) return;

    if (!sprite.isNull()) {
        painter.drawPixmap(position.toPoint(), sprite.scaled(size.toSize()));
    } else {
        // Fallback - червоний квадрат
        painter.setBrush(QBrush(Qt::red));
        painter.drawRect(QRectF(position, size));
    }

    // Health bar
    if (health < maxHealth) {
        double healthPercent = (double)health / maxHealth;
        painter.setBrush(Qt::red);
        painter.drawRect(QRectF(position.x(), position.y() - 8, size.width(), 4));
        painter.setBrush(Qt::green);
        painter.drawRect(QRectF(position.x(), position.y() - 8, size.width() * healthPercent, 4));
    }
}

QRectF Enemy::getBounds() const
{
    return QRectF(position, size);
}

void Enemy::takeDamage(int amount)
{
    if (hitCooldown > 0) return; // Захист від мульти-ударів

    health -= amount;
    hitCooldown = hitCooldownTime;
}

void Enemy::knockback(QPointF fromPos, double force)
{
    double dx = position.x() - fromPos.x();
    double dy = position.y() - fromPos.y();
    double distance = qSqrt(dx * dx + dy * dy);

    if (distance > 0) {
        knockbackVelocity.setX((dx / distance) * force);
        knockbackVelocity.setY((dy / distance) * force);
    } else {
        knockbackVelocity.setX(force);
        knockbackVelocity.setY(0);
    }
    knockbackTimer = 0.3;
}
