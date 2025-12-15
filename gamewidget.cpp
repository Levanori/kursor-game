#include "gamewidget.h"
#include <QResizeEvent>
#include <QDebug>

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(725, 450);
    game.initialize();
    game.handleResize(width(), height());

    setFocusPolicy(Qt::StrongFocus);

    backgroundImage = QPixmap(":/sprites/assets/backgrounds/desktop1_bg.svg");
    loadButtonSprites();

    connect(&game, &Game::quitRequested, this, &GameWidget::handleQuitRequest);
    connect(&game, &Game::requestWindowResize, this, &GameWidget::handleWindowResize);
    connect(&game, &Game::requestFullscreen, this, &GameWidget::handleFullscreenRequest);
    connect(&game, &Game::restartRequested, this, &GameWidget::handleRestartRequest);

    gameTimerId = startTimer(16);

    elapsedTimer.start();
    lastFrameTime = 0;
}

void GameWidget::handleQuitRequest()
{
    this->close();
}

void GameWidget::handleWindowResize(QSize newSize)
{
    if (isFullScreen()) {
        showNormal();
    }
    isFullscreen = false;
    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setFixedSize(newSize);

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - newSize.width()) / 2;
        int y = (screenGeometry.height() - newSize.height()) / 2;
        move(x, y);
    }

    game.handleResize(newSize.width(), newSize.height());
    update();
}

void GameWidget::handleFullscreenRequest()
{
    if (isFullscreen) {
        showNormal();
        isFullscreen = false;
        setFixedSize(game.getScreenWidth(), game.getScreenHeight());
    } else {
        showFullScreen();
        isFullscreen = true;
    }

    QSize currentSize = geometry().size();
    game.handleResize(currentSize.width(), currentSize.height());
    update();
}

void GameWidget::handleRestartRequest()
{
    // Не змінюємо розмір вікна - зберігаємо поточний масштаб
    update();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (!backgroundImage.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), backgroundImage);
    } else {

        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0, QColor(0, 120, 215));
        gradient.setColorAt(1, QColor(0, 80, 150));
        painter.fillRect(rect(), gradient);
    }

    GameScene* currentScene = game.getCurrentScene();
    if (currentScene) {

        double scale = currentScene->getScaleFactor();

        double levelW = currentScene->getVirtualWidth();
        double levelH = currentScene->getVirtualHeight();

        double offsetX = (width() / scale - levelW) / 2.0;
        double offsetY = (height() / scale - levelH) / 2.0;

        painter.save();
        painter.scale(scale, scale);
        painter.translate(offsetX, offsetY);
        game.render(painter);
        painter.restore();

        currentScene->renderUI(painter, scale, offsetX, offsetY);

        int w = width();
        int h = height();
        if (currentScene->isGameOver()) {
            painter.fillRect(0, 0, w, h, QColor(0, 0, 0, 180));

            painter.setPen(Qt::red);
            painter.setFont(QFont("Arial", 48, QFont::Bold));
            painter.drawText(QRect(0, h / 2 - 100, w, 100), Qt::AlignCenter, "GAME OVER");

            painter.setFont(QFont("Arial", 18));
            painter.setPen(Qt::white);
            painter.drawText(QRect(0, h / 2 + 20, w, 50), Qt::AlignCenter, QString("Score: %1").arg(currentScene->getScore()));
            painter.drawText(QRect(0, h / 2 + 80, w, 50), Qt::AlignCenter, "Press ESC to restart");

        } else if (game.isitPaused()) {
            painter.fillRect(0, 0, w, h, QColor(0, 0, 0, 128));

            painter.setFont(QFont("Arial", 18, QFont::Bold));

            int buttonW = 300;
            int buttonH = 50;
            int spacing = 80;

            int leftColX = (w / 2) - buttonW - 20;

            painter.setPen(Qt::yellow);
            painter.drawText(0, h / 4 - 20, w, 30, Qt::AlignCenter, "WASD/Arrows - Move | SPACE - Attack | ESC - Continue");

            painter.setFont(QFont("Arial", 18, QFont::Bold));
            QColor textColor = Qt::black;
            int textPaddingRight = 100;

            QRect continueRect(leftColX, h / 2 - 50, buttonW, buttonH);
            painter.drawPixmap(continueRect, m_btnContinue.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(continueRect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "Continue (ESC)");

            QRect restartRect(leftColX, h / 2 - 50 + spacing, buttonW, buttonH);
            painter.drawPixmap(restartRect, m_btnRestart.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(restartRect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "Restart");

            QRect quitRect(leftColX, h / 2 - 50 + 2 * spacing, buttonW, buttonH);
            painter.drawPixmap(quitRect, m_btnQuit.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(quitRect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "Quit");

            int rightColX = (w / 2) + 20;
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 18, QFont::Bold));
            painter.drawText(QRect(rightColX, h / 2 - 90, buttonW, 30), Qt::AlignCenter, "Scale");

            painter.setFont(QFont("Arial", 18, QFont::Bold));

            QRect scale725Rect(rightColX, h / 2 - 50, buttonW, buttonH);
            painter.drawPixmap(scale725Rect, m_btnScale.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(scale725Rect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "725x450");

            QRect scale1450Rect(rightColX, h / 2 - 50 + spacing, buttonW, buttonH);
            painter.drawPixmap(scale1450Rect, m_btnScale.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(scale1450Rect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "1450x900");

            QRect scaleFullRect(rightColX, h / 2 - 50 + 2 * spacing, buttonW, buttonH);
            painter.drawPixmap(scaleFullRect, m_btnFullscreen.scaled(buttonW, buttonH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter.setPen(textColor);
            painter.drawText(scaleFullRect.adjusted(0, 0, -textPaddingRight, 0), Qt::AlignCenter, "Fullscreen");
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
        // Просто передаємо клавішу - масштаб зберігається
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

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if (game.isitPaused()) {

        game.handleMouseClick(event->pos());
        update();
        return;
    }
    QWidget::mousePressEvent(event);
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    game.handleResize(event->size().width(), event->size().height());
    update();
}

void GameWidget::loadButtonSprites()
{
    m_btnContinue = QPixmap(":/buttons/continue.png");
    m_btnRestart = QPixmap(":/buttons/restart.png");
    m_btnQuit = QPixmap(":/buttons/quit.png");
    m_btnScale = QPixmap(":/buttons/scale.png");
    m_btnFullscreen = QPixmap(":/buttons/fullscreen.png");

    if (m_btnContinue.isNull()) qDebug() << "Warning: Could not load continue button";
    if (m_btnRestart.isNull()) qDebug() << "Warning: Could not load restart button";
    if (m_btnQuit.isNull()) qDebug() << "Warning: Could not load quit button";
    if (m_btnScale.isNull()) qDebug() << "Warning: Could not load scale button";
    if (m_btnFullscreen.isNull()) qDebug() << "Warning: Could not load fullscreen button";
}
