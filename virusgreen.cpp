#include "virusgreen.h"
#include <QDebug>

VirusGreen::VirusGreen(QPointF pos, QSizeF s)
    : Enemy(pos, s)
{
    maxHealth = 50;
    health = maxHealth;
    loadSprites();
    updateSprite();
}

void VirusGreen::loadSprites()
{
    spriteFull = QPixmap(":/sprites/assets/virus_green/vg_full.png");
    sprite7 = QPixmap(":/sprites/assets/virus_green/vg_7.png");
    sprite5 = QPixmap(":/sprites/assets/virus_green/vg_5.png");
    sprite4 = QPixmap(":/sprites/assets/virus_green/vg_4.png");
    sprite3 = QPixmap(":/sprites/assets/virus_green/vg_3.png");
    sprite2 = QPixmap(":/sprites/assets/virus_green/vg_2.png");
    sprite1 = QPixmap(":/sprites/assets/virus_green/vg_1.png");

    // Налаштовуємо анімацію смерті (4 -> 3 -> 2 -> 1)
    deathAnimationSprites.clear();
    deathAnimationSprites.append(sprite4);
    deathAnimationSprites.append(sprite3);
    deathAnimationSprites.append(sprite2);
    deathAnimationSprites.append(sprite1);

    // Перевіряємо чи спрайти завантажились
    if (spriteFull.isNull()) {
        qDebug() << "Failed to load VirusGreen sprites!";
    }
}

void VirusGreen::updateSprite()
{
    if (isPlayingDeathAnimation()) {
        return; // Під час анімації смерті не змінюємо спрайт
    }

    float healthPercent = (float)health / maxHealth * 100;

    // Вибираємо спрайт залежно від HP
    if (healthPercent > 80) {
        currentSprite = spriteFull;  // 8 точок
    } else if (healthPercent > 60) {
        currentSprite = sprite7;      // 7 точок
    } else if (healthPercent > 40) {
        currentSprite = sprite5;      // 5 точок
    } else {
        currentSprite = sprite4;      // 4 точки
    }
}

void VirusGreen::onDeath()
{
    // Запускаємо анімацію смерті
    playDeathAnimation();
}
