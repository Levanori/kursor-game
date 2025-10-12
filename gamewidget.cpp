#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
{
    game.initialize();

    setFocusPolicy(Qt::StrongFocus); // можливість використовувати клавіші не для вводу

    gameTimerId = startTimer(16);

    elapsedTimer.start();
    lastFrameTime = 0;
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(90, 50, 40)); // заповнення всієї сцени
    game.render(painter);
}

void GameWidget::timerEvent(QTimerEvent *event) // функція для покадрового перемалювання(оновлення станів об'єктів)
{
    if (event->timerId() == gameTimerId) {
        long long currentTime = elapsedTimer.nsecsElapsed();  // використання бібліотеки qt для отримання всього часу гри
        double deltaTime = (currentTime - lastFrameTime) / 1000000000.0;
        lastFrameTime = currentTime;
        game.update(deltaTime);
        update();
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    game.handleKeyPress(event->key());
    QWidget::keyPressEvent(event);
}
