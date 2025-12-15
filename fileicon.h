#ifndef FILEICON_H
#define FILEICON_H

#include "gameobject.h"
#include "folderscene.h"
#include <QPixmap>

class FileIcon : public GameObject
{
public:

    FileIcon(QPointF pos, FileActivityType activityType, int folderIndex);
    

    FileIcon(QPointF pos, FileType fileType, int folderIndex);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    QRectF getBounds() const;

    FileType getFileType() const { return fileType; }
    bool isCorrectFile() const { return fileType == FileType::Correct; }
    

    FileActivityType getActivityType() const { return activityType; }
    int getFolderIndex() const { return folderIndex; }

    bool isInteractable() const { return interactable && !completed && visible; }
    void setInteractable(bool value) { interactable = value; }
    

    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }

    bool isCompleted() const { return completed; }
    void setCompleted(bool value) { completed = value; }

    void setHighlighted(bool value) { highlighted = value; }
    bool isHighlighted() const { return highlighted; }

    QString getFileName() const;

private:
    void generateRandomName();
    
    FileType fileType;
    FileActivityType activityType;
    int folderIndex;
    QPixmap sprite;
    QString randomName;
    bool interactable = true;
    bool highlighted = false;
    bool completed = false;
    bool visible = true;

    double appearAnimation = 0.0;
    bool appearing = true;
};

#endif
