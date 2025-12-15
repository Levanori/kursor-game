#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include "gameobject.h"

class TestObject : public GameObject
{
public:
    TestObject(QPointF pos, QSizeF s);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

private:
    double speed;
};

#endif
