#include "finalbossscene.h"
#include "gamemanager.h"
#include "player.h"
#include "virusgreen.h"
#include "virusviolet.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>

FinalBossScene::FinalBossScene(GameManager* manager)
    : GameScene()
    , m_gameManager(manager)
{
    loadSprites();
}

FinalBossScene::~FinalBossScene()
{
    clearEnemies();
}

void FinalBossScene::loadSprites()
{

    m_bossFrames.resize(40);
    for (int i = 0; i < 40; ++i) {
        QString path = QString(":/finalBoss/fb_%1.png").arg(i);
        QPixmap original(path);
        if (!original.isNull()) {
            m_bossFrames[i] = original.scaled(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, 
                                              Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    

    auto loadAttack = [this](const QString& path) -> QPixmap {
        QPixmap original(path);
        if (!original.isNull()) {
            return original.scaled(VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        return original;
    };
    
    m_attackTop = loadAttack(":/finalBoss/attack_top.png");
    m_attackBottom = loadAttack(":/finalBoss/attack_bottom.png");
    m_attackCross1 = loadAttack(":/finalBoss/attack_cross1.png");
    m_attackCross2 = loadAttack(":/finalBoss/attack_cross2.png");
    

    QPixmap folderOrig(":/sprites/assets/folderIcons/folder.png");
    if (!folderOrig.isNull()) {
        m_folderSprite = folderOrig.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    qDebug() << "FinalBoss sprites loaded. Frames:" << m_bossFrames.size();
}

void FinalBossScene::clearEnemies()
{
    qDeleteAll(m_enemies);
    m_enemies.clear();
    qDeleteAll(m_projectiles);
    m_projectiles.clear();
    qDeleteAll(m_folderEnemies);
    m_folderEnemies.clear();
    qDeleteAll(m_folderProjectiles);
    m_folderProjectiles.clear();
}

// Перевірка чи гравець знаходиться в зоні ураження активної хвилі
bool FinalBossScene::isPlayerInWaveDangerZone() const
{
    if (!player) return false;
    if (m_attackOpacity <= 0.7) return false; // Урон тільки коли хвиля майже повністю видима
    if (m_currentAttackType < 0) return false;
    
    QPointF playerCenter = player->getPosition() + QPointF(player->getSize().width()/2, player->getSize().height()/2);
    double centerX = VIRTUAL_WIDTH / 2.0;
    double centerY = VIRTUAL_HEIGHT / 2.0;
    
    // 0 = attack_top - верхня частина екрану (верхні 20%)
    if (m_currentAttackType == 0) {
        return playerCenter.y() < VIRTUAL_HEIGHT * 0.20;
    }
    
    // 1 = attack_bottom - нижня частина екрану (нижні 20%)
    if (m_currentAttackType == 1) {
        return playerCenter.y() > VIRTUAL_HEIGHT * 0.80;
    }
    
    // 2 = attack_cross1 - горизонтальна + вертикальна смуги (вузькі)
    if (m_currentAttackType == 2) {
        double horizontalWidth = 12.0;
        double verticalWidth = 10.0;
        bool inHorizontalBand = qAbs(playerCenter.y() - centerY) < horizontalWidth;
        bool inVerticalBand = qAbs(playerCenter.x() - centerX) < verticalWidth;
        return inHorizontalBand || inVerticalBand;
    }
    
    // 3 = attack_cross2 - дві діагоналі під 45° (вузькі)
    if (m_currentAttackType == 3) {
        double crossWidth = 10.0;
        double dx = playerCenter.x() - centerX;
        double dy = playerCenter.y() - centerY;
        double distToDiag1 = qAbs(dx - dy) / 1.414;
        double distToDiag2 = qAbs(dx + dy) / 1.414;
        return distToDiag1 < crossWidth || distToDiag2 < crossWidth;
    }
    
    return false;
}

void FinalBossScene::spawnArenaProjectile()
{

    if (!player) return;
    
    QPointF spawnPos;
    int edge = QRandomGenerator::global()->bounded(4);
    
    switch (edge) {
    case 0:
        spawnPos = QPointF(QRandomGenerator::global()->bounded(VIRTUAL_WIDTH), -10);
        break;
    case 1:
        spawnPos = QPointF(QRandomGenerator::global()->bounded(VIRTUAL_WIDTH), VIRTUAL_HEIGHT + 10);
        break;
    case 2:
        spawnPos = QPointF(-10, QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT));
        break;
    case 3:
        spawnPos = QPointF(VIRTUAL_WIDTH + 10, QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT));
        break;
    }
    
    QPointF playerCenter = player->getPosition() + QPointF(16, 16);
    QPointF direction = playerCenter - spawnPos;
    
    float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        direction /= length;
    }
    

    double spreadAngle = (QRandomGenerator::global()->bounded(30) - 15) * M_PI / 180.0;
    double newX = direction.x() * cos(spreadAngle) - direction.y() * sin(spreadAngle);
    double newY = direction.x() * sin(spreadAngle) + direction.y() * cos(spreadAngle);
    direction = QPointF(newX, newY);
    
    double speed = 150.0 + QRandomGenerator::global()->bounded(100);
    Projectile* proj = new Projectile(spawnPos, direction, speed);
    m_projectiles.append(proj);
}

void FinalBossScene::spawnEnemy()
{
    double x, y;
    int edge = QRandomGenerator::global()->bounded(4);
    
    switch (edge) {
    case 0: x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH); y = -40; break;
    case 1: x = QRandomGenerator::global()->bounded(VIRTUAL_WIDTH); y = VIRTUAL_HEIGHT + 10; break;
    case 2: x = -40; y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT); break;
    case 3: x = VIRTUAL_WIDTH + 10; y = QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT); break;
    }
    

    Enemy* enemy;
    if (QRandomGenerator::global()->bounded(10) < 6) {
        enemy = new VirusGreen(QPointF(x, y), QSizeF(32, 32));
    } else {
        enemy = new VirusViolet(QPointF(x, y), QSizeF(32, 32));
    }
    

    if (player) {
        enemy->setTarget(player->getPositionPtr());
    }
    
    if (m_inFolder) {
        m_folderEnemies.append(enemy);
    } else {
        m_enemies.append(enemy);
    }
}

void FinalBossScene::spawnProjectile(VirusViolet* shooter)
{
    if (!player || !shooter) return;
    
    QPointF shooterCenter = shooter->getPosition() + QPointF(16, 16);
    QPointF playerCenter = player->getPosition() + QPointF(16, 16);
    QPointF direction = playerCenter - shooterCenter;
    
    float length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        direction /= length;
    }
    
    Projectile* proj = new Projectile(shooterCenter, direction, 250.0);
    
    if (m_inFolder) {
        m_folderProjectiles.append(proj);
    } else {
        m_projectiles.append(proj);
    }
    
    shooter->shoot();
}

void FinalBossScene::updateEnemies(double deltaTime)
{
    QVector<Enemy*>& enemies = m_inFolder ? m_folderEnemies : m_enemies;
    
    for (Enemy* enemy : enemies) {
        enemy->update(deltaTime);
        

        VirusViolet* violet = dynamic_cast<VirusViolet*>(enemy);
        if (violet && violet->canShoot() && player) {
            QPointF enemyCenter = enemy->getPosition() + QPointF(16, 16);
            QPointF playerCenter = player->getPosition() + QPointF(16, 16);
            QPointF diff = playerCenter - enemyCenter;
            float distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
            
            if (distance >= 80 && distance <= 350) {
                spawnProjectile(violet);
            }
        }
    }
    

    for (int i = enemies.size() - 1; i >= 0; --i) {
        if (enemies[i]->isDead()) {
            if (player) player->decreaseLoad(5);
            score += 50;
            if (m_inFolder) {
                m_currentFolder.currentScore += 50;
            }
            delete enemies[i];
            enemies.removeAt(i);
        }
    }
}

void FinalBossScene::updateProjectiles(double deltaTime)
{
    QVector<Projectile*>& projectiles = m_inFolder ? m_folderProjectiles : m_projectiles;
    
    for (int i = projectiles.size() - 1; i >= 0; --i) {
        projectiles[i]->update(deltaTime);
        
        QPointF pos = projectiles[i]->getPosition();
        if (pos.x() < -50 || pos.x() > VIRTUAL_WIDTH + 50 ||
            pos.y() < -50 || pos.y() > VIRTUAL_HEIGHT + 50 ||
            !projectiles[i]->isActive()) {
            delete projectiles[i];
            projectiles.removeAt(i);
        }
    }
}

void FinalBossScene::checkCollisions()
{
    if (!player) return;
    
    QRectF playerBounds = player->getBounds();
    QRectF attackBounds = player->getAttackBounds();
    
    QVector<Enemy*>& enemies = m_inFolder ? m_folderEnemies : m_enemies;
    QVector<Projectile*>& projectiles = m_inFolder ? m_folderProjectiles : m_projectiles;
    
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

void FinalBossScene::update(double deltaTime)
{
    if (gameOver) return;
    

    m_animTimer += deltaTime;
    if (m_animTimer >= m_frameTime) {
        m_animTimer = 0.0;
        m_currentFrame = (m_currentFrame + 1) % m_bossFrames.size();
    }
    
    switch (m_phase) {
    case FinalBossPhase::Intro:
        updateIntroPhase(deltaTime);
        break;
    case FinalBossPhase::Attack:
        updateAttackPhase(deltaTime);
        break;
    case FinalBossPhase::SpawnViruses:
        updateSpawnPhase(deltaTime);
        break;
    case FinalBossPhase::FolderPhase:
        updateFolderPhase(deltaTime);
        break;
    case FinalBossPhase::Victory:
        updateVictoryPhase(deltaTime);
        break;
    }
}

void FinalBossScene::updateIntroPhase(double deltaTime)
{
    m_phaseTimer += deltaTime;
    
    if (m_phaseTimer >= 4.0) {
        m_phase = FinalBossPhase::Attack;
        m_phaseTimer = 0.0;
        
        m_currentAttackType = QRandomGenerator::global()->bounded(4);
        switch (m_currentAttackType) {
        case 0: m_currentAttack = m_attackTop; break;
        case 1: m_currentAttack = m_attackBottom; break;
        case 2: m_currentAttack = m_attackCross1; break;
        case 3: m_currentAttack = m_attackCross2; break;
        }
    }
}

void FinalBossScene::updateAttackPhase(double deltaTime)
{
    m_phaseTimer += deltaTime;
    // Швидше наростання прозорості атаки (0.8 сек)
    m_attackOpacity = qMin(1.0, m_phaseTimer / 0.8);
    
    // Дозволяємо гравцю рухатись під час фази атаки
    if (player) {
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
    
    // Обробка урону та CPU Load від хвилі - ТІЛЬКИ якщо гравець у зоні
    if (player && m_attackOpacity > 0.3 && isPlayerInWaveDangerZone()) {
        player->increaseLoad(static_cast<int>(15 * deltaTime));
        if (!player->isInvincible()) {
            player->takeDamage(5);
        }
    }
    
    // Перехід до фази спавну вірусів після 3 секунд
    if (m_phaseTimer >= 3.0) {
        m_phase = FinalBossPhase::SpawnViruses;
        m_phaseTimer = 0.0;
        m_attackOpacity = 1.0;
        

        // Створюємо початкові снаряди
        for (int i = 0; i < 8; ++i) {
            spawnArenaProjectile();
        }
        spawnFolder();
    }
}

void FinalBossScene::updateSpawnPhase(double deltaTime)
{
    m_phaseTimer += deltaTime;
    
    // Оновлення ворогів та снарядів
    updateEnemies(deltaTime);
    updateProjectiles(deltaTime);
    
    // Оновлення гравця
    if (player) {
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
        
        // Перевірка взаємодії з папкою
        if (m_currentFolder.isActive && !m_currentFolder.isCompleted) {
            if (player->getBounds().intersects(m_currentFolder.bounds)) {
                enterFolder();
                return;
            }
        }
    }
    
    checkCollisions();
    
    // Згасання початкової атаки (якщо не активна рандомна хвиля)
    if (!m_randomWaveActive) {
        m_attackOpacity = qMax(0.0, 1.0 - m_phaseTimer / 8.0);
    } else {
        // Згасання рандомної хвилі
        m_randomWaveTimer += deltaTime;
        m_attackOpacity = qMax(0.0, 1.0 - m_randomWaveTimer / 5.0);
        if (m_attackOpacity <= 0.01) {
            m_randomWaveActive = false;
            m_randomWaveTimer = 0.0;
        }
    }
    
    // Спавн снарядів
    m_enemySpawnTimer += deltaTime;
    if (m_enemySpawnTimer >= 0.5 && m_projectiles.size() < 20) {
        m_enemySpawnTimer = 0.0;
        spawnArenaProjectile();
    }
    
    // Випадкова поява хвилі атаки під час фази спавну (1% шанс)
    if (m_phaseTimer > 5.0 && !m_randomWaveActive && m_attackOpacity <= 0.1 && QRandomGenerator::global()->bounded(100) < 1) {
        m_currentAttackType = QRandomGenerator::global()->bounded(4);
        switch (m_currentAttackType) {
        case 0: m_currentAttack = m_attackTop; break;
        case 1: m_currentAttack = m_attackBottom; break;
        case 2: m_currentAttack = m_attackCross1; break;
        case 3: m_currentAttack = m_attackCross2; break;
        }
        m_attackOpacity = 1.0;
        m_randomWaveActive = true;
        m_randomWaveTimer = 0.0;
    }
    
    // Обробка урону від хвилі атаки під час спавну - ТІЛЬКИ в зоні
    if (player && m_attackOpacity > 0.3 && isPlayerInWaveDangerZone()) {
        player->increaseLoad(static_cast<int>(10 * deltaTime));
        if (!player->isInvincible()) {
            player->takeDamage(5);
        }
    }
}

void FinalBossScene::updateFolderPhase(double deltaTime)
{
    // Оновлення ворогів та снарядів
    updateEnemies(deltaTime);
    updateProjectiles(deltaTime);
    
    // Оновлення гравця
    if (player) {
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
    
    checkCollisions();
    
    // Спавн ворогів
    m_folderEnemySpawnTimer += deltaTime;
    if (m_folderEnemySpawnTimer >= 1.5 && m_folderEnemies.size() < 8) {
        m_folderEnemySpawnTimer = 0.0;
        spawnEnemy();
    }
    
    // Випадкова поява хвилі атаки під час фази папки (0.5% шанс)
    if (!m_randomWaveActive && m_attackOpacity <= 0.1 && QRandomGenerator::global()->bounded(1000) < 5) {
        m_currentAttackType = QRandomGenerator::global()->bounded(4);
        switch (m_currentAttackType) {
        case 0: m_currentAttack = m_attackTop; break;
        case 1: m_currentAttack = m_attackBottom; break;
        case 2: m_currentAttack = m_attackCross1; break;
        case 3: m_currentAttack = m_attackCross2; break;
        }
        m_attackOpacity = 1.0;
        m_randomWaveActive = true;
        m_randomWaveTimer = 0.0;
    }
    
    // Згасання хвилі атаки
    if (m_randomWaveActive && m_attackOpacity > 0.01) {
        m_randomWaveTimer += deltaTime;
        m_attackOpacity = qMax(0.0, 1.0 - m_randomWaveTimer / 5.0);
        if (m_attackOpacity <= 0.01) {
            m_randomWaveActive = false;
            m_randomWaveTimer = 0.0;
        }
        
        // Обробка урону від хвилі під час фази папки (використовуємо спільну функцію)
        if (isPlayerInWaveDangerZone()) {
            player->increaseLoad(static_cast<int>(8 * deltaTime));
        }
    }

    // Перевірка завершення папки
    if (m_currentFolder.currentScore >= m_currentFolder.scoreNeeded) {
        exitFolder();
    }
}

void FinalBossScene::updateVictoryPhase(double deltaTime)
{
    m_phaseTimer += deltaTime;
    
    if (m_phaseTimer >= 3.0) {
        sceneCompleted = true;
        result.completed = true;
        result.nextScene = SceneType::Desktop;
        result.nextSceneIndex = 0;
    }
}

void FinalBossScene::spawnFolder()
{
    double x = 100 + QRandomGenerator::global()->bounded(VIRTUAL_WIDTH - 200);
    double y = 100 + QRandomGenerator::global()->bounded(VIRTUAL_HEIGHT - 200);
    
    m_currentFolder.bounds = QRectF(x, y, 50, 50);
    m_currentFolder.sprite = m_folderSprite;
    m_currentFolder.isActive = true;
    m_currentFolder.isCompleted = false;
    m_currentFolder.currentScore = 0;
    m_currentFolder.scoreNeeded = 500;
}

void FinalBossScene::enterFolder()
{
    m_inFolder = true;
    m_phase = FinalBossPhase::FolderPhase;
    m_folderEnemies.clear();
    m_folderProjectiles.clear();
    m_folderEnemySpawnTimer = 0.0;
    m_currentFolder.currentScore = 0;
    

    for (int i = 0; i < 4; ++i) {
        spawnEnemy();
    }
    
    if (player) {
        player->setPosition(QPointF(VIRTUAL_WIDTH / 2 - 16, VIRTUAL_HEIGHT / 2 - 16));
    }
    
    qDebug() << "Entered folder" << (m_foldersCompleted + 1);
}

void FinalBossScene::exitFolder()
{
    m_inFolder = false;
    m_currentFolder.isCompleted = true;
    m_currentFolder.isActive = false;
    m_foldersCompleted++;
    
    qDeleteAll(m_folderEnemies);
    m_folderEnemies.clear();
    qDeleteAll(m_folderProjectiles);
    m_folderProjectiles.clear();
    
    qDebug() << "Folder completed! Total:" << m_foldersCompleted << "/" << TOTAL_FOLDERS;
    
    if (m_foldersCompleted >= TOTAL_FOLDERS) {
        m_phase = FinalBossPhase::Victory;
        m_phaseTimer = 0.0;
        
        if (player) {
            player->setCpuLoad(0);
        }
        
        qDebug() << "VICTORY! Final Boss defeated!";
    } else {
        m_phase = FinalBossPhase::Attack;
        m_phaseTimer = 0.0;
        
        qDeleteAll(m_enemies);
        m_enemies.clear();
        qDeleteAll(m_projectiles);
        m_projectiles.clear();
        
        m_currentAttackType = QRandomGenerator::global()->bounded(4);
        switch (m_currentAttackType) {
        case 0: m_currentAttack = m_attackTop; break;
        case 1: m_currentAttack = m_attackBottom; break;
        case 2: m_currentAttack = m_attackCross1; break;
        case 3: m_currentAttack = m_attackCross2; break;
        }
        m_attackOpacity = 0.0;
    }
}

void FinalBossScene::render(QPainter& painter)
{
    painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Qt::black);
    
    if (!m_inFolder) {

        if (m_currentFrame >= 0 && m_currentFrame < m_bossFrames.size() && !m_bossFrames[m_currentFrame].isNull()) {
            const QPixmap& frame = m_bossFrames[m_currentFrame];
            int x = (VIRTUAL_WIDTH - frame.width()) / 2;
            int y = (VIRTUAL_HEIGHT - frame.height()) / 2;
            painter.drawPixmap(x, y, frame);
        }
    }
    
    if (!m_inFolder) {

        if (!m_currentAttack.isNull() && m_attackOpacity > 0.01) {
            painter.setOpacity(m_attackOpacity);
            int x = (VIRTUAL_WIDTH - m_currentAttack.width()) / 2;
            int y = (VIRTUAL_HEIGHT - m_currentAttack.height()) / 2;
            painter.drawPixmap(x, y, m_currentAttack);
            painter.setOpacity(1.0);
        }
        

        for (Enemy* enemy : m_enemies) {
            enemy->render(painter);
        }
        

        for (Projectile* proj : m_projectiles) {
            proj->render(painter);
        }
        

        if (m_currentFolder.isActive && !m_currentFolder.isCompleted) {
            if (!m_currentFolder.sprite.isNull()) {
                painter.drawPixmap(
                    static_cast<int>(m_currentFolder.bounds.x()),
                    static_cast<int>(m_currentFolder.bounds.y()),
                    m_currentFolder.sprite);
            }
        }
    } else {

        painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(30, 30, 50));
        
        for (Enemy* enemy : m_folderEnemies) {
            enemy->render(painter);
        }
        
        for (Projectile* proj : m_folderProjectiles) {
            proj->render(painter);
        }
        

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        QString progress = QString("Folder %1/%2 - Score: %3/%4")
            .arg(m_foldersCompleted + 1)
            .arg(TOTAL_FOLDERS)
            .arg(m_currentFolder.currentScore)
            .arg(m_currentFolder.scoreNeeded);
        painter.drawText(10, 30, progress);
    }
    

    if (player) {
        player->render(painter);
    }
    

    if (m_phase == FinalBossPhase::Intro) {
        painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(0, 0, 0, 100));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(QRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT), 
                        Qt::AlignCenter, "FINAL BOSS");
    }
    

    if (m_phase == FinalBossPhase::Victory) {
        painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(0, 100, 0, 150));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(QRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT), 
                        Qt::AlignCenter, "VICTORY!\nGAME COMPLETED!");
    }
}

void FinalBossScene::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{
    QString hint;
    if (m_phase == FinalBossPhase::Intro) {
        hint = "Prepare for the Final Boss!";
    } else if (m_phase == FinalBossPhase::Attack) {
        // Надпис під час хвилі атаки
        hint = "INCOMING WAVE! Dodge the attack!";
    } else if (m_inFolder) {
        hint = QString("Kill viruses! %1/%2 points").arg(m_currentFolder.currentScore).arg(m_currentFolder.scoreNeeded);
    } else if (m_phase == FinalBossPhase::SpawnViruses) {
        hint = "Reach the folder! Avoid viruses!";
    } else if (m_phase == FinalBossPhase::Victory) {
        hint = "Congratulations!";
    }
    
    renderBaseUI(painter, scaleFactor, offsetX, offsetY, hint);
}

void FinalBossScene::handleKeyPress(int key)
{
    if (player) {
        player->setKeyPressed(key, true);
        

        if (key == Qt::Key_Space) {
            player->attack();
        }
    }
    
    if (key == Qt::Key_Escape && !m_inFolder && m_phase != FinalBossPhase::Victory) {
        gameOver = true;
        result.completed = false;
        result.returnToPrevious = true;
    }
}

void FinalBossScene::handleKeyRelease(int key)
{
    if (player) {
        player->setKeyPressed(key, false);
    }
}

void FinalBossScene::onEnter()
{
    sceneCompleted = false;
    gameOver = false;
    m_phase = FinalBossPhase::Intro;
    m_phaseTimer = 0.0;
    m_currentFrame = 0;
    m_animTimer = 0.0;
    m_foldersCompleted = 0;
    m_inFolder = false;
    m_attackOpacity = 0.0;
    m_currentAttackType = -1;
    m_enemySpawnTimer = 0.0;
    m_randomWaveTimer = 0.0;
    m_randomWaveActive = false;
    
    clearEnemies();
    
    m_currentFolder.isActive = false;
    m_currentFolder.isCompleted = false;
    
    if (player) {
        player->setPosition(QPointF(VIRTUAL_WIDTH / 2 - 16, VIRTUAL_HEIGHT - 60));
        player->clearKeys();
    }
}

void FinalBossScene::onExit()
{
    if (player) {
        player->clearKeys();
    }
}

void FinalBossScene::reset()
{
    clearEnemies();
    m_phase = FinalBossPhase::Intro;
    m_phaseTimer = 0.0;
    m_currentFrame = 0;
    m_animTimer = 0.0;
    m_foldersCompleted = 0;
    m_inFolder = false;
    m_attackOpacity = 0.0;
    m_currentAttackType = -1;
    m_enemySpawnTimer = 0.0;
    m_randomWaveTimer = 0.0;
    m_randomWaveActive = false;
    gameOver = false;
    sceneCompleted = false;
    score = 0;
    
    m_currentFolder.isActive = false;
    m_currentFolder.isCompleted = false;
    
    if (player) {
        player->reset();
        player->clearKeys();
    }
}
