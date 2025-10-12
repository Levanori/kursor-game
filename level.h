#ifndef LEVEL_H
#define LEVEL_H

#include "player.h"
#include "testobject.h" // потрібно знати всі типи, щоб їх оголосити

class Level
{
public:
    Level();
    ~Level();

    void update(double deltaTime);
    void render(QPainter& painter);

private:
    GameObject* playerPtr;
    GameObject* testObjPtr;
};

#endif // LEVEL_H
