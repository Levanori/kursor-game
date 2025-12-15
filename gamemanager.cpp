#include "gamemanager.h"
#include "player.h"
#include "desktopscene.h"
#include "folderscene.h"
#include "bossscene.h"
#include "mazescene.h"
#include "finalbossscene.h"
#include <QDebug>

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
    delete desktopScene;
    qDeleteAll(folderScenes);
    qDeleteAll(bossScenes);
    qDeleteAll(mazeScenes);
    delete finalBossScene;
    delete player;
}

void GameManager::initialize()
{

    player = new Player(QPointF(400, 300), QSizeF(32, 32));

    loadBackgrounds();

    createScenes();

    currentScene = desktopScene;
    currentScene->onEnter();
}

void GameManager::loadBackgrounds()
{

    backgrounds.append(QPixmap(":/sprites/assets/backgrounds/desktop1_bg.svg"));

    backgrounds.append(QPixmap(":/sprites/assets/backgrounds/folder1_bg.png"));
    backgrounds.append(QPixmap(":/sprites/assets/backgrounds/folder2_bg.png"));
    backgrounds.append(QPixmap(":/sprites/assets/backgrounds/folder3_bg.png"));
}

void GameManager::createScenes()
{

    desktopScene = new DesktopScene();
    desktopScene->setPlayer(player);
    desktopScene->setGameManager(this);

    if (!backgrounds.isEmpty()) {
        desktopScene->setBackground(backgrounds[0]);
    }

    for (int i = 0; i < TOTAL_FOLDERS; ++i) {
        FolderScene* folder = new FolderScene(i);
        folder->setPlayer(player);
        folder->setGameManager(this);

        if (i + 1 < backgrounds.size()) {
            folder->setBackground(backgrounds[i + 1]);
        }

        folderScenes.append(folder);
        

        BossScene* boss = new BossScene(this, i);
        boss->setPlayer(player);
        bossScenes.append(boss);
        

        MazeScene* maze = new MazeScene(this, i);
        maze->setPlayer(player);
        mazeScenes.append(maze);
    }
    

    finalBossScene = new FinalBossScene(this);
    finalBossScene->setPlayer(player);
}

void GameManager::update(double deltaTime)
{
    if (!currentScene) return;

    currentScene->update(deltaTime);

    checkFolderUnlock();

    if (currentScene->isCompleted()) {
        SceneResult result = currentScene->getResult();
        

        if (currentScene->getType() == SceneType::Boss && result.completed && !currentScene->isGameOver()) {
            completeActivity(SceneType::Boss, currentFolderIndex);
            

            if (currentFolderIndex >= 0 && currentFolderIndex < folderScenes.size()) {
                FolderScene* folder = folderScenes[currentFolderIndex];
                folder->markActivityCompleted(FileActivityType::Boss);
            }
            

            if (desktopScene) {
                desktopScene->removeFolderIcon(currentFolderIndex);
                desktopScene->resetScore();
            }
        }
        

        if (currentScene->getType() == SceneType::Maze && result.completed && !currentScene->isGameOver()) {
            completeActivity(SceneType::Maze, currentFolderIndex);
            

            if (currentFolderIndex >= 0 && currentFolderIndex < folderScenes.size()) {
                FolderScene* folder = folderScenes[currentFolderIndex];
                folder->markActivityCompleted(FileActivityType::Maze);
            }
            

            if (desktopScene) {
                desktopScene->removeFolderIcon(currentFolderIndex);
                desktopScene->resetScore();
            }
        }
        

        if (currentScene->getType() == SceneType::FinalBoss && result.completed && !currentScene->isGameOver()) {

            if (TOTAL_FOLDERS > 2) {
                completeActivity(SceneType::FinalBoss, 2);
                
                if (folderScenes.size() > 2) {
                    FolderScene* folder = folderScenes[2];
                    folder->markActivityCompleted(FileActivityType::FinalBoss);
                }
                
                if (desktopScene) {
                    desktopScene->removeFolderIcon(2);
                    desktopScene->resetScore();
                }
            }
            
            qDebug() << "GAME COMPLETED! Final Boss defeated!";
        }
        
        if (result.returnToPrevious) {
            returnToPreviousScene();
        } else {
            changeScene(result.nextScene, result.nextSceneIndex);
        }
    }

    totalScore = currentScene->getScore();
}

void GameManager::render(QPainter& painter)
{
    if (currentScene) {
        currentScene->render(painter);
    }
}

void GameManager::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{
    if (currentScene) {
        currentScene->renderUI(painter, scaleFactor, offsetX, offsetY);
    }
}

void GameManager::handleKeyPress(int key)
{
    if (currentScene) {
        currentScene->handleKeyPress(key);
    }
}

void GameManager::handleKeyRelease(int key)
{
    if (currentScene) {
        currentScene->handleKeyRelease(key);
    }
}

void GameManager::handleResize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;

    if (currentScene) {
        currentScene->handleResize(w, h);
    }
}

void GameManager::changeScene(SceneType type, int index)
{
    if (currentScene) {
        currentScene->onExit();
        previousScene = currentScene;
    }

    switch (type) {
    case SceneType::Desktop:
        currentScene = desktopScene;
        currentFolderIndex = -1;
        break;

    case SceneType::Folder:
        if (index >= 0 && index < folderScenes.size()) {
            currentScene = folderScenes[index];
            currentFolderIndex = index;
        }
        break;

    case SceneType::Boss:
        if (index >= 0 && index < bossScenes.size()) {
            currentScene = bossScenes[index];
            currentFolderIndex = index;
        }
        break;

    case SceneType::Maze:
        if (index >= 0 && index < mazeScenes.size()) {
            currentScene = mazeScenes[index];
            currentFolderIndex = index;
        }
        break;
        
    case SceneType::FinalBoss:
        if (finalBossScene) {
            currentScene = finalBossScene;
            currentFolderIndex = -1;
        }
        break;
        
    case SceneType::Stealth:

        break;
    }

    if (currentScene) {
        currentScene->handleResize(screenWidth, screenHeight);
        currentScene->onEnter();
    }
}

void GameManager::returnToFolder(int folderIndex)
{
    if (folderIndex >= 0 && folderIndex < folderScenes.size()) {
        if (currentScene) {
            currentScene->onExit();
        }
        previousScene = currentScene;
        currentScene = folderScenes[folderIndex];
        currentFolderIndex = folderIndex;
        currentScene->handleResize(screenWidth, screenHeight);
        currentScene->onEnter();
    }
}

void GameManager::startBossActivity(int folderIndex)
{
    changeScene(SceneType::Boss, folderIndex);
}

void GameManager::startFinalBossActivity()
{
    changeScene(SceneType::FinalBoss, 0);
}

void GameManager::returnToPreviousScene()
{
    if (previousScene) {
        if (currentScene) {
            currentScene->onExit();
        }
        currentScene = previousScene;
        previousScene = nullptr;
        currentScene->onEnter();
    }
}

void GameManager::unlockFolder(int folderIndex)
{
    if (folderIndex >= 0 && folderIndex < TOTAL_FOLDERS) {
        folderProgress[folderIndex].unlocked = true;
    }
}

void GameManager::completeActivity(SceneType activityType, int folderIndex)
{
    if (folderIndex < 0 || folderIndex >= TOTAL_FOLDERS) return;

    switch (activityType) {
    case SceneType::Boss:
        folderProgress[folderIndex].bossCompleted = true;
        break;
    case SceneType::Maze:
        folderProgress[folderIndex].mazeCompleted = true;
        break;
    case SceneType::Stealth:
        folderProgress[folderIndex].stealthCompleted = true;
        break;
    default:
        break;
    }

    FolderProgress& fp = folderProgress[folderIndex];
    if (fp.bossCompleted && fp.mazeCompleted && fp.stealthCompleted) {
        fp.fullyCompleted = true;
    }
}

bool GameManager::isFolderUnlocked(int index) const
{
    if (index >= 0 && index < TOTAL_FOLDERS) {
        return folderProgress[index].unlocked;
    }
    return false;
}

bool GameManager::isGameCompleted() const
{

    for (int i = 0; i < TOTAL_FOLDERS; ++i) {
        if (!folderProgress[i].fullyCompleted) {
            return false;
        }
    }
    return true;
}

const FolderProgress& GameManager::getFolderProgress(int index) const
{
    static FolderProgress empty;
    if (index >= 0 && index < TOTAL_FOLDERS) {
        return folderProgress[index];
    }
    return empty;
}

SceneType GameManager::getCurrentSceneType() const
{
    if (currentScene) {
        return currentScene->getType();
    }
    return SceneType::Desktop;
}

bool GameManager::isGameOver() const
{
    if (currentScene) {
        return currentScene->isGameOver();
    }
    return false;
}

void GameManager::reset()
{

    for (int i = 0; i < TOTAL_FOLDERS; ++i) {
        folderProgress[i] = FolderProgress();
    }
    totalScore = 0;
    currentFolderIndex = -1;

    if (player) {
        player->reset();
        player->setPosition(QPointF(400, 300));
    }

    if (desktopScene) {
        desktopScene->reset();
    }
    for (auto* folder : folderScenes) {
        folder->reset();
    }
    for (auto* boss : bossScenes) {
        boss->reset();
    }
    for (auto* maze : mazeScenes) {
        maze->reset();
    }

    currentScene = desktopScene;
    previousScene = nullptr;
    if (currentScene) {
        currentScene->onEnter();
    }
}

int GameManager::getScoreToUnlockFolder(int folderIndex) const
{
    return (folderIndex + 1) * SCORE_PER_FOLDER;
}

void GameManager::checkFolderUnlock()
{

    for (int i = 0; i < TOTAL_FOLDERS; ++i) {
        if (!folderProgress[i].unlocked && totalScore >= getScoreToUnlockFolder(i)) {
            unlockFolder(i);
            

            if (desktopScene) {
                desktopScene->spawnFolderIcon(i);
            }
        }
    }
}

void GameManager::clearPlayerKeys()
{
    if (currentScene) {
        currentScene->clearPlayerKeys();
    }
}
