#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include "game.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override; // метод відображення
    void timerEvent(QTimerEvent *event) override; // метод подій за часом
    void keyPressEvent(QKeyEvent *event) override;

private:
    Game game;
    int gameTimerId;

    QElapsedTimer elapsedTimer;
    long long lastFrameTime;
};

#endif // GAMEWIDGET_H
