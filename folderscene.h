#ifndef FOLDERSCENE_H
#define FOLDERSCENE_H

#include "gamescene.h"
#include "enemy.h"
#include "virusgreen.h"
#include "virusviolet.h"
#include "projectile.h"
#include <QVector>

class GameManager;
class FileIcon;

enum class FileType {
    Correct,
    WrongCpuLoad,
    WrongVirusSpawn
};

enum class FileActivityType {
    Boss,
    Maze,
    FinalBoss
};

class FolderScene : public GameScene
{
public:
    FolderScene(int folderIndex);
    ~FolderScene() override;

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) override;
    void handleKeyPress(int key) override;
    void handleKeyRelease(int key) override;
    void onEnter() override;
    void onExit() override;
    void reset() override;
    SceneType getType() const override { return SceneType::Folder; }

    void setGameManager(GameManager* gm) { gameManager = gm; }

    int getFolderIndex() const { return folderIndex; }

    void spawnFileIcons();

    bool areAllActivitiesCompleted() const;
    

    void markActivityCompleted(FileActivityType activityType);

private:
    void checkCollisions();
    void spawnEnemy();
    void updateProjectiles(double deltaTime);
    void spawnProjectile(VirusViolet* shooter);
    void updateFileIcons(double deltaTime);
    void checkFileInteraction();
    void checkBackInteraction();
    void handleWrongFileClick(FileType wrongType);
    void spawnVirusesAroundPlayer();
    void hideFileIcons();

    GameManager* gameManager = nullptr;
    int folderIndex;

    QVector<Enemy*> enemies;
    QVector<Projectile*> projectiles;

    QVector<FileIcon*> fileIcons;
    bool fileIconsSpawned = false;
    bool fileIconsHidden = false;
    int scoreToSpawnFiles;
    int scoreWhenHidden = 0;
    int scoreToRespawn = 200;

    const double LOAD_PER_SECOND = 2.5;
    const int LOAD_PER_KILL = 5;
    double accumulatedLoad = 0.0;

    double spawnTimer = 0;
    double spawnInterval = 1.8;

    FileIcon* nearbyFile = nullptr;
    bool canInteractWithFile = false;

    QRectF backButtonBounds;
    bool canGoBack = false;
};

#endif
