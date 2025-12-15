#include "desktopscene.h"
#include "gamemanager.h"
#include "player.h"
#include "foldericon.h"
#include <QRandomGenerator>
#include <QtMath>

DesktopScene::DesktopScene()
    : GameScene()
{
}

DesktopScene::~DesktopScene()
{
    qDeleteAll(enemies);
    qDeleteAll(projectiles);
    qDeleteAll(folderIcons);
}

void DesktopScene::update(double deltaTime)
{
    if (gameOver) return;

    if (player) {

        accumulatedLoad += LOAD_PER_SECOND * deltaTime;
        if (accumulatedLoad >= 1.0) {
            int loadIncrease = (int)accumulatedLoad;
            player->increaseLoad(loadIncrease);
            accumulatedLoad -= loadIncrease;
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
    }

    for (Enemy* enemy : enemies) {
        enemy->update(deltaTime);

        VirusViolet* violet = dynamic_cast<VirusViolet*>(enemy);
        if (violet && violet->canShoot() && player) {
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

    updateFolderIcons(deltaTime);

    checkFolderInteraction();
}

void DesktopScene::render(QPainter& painter)
{

    if (!background.isNull()) {
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, background);
    }

    for (Projectile* proj : projectiles) {
        proj->render(painter);
    }

    for (FolderIcon* folder : folderIcons) {
        folder->render(painter);
    }

    for (Enemy* enemy : enemies) {
        enemy->render(painter);
    }

    if (player) {
        player->render(painter);
    }
}

void DesktopScene::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{

    QString hint;
    if (gameManager) {
        int nextFolderScore = gameManager->getScoreToUnlockFolder(folderIcons.size());
        if (folderIcons.size() < GameManager::TOTAL_FOLDERS && score < nextFolderScore) {
            hint = QString("Next folder: %1 pts").arg(nextFolderScore);
        }
    }

    renderBaseUI(painter, scaleFactor, offsetX, offsetY, hint);
}

void DesktopScene::handleKeyPress(int key)
{
    if (gameOver) return;

    if (player) {
        player->setKeyPressed(key, true);

        if (key == Qt::Key_Space) {

            if (canInteractWithFolder && nearbyFolder) {
                // Зберігаємо індекс папки ПЕРЕД скиданням
                int folderIndex = nearbyFolder->getFolderIndex();
                
                // Скидаємо очки та приховуємо папки при вході в папку
                resetScoreAndFolders();
                
                sceneCompleted = true;
                result.completed = true;
                result.nextScene = SceneType::Folder;
                result.nextSceneIndex = folderIndex;
                return;
            }

            player->attack();
        }
    }
}

void DesktopScene::handleKeyRelease(int key)
{
    if (player) {
        player->setKeyPressed(key, false);
    }
}

void DesktopScene::onEnter()
{
    sceneCompleted = false;
    gameOver = false;

}

void DesktopScene::onExit()
{
    if (player) {
        player->clearKeys();
    }
}

void DesktopScene::reset()
{
    qDeleteAll(enemies);
    enemies.clear();

    qDeleteAll(projectiles);
    projectiles.clear();

    qDeleteAll(folderIcons);
    folderIcons.clear();

    if (player) {
        player->setPosition(QPointF(400, 300));
        player->reset();
        player->clearKeys();
    }

    gameOver = false;
    sceneCompleted = false;
    score = 0;
    spawnTimer = 0;
    accumulatedLoad = 0.0;
    nearbyFolder = nullptr;
    canInteractWithFolder = false;

    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

void DesktopScene::checkCollisions()
{
    if (!player) return;

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

void DesktopScene::spawnEnemy()
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

    if (player) {
        enemy->setTarget(player->getPositionPtr());
    }
    enemies.append(enemy);
}

void DesktopScene::updateProjectiles(double deltaTime)
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

void DesktopScene::spawnProjectile(VirusViolet* shooter)
{
    if (!player) return;

    QPointF shooterCenter = shooter->getPosition() +
                            QPointF(shooter->getSize().width() / 2, shooter->getSize().height() / 2);

    QPointF direction = shooter->getShootDirection(
        player->getPosition() + QPointF(player->getSize().width() / 2, player->getSize().height() / 2)
    );

    shooter->shoot();

    Projectile* proj = new Projectile(shooterCenter, direction, 180.0);
    projectiles.append(proj);
}

void DesktopScene::spawnFolderIcon(int folderIndex)
{

    for (FolderIcon* folder : folderIcons) {
        if (folder->getFolderIndex() == folderIndex) {
            return;
        }
    }

    int margin = 60;
    double x = margin + QRandomGenerator::global()->bounded(VIRTUAL_WIDTH - margin * 2);
    double y = margin + QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT - margin * 2);

    FolderIcon* folder = new FolderIcon(QPointF(x, y), folderIndex);
    folderIcons.append(folder);
}

void DesktopScene::removeFolderIcon(int folderIndex)
{
    for (int i = folderIcons.size() - 1; i >= 0; --i) {
        if (folderIcons[i]->getFolderIndex() == folderIndex) {
            delete folderIcons[i];
            folderIcons.removeAt(i);
            break;
        }
    }
}

void DesktopScene::updateFolderIcons(double deltaTime)
{
    for (FolderIcon* folder : folderIcons) {
        folder->update(deltaTime);
    }
}

void DesktopScene::checkFolderInteraction()
{
    if (!player) return;

    nearbyFolder = nullptr;
    canInteractWithFolder = false;

    QRectF playerBounds = player->getBounds();

    QRectF interactionBounds = playerBounds.adjusted(-20, -20, 20, 20);

    for (FolderIcon* folder : folderIcons) {
        if (!folder->isInteractable()) continue;

        folder->setHighlighted(false);

        if (interactionBounds.intersects(folder->getBounds())) {
            nearbyFolder = folder;
            canInteractWithFolder = true;
            folder->setHighlighted(true);
            break;
        }
    }
}

void DesktopScene::resetScoreAndFolders()
{
    // Скидаємо очки при заході в папку
    score = 0;
    
    // Видаляємо всі папки з екрану (вони з'являться знову при наборі очків)
    qDeleteAll(folderIcons);
    folderIcons.clear();
    
    nearbyFolder = nullptr;
    canInteractWithFolder = false;
}
