#include "bossscene.h"
#include "player.h"
#include "gamemanager.h"
#include <QPainter>
#include <QDebug>

BossScene::BossScene(GameManager* manager, int folderIndex)
    : GameScene()
    , m_gameManager(manager)
    , m_folderIndex(folderIndex)
    , m_currentHandFrame(0)
    , m_handAnimTimer(0.0)
    , m_handAnimSpeed(0.08)
    , m_handsAnimating(true)
    , m_handsVisible(false)
    , m_currentHeadFrame(0)
    , m_headAnimTimer(0.0)
    , m_headAnimSpeed(0.1)
    , m_headVisible(false)
    , m_sceneTimer(0.0)
    , m_scenePhase(ScenePhase::NekoIntro)
    , m_bossDefeated(false)
    , m_victoryTimer(0.0)
{
    loadBackgroundLayers();
    loadHandsAnimation();
    loadHeadAnimation();

    qDebug() << "BossScene created for folder" << folderIndex;
}

BossScene::~BossScene()
{
}

void BossScene::loadBackgroundLayers()
{
    m_staticLayer = QPixmap(":/backgrounds/neko/static_layer.png");
    m_bottomLayer = QPixmap(":/backgrounds/neko/bottom_layer.png");
    
    if (m_staticLayer.isNull()) {
        qDebug() << "Warning: Could not load static layer";
    }
    if (m_bottomLayer.isNull()) {
        qDebug() << "Warning: Could not load bottom layer";
    }
}

void BossScene::loadHandsAnimation()
{

    QVector<int> handFrameOrder = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    
    for (int i : handFrameOrder) {
        QString path = QString(":/backgrounds/neko/hands/%1.png").arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            m_handsFrames.append(frame);
        } else {
            qDebug() << "Warning: Could not load hand frame" << path;
        }
    }
    
    qDebug() << "Loaded" << m_handsFrames.size() << "hand animation frames";
}

void BossScene::loadHeadAnimation()
{
    for (int i = 0; i <= 6; ++i) {
        QString path = QString(":/backgrounds/neko/head/%1.png").arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            m_headFrames.append(frame);
        } else {
            qDebug() << "Warning: Could not load head frame" << path;
        }
    }
    
    qDebug() << "Loaded" << m_headFrames.size() << "head animation frames";
}

void BossScene::update(double deltaTime)
{
    if (!player) return;
    
    player->update(deltaTime);
    m_sceneTimer += deltaTime;
    
    switch (m_scenePhase) {
        case ScenePhase::NekoIntro:
            updateAnimations(deltaTime);
            

            if (m_sceneTimer >= 1.0 && !m_handsVisible) {
                m_handsVisible = true;
                m_handsAnimating = true;
            }
            

            if (m_handsVisible && m_currentHandFrame >= m_handsFrames.size() - 1 && !m_handsAnimating) {
                if (!m_headVisible) {
                    m_headVisible = true;
                }
            }
            

            if (m_headVisible && m_currentHeadFrame >= m_headFrames.size() - 1) {
                m_scenePhase = ScenePhase::BossAppear;
                m_sceneTimer = 0.0;
                m_miniBoss.startAppear();
                qDebug() << "Neko animation complete, boss appearing";
            }
            break;
            
        case ScenePhase::BossAppear:
            updateAnimations(deltaTime);
            m_miniBoss.update(deltaTime);
            

            if (m_miniBoss.getPhase() >= MiniBoss::Phase::Idle) {
                m_scenePhase = ScenePhase::Battle;
                qDebug() << "Battle started!";
            }
            break;
            
        case ScenePhase::Battle:
            updateAnimations(deltaTime);
            updateBattle(deltaTime);
            break;
            
        case ScenePhase::Victory:
            m_victoryTimer += deltaTime;

            if (m_victoryTimer >= 2.0) {
                m_bossDefeated = true;
                sceneCompleted = true;
                result.completed = true;
                result.returnToPrevious = false;
                result.nextScene = SceneType::Desktop;
                result.nextSceneIndex = 0;
            }
            break;
            
        case ScenePhase::Defeated:
            gameOver = true;
            break;
    }
}

void BossScene::updateAnimations(double deltaTime)
{

    if (m_handsVisible && m_handsAnimating && !m_handsFrames.isEmpty()) {
        m_handAnimTimer += deltaTime;
        if (m_handAnimTimer >= m_handAnimSpeed) {
            m_handAnimTimer = 0.0;
            m_currentHandFrame++;
            if (m_currentHandFrame >= m_handsFrames.size()) {
                m_currentHandFrame = m_handsFrames.size() - 1;
                m_handsAnimating = false;
            }
        }
    }
    

    if (m_headVisible && !m_headFrames.isEmpty()) {
        m_headAnimTimer += deltaTime;
        if (m_headAnimTimer >= m_headAnimSpeed) {
            m_headAnimTimer = 0.0;
            if (m_currentHeadFrame < m_headFrames.size() - 1) {
                m_currentHeadFrame++;
            }
        }
    }
}

void BossScene::updateBattle(double deltaTime)
{

    if (player) {
        QPointF playerCenter = player->getPosition() + QPointF(player->getSize().width() / 2, 
                                                                player->getSize().height() / 2);
        m_miniBoss.setPlayerPosition(playerCenter);
        

        if (player->isCrashed()) {
            m_scenePhase = ScenePhase::Defeated;
            return;
        }
    }
    
    m_miniBoss.update(deltaTime);
    

    checkPlayerDamage();
    

    if (m_miniBoss.isDead()) {
        m_scenePhase = ScenePhase::Victory;
        m_victoryTimer = 0.0;
        score += 500;
        

        if (player) {
            player->setCpuLoad(0);
        }
        
        qDebug() << "Boss defeated! Victory!";
    }
}

void BossScene::checkPlayerDamage()
{
    if (!player) return;
    
    QPointF playerCenter = player->getPosition() + QPointF(player->getSize().width() / 2, 
                                                            player->getSize().height() / 2);
    

    if (m_miniBoss.checkAttackWaveHit(playerCenter)) {

        player->setCpuLoad(player->getCpuLoad() + 10);
        qDebug() << "Player hit by attack wave!";
    }
}

void BossScene::render(QPainter& painter)
{

    painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(20, 10, 30));
    

    renderBackground(painter);
    renderNekoAnimation(painter);
    

    if (m_scenePhase >= ScenePhase::BossAppear) {
        m_miniBoss.render(painter);
    }
    

    if (player) {
        player->render(painter);
    }
    

    if (m_scenePhase == ScenePhase::Victory) {
        painter.setPen(QPen(Qt::yellow, 2));
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(QRect(0, VIRTUAL_HEIGHT / 2 - 30, VIRTUAL_WIDTH, 60), 
                        Qt::AlignCenter, "VICTORY!");
    }
}

void BossScene::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{

    renderBaseUI(painter, scaleFactor, offsetX, offsetY, "BOSS FIGHT");

}

void BossScene::renderBackground(QPainter& painter)
{

    if (!m_staticLayer.isNull()) {
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, m_staticLayer);
    }
    

    if (!m_bottomLayer.isNull()) {
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, m_bottomLayer);
    }
}

void BossScene::renderNekoAnimation(QPainter& painter)
{

    if (m_headVisible && !m_headFrames.isEmpty() && m_currentHeadFrame < m_headFrames.size()) {
        const QPixmap& headFrame = m_headFrames[m_currentHeadFrame];
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, headFrame);
    }
    

    if (m_handsVisible && !m_handsFrames.isEmpty() && m_currentHandFrame < m_handsFrames.size()) {
        const QPixmap& handFrame = m_handsFrames[m_currentHandFrame];
        painter.drawPixmap(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, handFrame);
    }
}

void BossScene::handleKeyPress(int key)
{
    if (!player) return;
    
    if (key == Qt::Key_Escape) {

        if (m_gameManager) {
            m_gameManager->returnToFolder(m_folderIndex);
        }
        return;
    }
    

    if (key == Qt::Key_Space && m_scenePhase == ScenePhase::Battle) {

        player->attack();
        

        QRectF attackBounds = player->getAttackBounds();
        QRectF bossHitBox = m_miniBoss.getHitBox();
        
        if (attackBounds.intersects(bossHitBox)) {
            m_miniBoss.takeDamage(10);
            score += 10;
        }
    }
    
    player->setKeyPressed(key, true);
}

void BossScene::handleKeyRelease(int key)
{
    if (player) {
        player->setKeyPressed(key, false);
    }
}

void BossScene::reset()
{
    m_currentHandFrame = 0;
    m_handAnimTimer = 0.0;
    m_handsAnimating = true;
    m_handsVisible = false;
    m_currentHeadFrame = 0;
    m_headAnimTimer = 0.0;
    m_headVisible = false;
    m_sceneTimer = 0.0;
    m_scenePhase = ScenePhase::NekoIntro;
    m_bossDefeated = false;
    m_victoryTimer = 0.0;
    m_miniBoss.reset();
    sceneCompleted = false;
    gameOver = false;
    score = 0;
}
