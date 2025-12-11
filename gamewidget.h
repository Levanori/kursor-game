#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QElapsedTimer>
#include <QPixmap>
#include "game.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void handleQuitRequest();
    void handleWindowResize(QSize newSize);
    void handleFullscreenRequest();

private:
    Game game;
    int gameTimerId;

    QElapsedTimer elapsedTimer;
    long long lastFrameTime;

    QPixmap backgroundImage;
};

#endif // GAMEWIDGET_H
