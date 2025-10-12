#include "level.h"

Level::Level()
{
    playerPtr = new Player(QPointF(100, 100), QSizeF(40, 40)); // створення об'єктів
    testObjPtr = new TestObject(QPointF(50, 300), QSizeF(30, 30));
}

Level::~Level()
{
    delete playerPtr;
    delete testObjPtr;
}

void Level::update(double deltaTime)
{
    playerPtr->update(deltaTime);
    testObjPtr->update(deltaTime);
}

void Level::render(QPainter& painter)
{
    playerPtr->render(painter);
    testObjPtr->render(painter);
}
