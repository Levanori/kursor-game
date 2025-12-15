#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "gamescene.h"
#include <QVector>
#include <QPixmap>
#include <memory>

class Player;
class DesktopScene;
class FolderScene;
class BossScene;
class MazeScene;
class FinalBossScene;

struct FolderProgress {
    bool unlocked = false;
    bool bossCompleted = false;
    bool mazeCompleted = false;
    bool stealthCompleted = false;
    bool fullyCompleted = false;
};

class GameManager
{
public:
    GameManager();
    ~GameManager();

    void initialize();

    void update(double deltaTime);
    void render(QPainter& painter);
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY);

    void handleKeyPress(int key);
    void handleKeyRelease(int key);
    void handleResize(int w, int h);

    void changeScene(SceneType type, int index = 0);
    void returnToPreviousScene();
    void returnToFolder(int folderIndex);
    void startBossActivity(int folderIndex);
    void startFinalBossActivity();

    void unlockFolder(int folderIndex);
    void completeActivity(SceneType activityType, int folderIndex);
    bool isFolderUnlocked(int index) const;
    bool isGameCompleted() const;
    const FolderProgress& getFolderProgress(int index) const;

    GameScene* getCurrentScene() const { return currentScene; }
    SceneType getCurrentSceneType() const;

    int getTotalScore() const { return totalScore; }
    bool isGameOver() const;
    void reset();

    int getScoreToUnlockFolder(int folderIndex) const;

    static const int TOTAL_FOLDERS = 3;

    void clearPlayerKeys();

private:
    void loadBackgrounds();
    void createScenes();
    void checkFolderUnlock();
    void updateSceneTransition();

    Player* player = nullptr;

    GameScene* currentScene = nullptr;
    GameScene* previousScene = nullptr;

    DesktopScene* desktopScene = nullptr;
    QVector<FolderScene*> folderScenes;
    QVector<BossScene*> bossScenes;
    QVector<MazeScene*> mazeScenes;
    FinalBossScene* finalBossScene = nullptr;

    FolderProgress folderProgress[TOTAL_FOLDERS];
    int totalScore = 0;
    int currentFolderIndex = -1;

    const int SCORE_PER_FOLDER = 200;

    QVector<QPixmap> backgrounds;

    int screenWidth = 725;
    int screenHeight = 450;

    bool transitioning = false;
    SceneType pendingSceneType;
    int pendingSceneIndex;
};

#endif
