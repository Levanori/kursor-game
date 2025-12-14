#include "enemy.h"
#include <QtMath>
#include <QDebug>

Enemy::Enemy(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 80.0f;
}

void Enemy::setTarget(QPointF* targetPos)
{
    targetPosition = targetPos;
}

void Enemy::update(double deltaTime)
{
    // Анімація смерті
    if (playingDeathAnimation) {
        deathAnimTimer += deltaTime;
        if (deathAnimTimer >= deathFrameDuration) {
            deathAnimTimer = 0;
            deathAnimFrame++;
            if (deathAnimFrame >= deathAnimationSprites.size()) {
                deathAnimComplete = true;
            } else {
                currentSprite = deathAnimationSprites[deathAnimFrame];
            }
        }
        return;
    }

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
    if (deathAnimComplete) return;

    if (!currentSprite.isNull()) {
        painter.drawPixmap(position.toPoint(), currentSprite.scaled(size.toSize()));
    } else {
        // Fallback - червоний квадрат
        painter.setBrush(QBrush(Qt::red));
        painter.drawRect(QRectF(position, size));
    }

    // Health bar (тільки якщо живий і не повне HP)
    if (!isDead() && health < maxHealth && !playingDeathAnimation) {
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

bool Enemy::isDead() const
{
    return health <= 0 && deathAnimComplete;
}

void Enemy::takeDamage(int amount)
{
    if (hitCooldown > 0 || playingDeathAnimation) return;

    health -= amount;
    hitCooldown = hitCooldownTime;

    if (health <= 0) {
        onDeath();
    } else {
        updateSprite();
    }
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

void Enemy::playDeathAnimation()
{
    if (deathAnimationSprites.isEmpty()) {
        deathAnimComplete = true;
        return;
    }

    playingDeathAnimation = true;
    deathAnimFrame = 0;
    deathAnimTimer = 0;
    currentSprite = deathAnimationSprites[0];
}

void Enemy::onDeath()
{
    // Запускаємо анімацію смерті
    playDeathAnimation();
}
