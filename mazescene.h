#ifndef MAZESCENE_H
#define MAZESCENE_H

#include "gamescene.h"
#include <QVector>
#include <QPixmap>
#include <QRectF>

class GameManager;

enum class MazeElementType {
    None,
    Reload
};

struct MazeWall {
    QRectF bounds;
    int gridX;
    int gridY;
};

struct VirusMine {
    QRectF bounds;
    QPixmap sprite;
    bool active = true;
};

struct MazeElement {
    MazeElementType type;
    QRectF bounds;
    QPixmap sprite;
    double animTimer = 0.0;
    

    int moveAxis = 0;
    int moveDirection = 1;
    double moveSpeed = 40.0;
    double minPos = 0.0;
    double maxPos = 0.0;
};

class MazeScene : public GameScene
{
public:
    MazeScene(GameManager* manager, int folderIndex);
    ~MazeScene() override;

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) override;
    void handleKeyPress(int key) override;
    void handleKeyRelease(int key) override;
    void onEnter() override;
    void onExit() override;
    void reset() override;
    SceneType getType() const override { return SceneType::Maze; }

private:
    void loadSprites();
    void generateMaze();
    void spawnSpecialElements();
    void spawnVirusMines();
    void checkVirusMineCollisions();
    bool checkWallCollision();
    void checkElementInteraction();
    void checkFinish();
    void resetPlayerToStart();
    void updateMovingElements(double deltaTime);
    void findMovableRange(int gridX, int gridY, int axis, double& minPos, double& maxPos);
    void regenerateElements();
    QPointF getRandomEmptyCell();

    GameManager* m_gameManager = nullptr;
    int m_folderIndex;

    QPixmap m_errorWallSprite;
    QPixmap m_virusSprite;
    QPixmap m_reloadSprite;
    QPixmap m_homeSprite;
    

    QVector<VirusMine> m_virusMines;
    

    QPixmap m_scaledWallSprite;
    int m_wallWidth;
    int m_wallHeight;
    

    static const int GRID_COLS = 11;
    static const int GRID_ROWS = 7;
    int m_grid[GRID_ROWS][GRID_COLS];

    QVector<MazeWall> m_walls;
    QVector<MazeElement> m_elements;

    QPointF m_startPoint;
    QPointF m_finishPoint;
    QRectF m_finishZone;
    

    bool m_mazeCompleted = false;
    double m_completionTimer = 0.0;
    

    double m_elementSpawnTimer = 0.0;
    double m_elementSpawnInterval = 3.0;
};

#endif
