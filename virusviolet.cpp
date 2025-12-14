#include "virusviolet.h"
#include <QDebug>
#include <QtMath>

VirusViolet::VirusViolet(QPointF pos, QSizeF s)
    : Enemy(pos, s)
{
    maxHealth = 50;
    health = maxHealth;
    speed = 60.0f;  // Повільніший ніж зелений, бо стріляє
    loadSprites();
    updateSprite();
}

void VirusViolet::loadSprites()
{
    spriteFull = QPixmap(":/sprites/assets/virus_violet/vv_full.png");
    sprite8 = QPixmap(":/sprites/assets/virus_violet/vv_8.png");
    sprite5 = QPixmap(":/sprites/assets/virus_violet/vv_5.png");
    sprite4 = QPixmap(":/sprites/assets/virus_violet/vv_4.png");
    sprite3 = QPixmap(":/sprites/assets/virus_violet/vv_3.png");
    sprite2 = QPixmap(":/sprites/assets/virus_violet/vv_2.png");
    sprite1 = QPixmap(":/sprites/assets/virus_violet/vv_1.png");

    // Налаштовуємо анімацію смерті
    deathAnimationSprites.clear();
    deathAnimationSprites.append(sprite4);
    deathAnimationSprites.append(sprite3);
    deathAnimationSprites.append(sprite2);
    deathAnimationSprites.append(sprite1);

    if (spriteFull.isNull()) {
        qDebug() << "Failed to load VirusViolet sprites!";
    }
}

void VirusViolet::update(double deltaTime)
{
    // Оновлюємо таймер стрільби
    if (shootTimer > 0) {
        shootTimer -= deltaTime;
    }

    // Викликаємо базовий update
    Enemy::update(deltaTime);
}

void VirusViolet::updateSprite()
{
    if (isPlayingDeathAnimation()) {
        return;
    }

    float healthPercent = (float)health / maxHealth * 100;

    if (healthPercent > 80) {
        currentSprite = spriteFull;
    } else if (healthPercent > 60) {
        currentSprite = sprite8;
    } else if (healthPercent > 40) {
        currentSprite = sprite5;
    } else {
        currentSprite = sprite4;
    }
}

bool VirusViolet::canShoot() const
{
    // Може стріляти якщо кулдаун пройшов і є достатньо HP
    return shootTimer <= 0 && health > healthCostPerShot && !isPlayingDeathAnimation();
}

QPointF VirusViolet::getShootDirection(QPointF targetPos) const
{
    QPointF center = position + QPointF(size.width() / 2, size.height() / 2);
    QPointF direction = targetPos - center;

    // Нормалізуємо напрямок
    float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        direction /= length;
    }

    return direction;
}

void VirusViolet::shoot()
{
    if (!canShoot()) return;

    // Витрачаємо HP на постріл
    health -= healthCostPerShot;
    shootTimer = shootCooldown;

    // Оновлюємо спрайт після втрати HP
    updateSprite();

    // Перевіряємо чи не померли від пострілу
    if (health <= 0) {
        onDeath();
    }
}

void VirusViolet::onDeath()
{
    playDeathAnimation();
}
