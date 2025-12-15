#ifndef BOSSSCENE_H
#define BOSSSCENE_H

#include "gamescene.h"
#include "miniboss.h"
#include <QVector>
#include <QPixmap>

class Player;
class GameManager;

class BossScene : public GameScene
{
public:
    BossScene(GameManager* manager, int folderIndex);
    ~BossScene();

    void update(double deltaTime) override;
    void render(QPainter& painter) override;
    void renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY) override;
    void handleKeyPress(int key) override;
    void handleKeyRelease(int key) override;
    void reset() override;
    SceneType getType() const override { return SceneType::Boss; }

    bool isCompleted() const override { return m_bossDefeated; }

private:
    void loadBackgroundLayers();
    void loadHandsAnimation();
    void loadHeadAnimation();
    
    void updateAnimations(double deltaTime);
    void updateBattle(double deltaTime);
    void renderBackground(QPainter& painter);
    void renderNekoAnimation(QPainter& painter);
    void checkPlayerDamage();

    GameManager* m_gameManager;
    int m_folderIndex;

    QPixmap m_staticLayer;
    QPixmap m_bottomLayer;

    QVector<QPixmap> m_handsFrames;
    int m_currentHandFrame;
    double m_handAnimTimer;
    double m_handAnimSpeed;
    bool m_handsAnimating;
    bool m_handsVisible;

    QVector<QPixmap> m_headFrames;
    int m_currentHeadFrame;
    double m_headAnimTimer;
    double m_headAnimSpeed;
    bool m_headVisible;

    double m_sceneTimer;
    enum class ScenePhase {
        NekoIntro,
        BossAppear,
        Battle,
        Victory,
        Defeated
    };
    ScenePhase m_scenePhase;

    MiniBoss m_miniBoss;
    bool m_bossDefeated;
    

    double m_victoryTimer;
};

#endif
