#include "testobject.h"

TestObject::TestObject(QPointF pos, QSizeF s)
{
    position = pos;
    size = s;
    speed = 100.0;
}

void TestObject::update(double deltaTime)
{

    position.setX(position.x() + speed * deltaTime);

    if (position.x() > 1000) {
        position.setX(0);
    }
}

void TestObject::render(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::green));
    painter.drawRect(QRectF(position, size));
}
