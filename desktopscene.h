#ifndef DESKTOPSCENE_H
#define DESKTOPSCENE_H

#include "gamescene.h"
#include "enemy.h"
#include "virusgreen.h"
#include "virusviolet.h"
#include "projectile.h"
#include <QVector>

class GameManager;
class FolderIcon;

class DesktopScene : public GameScene
{
public:
    DesktopScene();
    ~DesktopScene() override;

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) override;
    void handleKeyPress(int key) override;
    void handleKeyRelease(int key) override;
    void onEnter() override;
    void onExit() override;
    void reset() override;
    SceneType getType() const override { return SceneType::Desktop; }

    void setGameManager(GameManager* gm) { gameManager = gm; }

    void spawnFolderIcon(int folderIndex);
    
    // Скидання очків та папок при заході в папку
    void resetScoreAndFolders();
    

    void removeFolderIcon(int folderIndex);

private:
    void checkCollisions();
    void spawnEnemy();
    void updateProjectiles(double deltaTime);
    void spawnProjectile(VirusViolet* shooter);
    void updateFolderIcons(double deltaTime);
    void checkFolderInteraction();

    GameManager* gameManager = nullptr;

    QVector<Enemy*> enemies;
    QVector<Projectile*> projectiles;

    QVector<FolderIcon*> folderIcons;

    const double LOAD_PER_SECOND = 2.0;
    const int LOAD_PER_KILL = 5;
    double accumulatedLoad = 0.0;

    double spawnTimer = 0;
    double spawnInterval = 2.0;

    FolderIcon* nearbyFolder = nullptr;
    bool canInteractWithFolder = false;
};

#endif
