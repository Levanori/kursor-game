#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QPainter>
#include <QPixmap>

class Player;

enum class SceneType {
    Desktop,
    Folder,
    Boss,
    Maze,
    Stealth,
    FinalBoss
};

struct SceneResult {
    bool completed = false;
    bool returnToPrevious = false;
    SceneType nextScene = SceneType::Desktop;
    int nextSceneIndex = 0;
};

class GameScene
{
public:
    GameScene();
    virtual ~GameScene() = default;

    virtual void update(double deltaTime) = 0;
    virtual void render(QPainter& painter) = 0;
    virtual void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) = 0;

    virtual void handleKeyPress(int key) = 0;
    virtual void handleKeyRelease(int key) = 0;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void reset() = 0;

    virtual bool isCompleted() const { return sceneCompleted; }
    virtual bool isGameOver() const { return gameOver; }
    virtual SceneResult getResult() const { return result; }

    virtual SceneType getType() const = 0;

    void handleResize(int w, int h);
    double getScaleFactor() const;
    int getVirtualWidth() const { return VIRTUAL_WIDTH; }
    int getVirtualHeight() const { return VIRTUAL_HEIGHT; }

    void setPlayer(Player* p) { player = p; }
    Player* getPlayer() const { return player; }

    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    void resetScore() { score = 0; }

    void setBackground(const QPixmap& bg) { background = bg; }
    const QPixmap& getBackground() const { return background; }

    void clearPlayerKeys();

    void renderBaseUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY, const QString& extraHint = "");

protected:
    Player* player = nullptr;

    bool sceneCompleted = false;
    bool gameOver = false;
    SceneResult result;
    int score = 0;

    QPixmap background;

    const int VIRTUAL_WIDTH = 725;
    const int VIRTUAL_HEIGHT = 450;
    int currentScreenWidth = VIRTUAL_WIDTH;
    int currentScreenHeight = VIRTUAL_HEIGHT;
};

#endif
