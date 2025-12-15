#ifndef FOLDERICON_H
#define FOLDERICON_H

#include "gameobject.h"
#include <QPixmap>

class FolderIcon : public GameObject
{
public:
    FolderIcon(QPointF pos, int folderIndex);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    QRectF getBounds() const;

    int getFolderIndex() const { return folderIndex; }

    bool isInteractable() const { return interactable; }
    void setInteractable(bool value) { interactable = value; }

    void setHighlighted(bool value) { highlighted = value; }
    bool isHighlighted() const { return highlighted; }

private:
    int folderIndex;
    QPixmap sprite;
    bool interactable = true;
    bool highlighted = false;

    double appearAnimation = 0.0;
    bool appearing = true;
};

#endif
