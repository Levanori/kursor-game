#include "foldericon.h"
#include <QPainter>

FolderIcon::FolderIcon(QPointF pos, int index)
    : folderIndex(index)
{
    position = pos;
    size = QSizeF(48, 48);

    QString spritePath;
    switch (folderIndex) {
    case 0:
        spritePath = ":/sprites/assets/folderIcons/folder.png";
        break;
    case 1:
        spritePath = ":/sprites/assets/folderIcons/folder2.png";
        break;
    case 2:
        spritePath = ":/sprites/assets/folderIcons/folder3.png";
        break;
    default:
        spritePath = ":/sprites/assets/folderIcons/folder.png";
        break;
    }

    sprite = QPixmap(spritePath);
}

void FolderIcon::update(double deltaTime)
{

    if (appearing) {
        appearAnimation += deltaTime * 2.0;
        if (appearAnimation >= 1.0) {
            appearAnimation = 1.0;
            appearing = false;
        }
    }
}

void FolderIcon::render(QPainter& painter)
{
    if (sprite.isNull()) {

        painter.setBrush(QBrush(QColor(255, 200, 100)));
        painter.drawRect(QRectF(position, size));
        return;
    }

    double scale = appearing ? appearAnimation : 1.0;
    QSizeF currentSize = size * scale;
    QPointF currentPos = position + QPointF(
        (size.width() - currentSize.width()) / 2,
        (size.height() - currentSize.height()) / 2
    );

    if (highlighted) {
        painter.setPen(QPen(Qt::yellow, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRectF(position - QPointF(2, 2), size + QSizeF(4, 4)));
    }

    QPixmap scaledSprite = sprite.scaled(currentSize.toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(currentPos.toPoint(), scaledSprite);

    if (highlighted) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        QString hint = "SPACE";
        QRectF textRect(position.x() - 10, position.y() + size.height() + 2, size.width() + 20, 15);
        painter.drawText(textRect, Qt::AlignCenter, hint);
    }
}

QRectF FolderIcon::getBounds() const
{
    return QRectF(position, size);
}
