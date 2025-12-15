#ifndef FINALBOSSSCENE_H
#define FINALBOSSSCENE_H

#include "gamescene.h"
#include "enemy.h"
#include "projectile.h"
#include <QVector>
#include <QPixmap>
#include <QRectF>

class GameManager;
class VirusViolet;

struct BossFolder {
    QRectF bounds;
    QPixmap sprite;
    bool isActive = false;
    bool isCompleted = false;
    int scoreNeeded = 500;
    int currentScore = 0;
};

enum class FinalBossPhase {
    Intro,
    Attack,
    SpawnViruses,
    FolderPhase,
    Victory
};

class FinalBossScene : public GameScene
{
public:
    FinalBossScene(GameManager* manager);
    ~FinalBossScene() override;

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) override;
    void handleKeyPress(int key) override;
    void handleKeyRelease(int key) override;
    void onEnter() override;
    void onExit() override;
    void reset() override;
    SceneType getType() const override { return SceneType::FinalBoss; }

private:
    void loadSprites();
    void updateIntroPhase(double deltaTime);
    void updateAttackPhase(double deltaTime);
    void updateSpawnPhase(double deltaTime);
    void updateFolderPhase(double deltaTime);
    void updateVictoryPhase(double deltaTime);
    void updateEnemies(double deltaTime);
    void updateProjectiles(double deltaTime);
    void checkCollisions();
    void spawnEnemy();
    void spawnArenaProjectile();
    void spawnProjectile(VirusViolet* shooter);
    void spawnFolder();
    void enterFolder();
    void exitFolder();
    void clearEnemies();

    GameManager* m_gameManager = nullptr;
    

    QVector<QPixmap> m_bossFrames;
    int m_currentFrame = 0;
    double m_animTimer = 0.0;
    double m_frameTime = 0.1;
    

    QPixmap m_attackTop;
    QPixmap m_attackBottom;
    QPixmap m_attackCross1;
    QPixmap m_attackCross2;
    QPixmap m_currentAttack;
    double m_attackOpacity = 0.0;
    

    QPixmap m_folderSprite;
    

    QVector<Enemy*> m_enemies;
    

    QVector<Projectile*> m_projectiles;
    

    BossFolder m_currentFolder;
    int m_foldersCompleted = 0;
    static const int TOTAL_FOLDERS = 5;
    

    FinalBossPhase m_phase = FinalBossPhase::Intro;
    double m_phaseTimer = 0.0;
    double m_enemySpawnTimer = 0.0;
    double m_spawnInterval = 2.0;
    

    bool m_inFolder = false;
    

    QVector<Enemy*> m_folderEnemies;
    QVector<Projectile*> m_folderProjectiles;
    double m_folderEnemySpawnTimer = 0.0;
};

#endif
