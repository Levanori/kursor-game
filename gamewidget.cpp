#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(800, 600);
    game.initialize();

    setFocusPolicy(Qt::StrongFocus);

    // Завантаження фону робочого столу
    backgroundImage = QPixmap(":/sprites/assets/desktop_background.svg");

    gameTimerId = startTimer(16); // ~60 FPS

    elapsedTimer.start();
    lastFrameTime = 0;
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // Фон робочого столу
    if (!backgroundImage.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    } else {
        // Fallback - градієнт
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0, QColor(0, 120, 215));
        gradient.setColorAt(1, QColor(0, 80, 150));
        painter.fillRect(rect(), gradient);
    }

    game.render(painter);
}

void GameWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == gameTimerId) {
        long long currentTime = elapsedTimer.nsecsElapsed();
        double deltaTime = (currentTime - lastFrameTime) / 1000000000.0;
        lastFrameTime = currentTime;
        game.update(deltaTime);
        update();
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        game.handleKeyPress(event->key());
    }
    QWidget::keyPressEvent(event);
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        game.handleKeyRelease(event->key());
    }
    QWidget::keyReleaseEvent(event);
}
