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

    QPixmap spriteFull;
    QPixmap sprite7;
    QPixmap sprite5;
    QPixmap sprite4;

    QPixmap sprite3;
    QPixmap sprite2;
    QPixmap sprite1;
};

#endif
