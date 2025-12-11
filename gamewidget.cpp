#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(1000, 600);
    game.initialize();

    setFocusPolicy(Qt::StrongFocus);
    // Завантаження фону робочого столу
    backgroundImage = QPixmap(":/sprites/assets/desktop_background.svg");

    gameTimerId = startTimer(16); // ~60 FPS

    elapsedTimer.start();
    lastFrameTime = 0;
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    game.handleResize(event->size().width(), event->size().height());
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

    Level* currentLevel = game.getCurrentLevel();
    if (currentLevel) {
        // коефіцієнт масштабування
        double scale = currentLevel->getScaleFactor();

        double levelW = currentLevel->getVirtualWidth();
        double levelH = currentLevel->getVirtualHeight();

        // (width() / scale - levelW) / 2.0; - це зайвий простір зліва/справа у віртуальних коорд.
        double offsetX = (width() / scale - levelW) / 2.0;
        double offsetY = (height() / scale - levelH) / 2.0;

        painter.save();
        painter.scale(scale, scale);
        painter.translate(offsetX, offsetY);
        game.render(painter);
        painter.restore();

        int w = width();
        int h = height();
        if (currentLevel->isGameOver()) {
            painter.fillRect(0, 0, w, h, QColor(0, 0, 0, 180));

            painter.setPen(Qt::red);
            painter.setFont(QFont("Arial", 48, QFont::Bold));
            painter.drawText(QRect(0, h / 2 - 100, w, 100), Qt::AlignCenter, "GAME OVER");

            painter.setFont(QFont("Arial", 18));
            painter.setPen(Qt::white);
            painter.drawText(QRect(0, h / 2 + 20, w, 50), Qt::AlignCenter, QString("Score: %1").arg(currentLevel->getScore()));
            painter.drawText(QRect(0, h / 2 + 80, w, 50), Qt::AlignCenter, "Press ESC to restart");

        } else if (game.isitPaused()) {
            painter.fillRect(0, 0, w, h, QColor(0, 0, 0, 128));

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 32, QFont::Bold));
            painter.drawText(QRect(0, h / 2 - 50, w, 50), Qt::AlignCenter, "PAUSED");

            painter.setFont(QFont("Arial", 18));
            painter.drawText(QRect(0, h / 2 + 20, w, 50), Qt::AlignCenter, "Press ESC to continue");
        }
    }
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
