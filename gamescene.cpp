#include "gamescene.h"
#include "player.h"

GameScene::GameScene()
{
    // Ініціалізація розмірів екрану за замовчуванням
    currentScreenWidth = VIRTUAL_WIDTH;
    currentScreenHeight = VIRTUAL_HEIGHT;
}

void GameScene::handleResize(int w, int h)
{
    // Просто зберігаємо поточні розміри
    currentScreenWidth = w;
    currentScreenHeight = h;
}

double GameScene::getScaleFactor() const
{
    double scaleX = (double)currentScreenWidth / VIRTUAL_WIDTH;
    double scaleY = (double)currentScreenHeight / VIRTUAL_HEIGHT;
    return qMin(scaleX, scaleY);
}

void GameScene::clearPlayerKeys()
{
    if (player) {
        player->clearKeys();
    }
}

void GameScene::renderBaseUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY, const QString& extraHint)
{
    if (!player) return;

    int screenX = (int)(offsetX * scaleFactor);
    int screenY = (int)(offsetY * scaleFactor);
    int scaledW = (int)(VIRTUAL_WIDTH * scaleFactor);

    int hpBarX = screenX + 10;
    int hpBarY = screenY + 10;
    int hpBarW = 200;
    int hpBarH = 20;

    painter.setBrush(Qt::darkRed);
    painter.setPen(Qt::black);
    painter.drawRect(hpBarX, hpBarY, hpBarW, hpBarH);

    double loadPercent = (double)player->getCpuLoad() / player->getMaxCpuLoad();
    if (player->getCpuLoad() > 80) {
        painter.setBrush(Qt::red);
    } else if (player->getCpuLoad() > 50) {
        painter.setBrush(Qt::yellow);
    } else {
        painter.setBrush(Qt::green);
    }

    painter.drawRect(hpBarX, hpBarY, (int)(hpBarW * loadPercent), hpBarH);

    painter.setPen(Qt::darkGray);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(hpBarX + 5, hpBarY + 16, QString("CPU Load: %1%").arg(player->getCpuLoad()));

    painter.setPen(Qt::white);
    int scoreTextX = screenX + scaledW - 120;
    painter.drawText(scoreTextX, hpBarY + 16, QString("Score: %1").arg(score));

    if (!extraHint.isEmpty()) {
        painter.setPen(Qt::lightGray);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(screenX + 10, hpBarY + 40, extraHint);
    }
}
