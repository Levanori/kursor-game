#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

class Player : public GameObject
{
public:
    Player(QPointF pos, QSizeF s);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
};

#endif // PLAYER_H
