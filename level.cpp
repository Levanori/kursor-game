#include "level.h"
#include <QRandomGenerator>

Level::Level()
{
    player = new Player(QPointF(400, 300), QSizeF(32, 32));

    // Створюємо початкових ворогів
    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

Level::~Level()
{
    delete player;
    qDeleteAll(enemies);
}

void Level::update(double deltaTime)
{
    if (gameOver) return;

    // Оновлення гравця
    player->update(deltaTime);

    QPointF pos = player->getPosition();
    QSizeF size = player->getSize();

    // обмеження X та Y для гравця
    pos.setX(qBound(0.0, pos.x(), (double)VIRTUAL_WIDTH - size.width()));
    pos.setY(qBound(0.0, pos.y(), (double)VIRTUAL_HEIGHT - size.height()));

    player->setPosition(pos);

    if (player->isDead()) {
        gameOver = true;
        return;
    }

    // Оновлення ворогів
    for (Enemy* enemy : enemies) {
        enemy->update(deltaTime);
    }

    // Видалення мертвих ворогів
    for (int i = enemies.size() - 1; i >= 0; --i) {
        if (enemies[i]->isDead()) {
            delete enemies[i];
            enemies.removeAt(i);
            score += 100;
        }
    }

    // Перевірка колізій
    checkCollisions();

    // Спавн нових ворогів
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        spawnEnemy();
    }
}

void Level::render(QPainter& painter)
{
    // Рендер ворогів
    for (Enemy* enemy : enemies) {
        enemy->render(painter);
    }

    // Рендер гравця
    player->render(painter);

    // UI
    renderUI(painter);
}

void Level::handleKeyPress(int key)
{
    if (gameOver) {
        if (key == Qt::Key_Escape) {
            // логіка для рестарту
        }
        return;
    }
    player->setKeyPressed(key, true);

    // Атака пробілом
    if (key == Qt::Key_Space) {
        player->attack();
    }

}

void Level::handleKeyRelease(int key)
{
    player->setKeyPressed(key, false);
}

void Level::checkCollisions()
{
    QRectF playerBounds = player->getBounds();
    QRectF attackBounds = player->getAttackBounds();

    for (Enemy* enemy : enemies) {
        if (enemy->isDead()) continue;

        QRectF enemyBounds = enemy->getBounds();

        // Атака гравця на ворога
        if (player->isAttacking() && attackBounds.intersects(enemyBounds)) {
            enemy->takeDamage(25);
            enemy->knockback(player->getPosition(), 200.0);
        }

        // Ворог атакує гравця
        if (playerBounds.intersects(enemyBounds) && !player->isInvincible()) {
            player->takeDamage(enemy->getDamage());
            enemy->knockback(player->getPosition(), 150.0);
        }
    }
}

void Level::spawnEnemy()
{
    // Спавн на краю екрану
    double x, y;
    int edge = QRandomGenerator::global()->bounded(4);

    switch (edge) {
        case 0: // Верх
            x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH);
            y = -32;
            break;
        case 1: // Право
            x = VIRTUAL_WIDTH;
            y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT);
            break;
        case 2: // Низ
            x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH);
            y = VIRTUAL_HEIGHT;
            break;
        default: // Ліво
            x = -32;
            y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT);
            break;
    }

    Enemy* enemy = new Enemy(QPointF(x, y), QSizeF(32, 32));
    enemy->setTarget(player->getPositionPtr());
    enemies.append(enemy);
}

void Level::renderUI(QPainter& painter)
{
    // HP Bar
    painter.setBrush(Qt::darkRed);
    painter.setPen(Qt::black);
    painter.drawRect(10, 10, 200, 20);

    double healthPercent = (double)player->getHealth() / player->getMaxHealth();
    painter.setBrush(Qt::green);
    painter.drawRect(10, 10, (int)(200 * healthPercent), 20);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(15, 26, QString("HP: %1/%2").arg(player->getHealth()).arg(player->getMaxHealth()));

    // Score
    painter.drawText(VIRTUAL_WIDTH - 120, 26, QString("Score: %1").arg(score));

    // Підказки керування
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, VIRTUAL_WIDTH - 10, "WASD/Arrows - Move | SPACE - Attack | ESC - Pause");
}

void Level::handleResize(int w, int h)
{
    currentScreenWidth = w;
    currentScreenHeight = h;
}


double Level::getScaleFactor() const
{
    double scaleX = (double)currentScreenWidth / VIRTUAL_WIDTH;
    double scaleY = (double)currentScreenHeight / VIRTUAL_HEIGHT;
    return qMin(scaleX, scaleY);
}

void Level::reset()
{
    qDeleteAll(enemies);
    enemies.clear();

    player->setPosition(QPointF(400, 300));
    player->resetHealth();

    gameOver = false;
    score = 0;
    spawnTimer = 0;

    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}
