#include "player.h"
#include <QKeyEvent>
#include <QtMath>

Player::Player(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 300.0;

    // Завантаження спрайту
    sprite = QPixmap(":/sprites/assets/cursor.svg");
    if (sprite.isNull()) {
        qDebug() << "Failed to load cursor sprite!";
    }
}

void Player::update(double deltaTime)
{
    // Оновлення таймера невразливості
    if (invincibilityTimer > 0) {
        invincibilityTimer -= deltaTime;
    }

    // Оновлення атаки
    if (attacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0) {
            attacking = false;
        }
    }

    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
    }

    // Керування рухом
    double velocityX = 0;
    double velocityY = 0;

    if (pressedKeys.contains(Qt::Key_W) || pressedKeys.contains(Qt::Key_Up))
        velocityY = -speed;
    if (pressedKeys.contains(Qt::Key_S) || pressedKeys.contains(Qt::Key_Down))
        velocityY = speed;
    if (pressedKeys.contains(Qt::Key_A) || pressedKeys.contains(Qt::Key_Left))
        velocityX = -speed;
    if (pressedKeys.contains(Qt::Key_D) || pressedKeys.contains(Qt::Key_Right))
        velocityX = speed;

    // Нормалізація діагонального руху
    if (velocityX != 0 && velocityY != 0) {
        double factor = 1.0 / qSqrt(2.0);
        velocityX *= factor;
        velocityY *= factor;
    }

    position.setX(position.x() + velocityX * deltaTime);
    position.setY(position.y() + velocityY * deltaTime);

    // Обмеження в межах екрану (800x600)
    position.setX(qBound(0.0, position.x(), 800.0 - size.width()));
    position.setY(qBound(0.0, position.y(), 600.0 - size.height()));
}

void Player::render(QPainter& painter)
{
    // Ефект миготіння при невразливості
    if (invincibilityTimer > 0) {
        int blinkPhase = (int)(invincibilityTimer * 10) % 2;
        if (blinkPhase == 0) {
            painter.setOpacity(0.3);
        }
    }

    if (!sprite.isNull()) {
        painter.drawPixmap(position.toPoint(), sprite.scaled(size.toSize()));
    } else {
        // Fallback - білий курсор
        painter.setBrush(Qt::white);
        painter.setPen(Qt::black);
        QPolygonF cursor;
        cursor << position
               << position + QPointF(0, size.height() * 0.75)
               << position + QPointF(size.width() * 0.25, size.height() * 0.6)
               << position + QPointF(size.width() * 0.45, size.height())
               << position + QPointF(size.width() * 0.55, size.height() * 0.95)
               << position + QPointF(size.width() * 0.35, size.height() * 0.55)
               << position + QPointF(size.width() * 0.6, size.height() * 0.55);
        painter.drawPolygon(cursor);
    }

    painter.setOpacity(1.0);

    // Візуалізація атаки
    if (attacking) {
        painter.setBrush(QColor(255, 255, 0, 100));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(getAttackBounds());
    }

    // Прогрес-бар cooldown'у атаки (над курсором)
    if (attackCooldown > 0) {
        double cooldownPercent = 1.0 - (attackCooldown / attackCooldownTime);
        double barWidth = 24;
        double barHeight = 4;
        double barX = position.x() + (size.width() - barWidth) / 2;
        double barY = position.y() - 10;

        // Фон (сірий)
        painter.setBrush(QColor(60, 60, 60));
        painter.setPen(Qt::NoPen);
        painter.drawRect(QRectF(barX, barY, barWidth, barHeight));

        // Заповнення (жовтий/помаранчевий)
        painter.setBrush(QColor(255, 180, 0));
        painter.drawRect(QRectF(barX, barY, barWidth * cooldownPercent, barHeight));

        // Рамка
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRectF(barX, barY, barWidth, barHeight));
    }
}

void Player::setKeyPressed(int key, bool pressed)
{
    if (pressed) {
        pressedKeys.insert(key);
    } else {
        pressedKeys.remove(key);
    }
}

void Player::takeDamage(int amount)
{
    if (invincibilityTimer > 0) return;

    health = qMax(0, health - amount);
    invincibilityTimer = invincibilityDuration;
}

void Player::heal(int amount)
{
    health = qMin(maxHealth, health + amount);
}

void Player::attack()
{
    if (attackCooldown <= 0 && !attacking) {
        attacking = true;
        attackTimer = attackDuration;
        attackCooldown = attackCooldownTime;
    }
}

QRectF Player::getAttackBounds() const
{
    double attackRadius = 50;
    return QRectF(
        position.x() + size.width() / 2 - attackRadius,
        position.y() + size.height() / 2 - attackRadius,
        attackRadius * 2,
        attackRadius * 2
    );
}

QRectF Player::getBounds() const
{
    return QRectF(position, size);
}
