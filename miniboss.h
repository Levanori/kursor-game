#ifndef MINIBOSS_H
#define MINIBOSS_H

#include <QPixmap>
#include <QPointF>
#include <QRectF>

class QPainter;

class MiniBoss
{
public:
    MiniBoss();
    ~MiniBoss();

    void update(double deltaTime);
    void render(QPainter& painter);
    

    void setPlayerPosition(const QPointF& pos) { m_playerPosition = pos; }

    enum class Phase {
        Hidden,
        WindowAppear,
        WindowOpen,
        CheekbonesAppear,
        FaceAppear,
        Idle,
        Attack,
        Hit,
        Dead
    };

    Phase getPhase() const { return m_phase; }
    void startAppear();
    
    bool takeDamage(int damage);
    bool isAlive() const { return m_health > 0; }
    bool isDead() const { return m_phase == Phase::Dead; }
    
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    
    QRectF getHitBox() const;
    

    bool checkAttackWaveHit(const QPointF& point) const;
    
    void reset();

private:
    void loadSprites();
    void updateAnimation(double deltaTime);
    void spawnAttackWave();
    

    QPixmap m_windowUpper;
    QPixmap m_windowLower;
    QPixmap m_cheekbone1;
    QPixmap m_cheekbone2;
    QPixmap m_bossFace;
    QPixmap m_bossFaceHit;
    QPixmap m_bossFaceDead;
    QPixmap m_hammer;
    QPixmap m_hammerAttack;
    QPixmap m_attackWave1;
    QPixmap m_attackWave2;
    

    QPointF m_position;
    

    QPointF m_playerPosition;
    

    double m_windowOffset;
    double m_targetWindowOffset;
    

    Phase m_phase;
    int m_health;
    int m_maxHealth;
    

    double m_phaseTimer;
    double m_attackTimer;
    double m_attackCooldown;
    double m_hitTimer;
    double m_invulnerableTimer;
    

    struct AttackWave {
        QPointF position;
        QPointF velocity;
        int type;
        bool active;
    };
    static const int MAX_WAVES = 8;
    AttackWave m_waves[MAX_WAVES];
    

    bool m_hammerSwinging;
    double m_hammerAngle;
};

#endif
