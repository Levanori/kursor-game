#include "level.h"
#include <QRandomGenerator>
#include <QtMath>

Level::Level()
{
    player = new Player(QPointF(400, 300), QSizeF(32, 32));

    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

Level::~Level()
{
    delete player;
    qDeleteAll(enemies);
    qDeleteAll(projectiles);
}

void Level::update(double deltaTime)
{
    if (gameOver) return;

    if (player) {
        accumulatedLoad += LOAD_PER_SECOND * deltaTime;
        if (accumulatedLoad >= 1.0) {

            int loadIncrease = (int)accumulatedLoad;

            player->increaseLoad(loadIncrease);

            accumulatedLoad -= loadIncrease;
        }
    }

    player->update(deltaTime);

    QPointF pos = player->getPosition();
    QSizeF size = player->getSize();

    pos.setX(qBound(0.0, pos.x(), (double)VIRTUAL_WIDTH - size.width()));
    pos.setY(qBound(0.0, pos.y(), (double)VIRTUAL_HEIGHT - size.height()));

    player->setPosition(pos);

    if (player->isCrashed()) {
        gameOver = true;
        return;
    }

    for (Enemy* enemy : enemies) {
        enemy->update(deltaTime);

        VirusViolet* violet = dynamic_cast<VirusViolet*>(enemy);
        if (violet && violet->canShoot()) {

            QPointF enemyCenter = enemy->getPosition() + QPointF(enemy->getSize().width() / 2, enemy->getSize().height() / 2);
            QPointF playerCenter = player->getPosition() + QPointF(player->getSize().width() / 2, player->getSize().height() / 2);
            QPointF diff = playerCenter - enemyCenter;
            float distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());

            if (distance >= 100 && distance <= 400) {
                spawnProjectile(violet);
            }
        }
    }

    updateProjectiles(deltaTime);

    for (int i = enemies.size() - 1; i >= 0; --i) {
        if (enemies[i]->isDead()) {
            if (player) player->decreaseLoad(LOAD_PER_KILL);
            delete enemies[i];
            enemies.removeAt(i);
            score += 100;
        }
    }

    checkCollisions();

    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        spawnEnemy();
    }
}

void Level::render(QPainter& painter)
{

    for (Projectile* proj : projectiles) {
        proj->render(painter);
    }

    for (Enemy* enemy : enemies) {
        enemy->render(painter);
    }

    player->render(painter);

}

void Level::handleKeyPress(int key)
{
    if (gameOver) {
        return;
    }
    player->setKeyPressed(key, true);

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

        if (player->isAttacking() && attackBounds.intersects(enemyBounds)) {
            enemy->takeDamage(25);
            enemy->knockback(player->getPosition(), 200.0);
        }

        if (playerBounds.intersects(enemyBounds) && !player->isInvincible()) {
            player->takeDamage(enemy->getDamage());
            enemy->knockback(player->getPosition(), 150.0);
        }
    }

    for (Projectile* proj : projectiles) {
        if (!proj->isActive()) continue;

        QRectF projBounds(proj->getPosition(), proj->getSize());
        if (projBounds.intersects(playerBounds) && !player->isInvincible()) {
            player->takeDamage(proj->getDamage());
            proj->deactivate();
        }
    }
}

void Level::spawnEnemy()
{

    double x, y;
    int edge = QRandomGenerator::global()->bounded(4);

    switch (edge) {
    case 0:
        x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH);
        y = -32;
        break;
    case 1:
        x = VIRTUAL_WIDTH;
        y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT);
        break;
    case 2:
        x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH);
        y = VIRTUAL_HEIGHT;
        break;
    default:
        x = -32;
        y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT);
        break;
    }

    Enemy* enemy;
    if (QRandomGenerator::global()->bounded(100) < 70) {
        enemy = new VirusGreen(QPointF(x, y), QSizeF(32, 32));
    } else {
        enemy = new VirusViolet(QPointF(x, y), QSizeF(32, 32));
    }
    enemy->setTarget(player->getPositionPtr());
    enemies.append(enemy);
}

void Level::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{

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

    qDeleteAll(projectiles);
    projectiles.clear();

    player->setPosition(QPointF(400, 300));
    player->reset();
    player->clearKeys();

    gameOver = false;
    score = 0;
    spawnTimer = 0;
    accumulatedLoad = 0.0;

    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

void Level::updateProjectiles(double deltaTime)
{

    for (Projectile* proj : projectiles) {
        proj->update(deltaTime);
    }

    for (int i = projectiles.size() - 1; i >= 0; --i) {
        if (!projectiles[i]->isActive() ||
            projectiles[i]->isOutOfBounds(QSizeF(VIRTUAL_WIDTH, VIRTUAL_HEIGHT))) {
            delete projectiles[i];
            projectiles.removeAt(i);
        }
    }
}

void Level::spawnProjectile(VirusViolet* shooter)
{

    QPointF shooterCenter = shooter->getPosition() +
                            QPointF(shooter->getSize().width() / 2, shooter->getSize().height() / 2);

    QPointF direction = shooter->getShootDirection(
        player->getPosition() + QPointF(player->getSize().width() / 2, player->getSize().height() / 2)
    );

    shooter->shoot();

    Projectile* proj = new Projectile(shooterCenter, direction, 180.0);
    projectiles.append(proj);
}
