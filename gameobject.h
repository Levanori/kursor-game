#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QPainter> // тимчасово для малювання фігур

class GameObject
{
public:
    virtual ~GameObject() = default;
    virtual void update(double deltaTime) = 0; // оновлення об'єкта
    virtual void render(QPainter& painter) = 0;

    QPointF getPosition() {
        return position;
    }
    QSizeF getSize() {
        return size;
    }

protected:
    QPointF position;
    QSizeF size;
    double speed = 0.0;
};

#endif // GAMEOBJECT_H
