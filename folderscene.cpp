#include "folderscene.h"
#include "gamemanager.h"
#include "player.h"
#include "fileicon.h"
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>

FolderScene::FolderScene(int index)
    : GameScene()
    , folderIndex(index)
{

    backButtonBounds = QRectF(10, VIRTUAL_HEIGHT - 40, 60, 30);
    

    scoreToSpawnFiles = (folderIndex + 1) * 100;
    scoreToRespawn = 100;
}

FolderScene::~FolderScene()
{
    qDeleteAll(enemies);
    qDeleteAll(projectiles);
    qDeleteAll(fileIcons);
}

void FolderScene::update(double deltaTime)
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
        QSizeF psize = player->getSize();
        pos.setX(qBound(0.0, pos.x(), (double)VIRTUAL_WIDTH - psize.width()));
        pos.setY(qBound(0.0, pos.y(), (double)VIRTUAL_HEIGHT - psize.height()));
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

    if (!fileIconsSpawned && score >= scoreToSpawnFiles) {
        spawnFileIcons();
        fileIconsSpawned = true;
        fileIconsHidden = false;
    }
    

    if (fileIconsHidden && score >= scoreWhenHidden + scoreToRespawn) {

        for (FileIcon* file : fileIcons) {
            file->setVisible(true);
            file->setInteractable(true);
        }
        fileIconsHidden = false;
    }

    updateFileIcons(deltaTime);

    checkFileInteraction();

    checkBackInteraction();
}

void FolderScene::render(QPainter& painter)
{

    if (!background.isNull()) {
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, background);
    } else {

        painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(40, 40, 60));
    }

    for (Projectile* proj : projectiles) {
        proj->render(painter);
    }

    for (FileIcon* file : fileIcons) {
        file->render(painter);
    }

    for (Enemy* enemy : enemies) {
        enemy->render(painter);
    }

    if (player) {
        player->render(painter);
    }

    if (canGoBack || areAllActivitiesCompleted()) {
        painter.setBrush(canGoBack ? QColor(100, 200, 100) : QColor(80, 80, 100));
        painter.setPen(canGoBack ? QPen(Qt::yellow, 2) : QPen(Qt::gray, 1));
        painter.drawRect(backButtonBounds);
        
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(backButtonBounds, Qt::AlignCenter, "← Back");
    }
}

void FolderScene::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{

    QString hint;
    if (!fileIconsSpawned) {
        hint = QString("Folder %1 | Collect %2 pts for activities").arg(folderIndex + 1).arg(scoreToSpawnFiles);
    } else {
        hint = QString("Folder %1").arg(folderIndex + 1);
    }

    renderBaseUI(painter, scaleFactor, offsetX, offsetY, hint);
}

void FolderScene::handleKeyPress(int key)
{
    if (gameOver) return;

    if (player) {
        player->setKeyPressed(key, true);

        if (key == Qt::Key_Space) {

            if (canInteractWithFile && nearbyFile && !nearbyFile->isCompleted()) {
                if (nearbyFile->isCorrectFile()) {

                    sceneCompleted = true;
                    result.completed = true;
                    
                    switch (nearbyFile->getActivityType()) {
                    case FileActivityType::Boss:
                        result.nextScene = SceneType::Boss;
                        break;
                    case FileActivityType::Maze:
                        result.nextScene = SceneType::Maze;
                        break;
                    case FileActivityType::FinalBoss:
                        result.nextScene = SceneType::FinalBoss;
                        break;
                    }
                    result.nextSceneIndex = folderIndex;
                    return;
                } else {

                    handleWrongFileClick(nearbyFile->getFileType());
                    return;
                }
            }

            if (canGoBack && areAllActivitiesCompleted()) {
                sceneCompleted = true;
                result.completed = true;
                result.returnToPrevious = true;
                result.nextScene = SceneType::Desktop;
                return;
            }

            player->attack();
        }
    }
}

void FolderScene::handleKeyRelease(int key)
{
    if (player) {
        player->setKeyPressed(key, false);
    }
}

void FolderScene::onEnter()
{
    sceneCompleted = false;
    gameOver = false;

    

    score = 0;
    fileIconsSpawned = false;
    fileIconsHidden = false;
    scoreWhenHidden = 0;
    

    qDeleteAll(fileIcons);
    fileIcons.clear();
    nearbyFile = nullptr;
    canInteractWithFile = false;

    if (player) {
        player->setPosition(QPointF(VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2));
    }

    for (int i = 0; i < 2; ++i) {
        spawnEnemy();
    }
}

void FolderScene::onExit()
{
    if (player) {
        player->clearKeys();
    }
}

void FolderScene::reset()
{
    qDeleteAll(enemies);
    enemies.clear();

    qDeleteAll(projectiles);
    projectiles.clear();

    qDeleteAll(fileIcons);
    fileIcons.clear();

    if (player) {
        player->setPosition(QPointF(VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2));
        player->reset();
        player->clearKeys();
    }

    gameOver = false;
    sceneCompleted = false;
    score = 0;
    spawnTimer = 0;
    accumulatedLoad = 0.0;
    fileIconsSpawned = false;
    fileIconsHidden = false;
    scoreWhenHidden = 0;
    nearbyFile = nullptr;
    canInteractWithFile = false;
    canGoBack = false;
}

void FolderScene::checkCollisions()
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

void FolderScene::spawnEnemy()
{
    double x, y;
    int edge = QRandomGenerator::global()->bounded(4);

    switch (edge) {
    case 0: x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH); y = -32; break;
    case 1: x = VIRTUAL_WIDTH; y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT); break;
    case 2: x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH); y = VIRTUAL_HEIGHT; break;
    default: x = -32; y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT); break;
    }

    Enemy* enemy;
    if (QRandomGenerator::global()->bounded(100) < 60) {
        enemy = new VirusGreen(QPointF(x, y), QSizeF(32, 32));
    } else {
        enemy = new VirusViolet(QPointF(x, y), QSizeF(32, 32));
    }

    if (player) {
        enemy->setTarget(player->getPositionPtr());
    }
    enemies.append(enemy);
}

void FolderScene::updateProjectiles(double deltaTime)
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

void FolderScene::spawnProjectile(VirusViolet* shooter)
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

void FolderScene::spawnFileIcons()
{

    qDeleteAll(fileIcons);
    fileIcons.clear();
    

    int margin = 60;
    int iconSize = 48;

    QVector<QPointF> positions;

    for (int i = 0; i < 3; ++i) {
        QPointF pos;
        bool validPosition = false;
        int attempts = 0;

        while (!validPosition && attempts < 50) {
            double x = margin + QRandomGenerator::global()->bounded(VIRTUAL_WIDTH - margin * 2 - iconSize);
            double y = margin + QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT - margin * 2 - iconSize);
            pos = QPointF(x, y);

            validPosition = true;
            for (const QPointF& existingPos : positions) {
                double dist = qSqrt(qPow(pos.x() - existingPos.x(), 2) + qPow(pos.y() - existingPos.y(), 2));
                if (dist < iconSize * 2) {
                    validPosition = false;
                    break;
                }
            }
            attempts++;
        }

        positions.append(pos);
    }

    FileActivityType correctActivity;
    switch (folderIndex) {
    case 0:
        correctActivity = FileActivityType::Boss;
        break;
    case 1:
        correctActivity = FileActivityType::Maze;
        break;
    case 2:
    default:
        correctActivity = FileActivityType::FinalBoss;
        break;
    }
    

    int correctIndex = QRandomGenerator::global()->bounded(3);
    
    for (int i = 0; i < 3; ++i) {
        FileIcon* icon;
        if (i == correctIndex) {

            icon = new FileIcon(positions[i], correctActivity, folderIndex);
        } else {

            FileType wrongType = (i < correctIndex) ? 
                ((i % 2 == 0) ? FileType::WrongCpuLoad : FileType::WrongVirusSpawn) :
                ((i % 2 == 1) ? FileType::WrongCpuLoad : FileType::WrongVirusSpawn);
            icon = new FileIcon(positions[i], wrongType, folderIndex);
        }
        fileIcons.append(icon);
    }
}

void FolderScene::updateFileIcons(double deltaTime)
{
    for (FileIcon* file : fileIcons) {
        file->update(deltaTime);
    }
}

void FolderScene::checkFileInteraction()
{
    if (!player) return;

    nearbyFile = nullptr;
    canInteractWithFile = false;

    QRectF playerBounds = player->getBounds();
    QRectF interactionBounds = playerBounds.adjusted(-20, -20, 20, 20);

    for (FileIcon* file : fileIcons) {
        file->setHighlighted(false);

        if (!file->isInteractable()) continue;

        if (interactionBounds.intersects(file->getBounds())) {
            nearbyFile = file;
            canInteractWithFile = true;
            file->setHighlighted(true);
            break;
        }
    }
}

void FolderScene::checkBackInteraction()
{
    if (!player) return;

    QRectF playerBounds = player->getBounds();
    canGoBack = playerBounds.intersects(backButtonBounds) && areAllActivitiesCompleted();
}

bool FolderScene::areAllActivitiesCompleted() const
{
    if (fileIcons.isEmpty()) return false;

    for (FileIcon* file : fileIcons) {
        if (file->isCorrectFile() && !file->isCompleted()) {
            return false;
        }
    }
    return true;
}

void FolderScene::handleWrongFileClick(FileType wrongType)
{
    if (wrongType == FileType::WrongCpuLoad) {

        if (player) {
            player->setCpuLoad(player->getCpuLoad() + 20);
        }
        qDebug() << "Wrong file! CPU Load increased!";
    } else if (wrongType == FileType::WrongVirusSpawn) {

        spawnVirusesAroundPlayer();
        qDebug() << "Wrong file! Viruses spawned!";
    }
    

    hideFileIcons();
}

void FolderScene::spawnVirusesAroundPlayer()
{
    if (!player) return;
    
    QPointF playerCenter = player->getPosition() + QPointF(player->getSize().width() / 2, 
                                                            player->getSize().height() / 2);
    

    const int virusCount = 6;
    const double radius = 80.0;
    
    for (int i = 0; i < virusCount; ++i) {
        double angle = (2.0 * M_PI * i) / virusCount;
        double x = playerCenter.x() + radius * cos(angle) - 16;
        double y = playerCenter.y() + radius * sin(angle) - 16;
        

        x = qBound(0.0, x, (double)VIRTUAL_WIDTH - 32);
        y = qBound(0.0, y, (double)VIRTUAL_HEIGHT - 32);
        
        Enemy* virus;
        if (i % 2 == 0) {
            virus = new VirusGreen(QPointF(x, y), QSizeF(32, 32));
        } else {
            virus = new VirusViolet(QPointF(x, y), QSizeF(32, 32));
        }
        
        virus->setTarget(player->getPositionPtr());
        enemies.append(virus);
    }
}

void FolderScene::hideFileIcons()
{
    scoreWhenHidden = score;
    fileIconsHidden = true;
    
    for (FileIcon* file : fileIcons) {
        file->setVisible(false);
        file->setInteractable(false);
    }
    
    nearbyFile = nullptr;
    canInteractWithFile = false;
}

void FolderScene::markActivityCompleted(FileActivityType activityType)
{

    for (FileIcon* file : fileIcons) {
        if (file->isCorrectFile() && file->getActivityType() == activityType) {
            file->setCompleted(true);
            break;
        }
    }
}
