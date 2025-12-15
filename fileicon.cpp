#include "fileicon.h"
#include <QPainter>
#include <QRandomGenerator>

FileIcon::FileIcon(QPointF pos, FileActivityType activity, int folder)
    : fileType(FileType::Correct)
    , activityType(activity)
    , folderIndex(folder)
{
    position = pos;
    size = QSizeF(48, 48);
    sprite = QPixmap();
    generateRandomName();
}

FileIcon::FileIcon(QPointF pos, FileType type, int folder)
    : fileType(type)
    , activityType(FileActivityType::Boss)
    , folderIndex(folder)
{
    position = pos;
    size = QSizeF(48, 48);
    sprite = QPixmap();
    generateRandomName();
}

void FileIcon::update(double deltaTime)
{
    if (!visible) return;
    

    if (appearing) {
        appearAnimation += deltaTime * 2.0;
        if (appearAnimation >= 1.0) {
            appearAnimation = 1.0;
            appearing = false;
        }
    }
}

void FileIcon::render(QPainter& painter)
{
    if (!visible) return;
    

    double scale = appearing ? appearAnimation : 1.0;
    QSizeF currentSize = size * scale;
    QPointF currentPos = position + QPointF(
        (size.width() - currentSize.width()) / 2,
        (size.height() - currentSize.height()) / 2
    );

    QColor fillColor = completed ? QColor(80, 80, 100) : QColor(120, 120, 160);
    QColor borderColor = Qt::black;

    if (highlighted && !completed) {
        borderColor = Qt::yellow;
        painter.setPen(QPen(Qt::yellow, 3));
    } else {
        painter.setPen(QPen(borderColor, 2));
    }

    painter.setBrush(fillColor);
    
    QPolygonF fileShape;
    double earSize = currentSize.width() * 0.25;
    fileShape << currentPos
              << currentPos + QPointF(currentSize.width() - earSize, 0)
              << currentPos + QPointF(currentSize.width(), earSize)
              << currentPos + QPointF(currentSize.width(), currentSize.height())
              << currentPos + QPointF(0, currentSize.height());
    painter.drawPolygon(fileShape);

    QPolygonF ear;
    ear << currentPos + QPointF(currentSize.width() - earSize, 0)
        << currentPos + QPointF(currentSize.width() - earSize, earSize)
        << currentPos + QPointF(currentSize.width(), earSize);
    painter.setBrush(fillColor.darker(120));
    painter.drawPolygon(ear);

    if (completed && fileType == FileType::Correct) {
        painter.setPen(QPen(Qt::green, 3));
        QPointF center = currentPos + QPointF(currentSize.width() / 2, currentSize.height() / 2);
        painter.drawLine(center + QPointF(-8, 0), center + QPointF(-2, 8));
        painter.drawLine(center + QPointF(-2, 8), center + QPointF(10, -8));
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 7, QFont::Bold));
    QString name = getFileName();
    QRectF textRect(currentPos.x() - 10, currentPos.y() + currentSize.height() + 2, 
                    currentSize.width() + 20, 12);
    painter.drawText(textRect, Qt::AlignCenter, name);

    if (highlighted && !completed) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        QRectF hintRect(position.x() - 10, position.y() - 18, size.width() + 20, 15);
        painter.drawText(hintRect, Qt::AlignCenter, "SPACE");
    }
}

QRectF FileIcon::getBounds() const
{
    return QRectF(position, size);
}

QString FileIcon::getFileName() const
{
    return randomName;
}

void FileIcon::generateRandomName()
{

    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-#@!$%";
    const QStringList extensions = {".exe", ".dat", ".sys", ".tmp", ".bin", ".dll"};
    

    int nameLength = 4 + QRandomGenerator::global()->bounded(5);
    
    QString name;
    for (int i = 0; i < nameLength; ++i) {
        int charIndex = QRandomGenerator::global()->bounded(chars.length());
        name += chars[charIndex];
    }
    

    int extIndex = QRandomGenerator::global()->bounded(extensions.size());
    randomName = name + extensions[extIndex];
}
