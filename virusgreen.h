#ifndef VIRUSGREEN_H
#define VIRUSGREEN_H

#include "enemy.h"

class VirusGreen : public Enemy
{
public:
    VirusGreen(QPointF pos, QSizeF s);

protected:
    void updateSprite() override;
    void onDeath() override;

private:
    void loadSprites();

    // Спрайти для різних станів HP
    QPixmap spriteFull;   // 8 точок - 100% HP
    QPixmap sprite7;      // 7 точок
    QPixmap sprite5;      // 5 точок - ~50% HP
    QPixmap sprite4;      // 4 точки

    // Спрайти для анімації смерті
    QPixmap sprite3;      // 3 точки
    QPixmap sprite2;      // 2 точки
    QPixmap sprite1;      // 1 точка
};

#endif // VIRUSGREEN_H
