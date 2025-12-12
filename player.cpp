#include "player.h"
#include <QKeyEvent>
#include <QtMath>

Player::Player(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 300.0;

    // Завантаження спрайтів курсора (5 станів HP: 100, 80, 60, 40, 20)
    cursorSprites.append(QPixmap(":/sprites/assets/cursor_100.png")); // 100 HP - цілий
    cursorSprites.append(QPixmap(":/sprites/assets/cursor_80.png"));  // 80 HP
    cursorSprites.append(QPixmap(":/sprites/assets/cursor_60.png"));  // 60 HP
    cursorSprites.append(QPixmap(":/sprites/assets/cursor_40.png"));  // 40 HP
    cursorSprites.append(QPixmap(":/sprites/assets/cursor_20.png"));  // 20 HP - найбільш пошкоджений

    updateSprite();
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

    if (pressedKeys.contains(Qt::Key_W) || pressedKeys.contains(Qt::Key_Up) || pressedKeys.contains(QChar(0x0426).unicode()))
        velocityY = -speed;
    if (pressedKeys.contains(Qt::Key_S) || pressedKeys.contains(Qt::Key_Down)
        || pressedKeys.contains(QChar(0x0406).unicode()) || pressedKeys.contains(QChar(0x042B).unicode()))
        velocityY = speed;
    if (pressedKeys.contains(Qt::Key_A) || pressedKeys.contains(Qt::Key_Left) || pressedKeys.contains(QChar(0x0424).unicode()))
        velocityX = -speed;
    if (pressedKeys.contains(Qt::Key_D) || pressedKeys.contains(Qt::Key_Right) || pressedKeys.contains(QChar(0x0412).unicode()))
        velocityX = speed;

    // Нормалізація діагонального руху
    if (velocityX != 0 && velocityY != 0) {
        double factor = 1.0 / qSqrt(2.0);
        velocityX *= factor;
        velocityY *= factor;
    }

    position.setX(position.x() + velocityX * deltaTime);
    position.setY(position.y() + velocityY * deltaTime);

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

    increaseLoad(amount);
    invincibilityTimer = invincibilityDuration;
    updateSprite();
}

void Player::increaseLoad(int amount)
{
    cpuLoad += amount;
    if (cpuLoad > maxCpuLoad) {
        cpuLoad = maxCpuLoad;
    }
    updateSprite();
}

void Player::decreaseLoad(int amount)
{
    cpuLoad -= amount;
    if (cpuLoad < 0) {
        cpuLoad = 0;
    }
    updateSprite();
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

void Player::reset()
{
    cpuLoad = 0;
    invincibilityTimer = 0;
    attacking = false;
    attackTimer = 0;
    attackCooldown = 0;
    updateSprite();
}

void Player::clearKeys()
{
    pressedKeys.clear();
}

void Player::updateSprite()
{
    // Вибір спрайту залежно від cpuLoad
    int spriteIndex = 0;

    if (cpuLoad > 80) {
        spriteIndex = 4;
    } else if (cpuLoad > 60) {
        spriteIndex = 3;
    } else if (cpuLoad > 40) {
        spriteIndex = 2;
    } else if (cpuLoad > 20) {
        spriteIndex = 1;
    } else {
        spriteIndex = 0;
    }

    if (spriteIndex < cursorSprites.size() && !cursorSprites[spriteIndex].isNull()) {
        sprite = cursorSprites[spriteIndex];
    }
}
