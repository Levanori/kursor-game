#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include <QPixmap>
#include <QVector>
#include <QSet>

class Player : public GameObject
{
public:
    Player(QPointF pos, QSizeF s);

    void update(double deltaTime) override;
    void render(QPainter& painter) override;

    // Керування
    void setKeyPressed(int key, bool pressed);

    // Нагрузка
    void takeDamage(int amount);
    int getCpuLoad() { return cpuLoad; }
    int getMaxCpuLoad() { return maxCpuLoad; }
    bool isCrashed() { return cpuLoad >= maxCpuLoad; }
    void reset();
    void increaseLoad(int amount);
    void decreaseLoad(int amount);

    bool isInvincible() { return invincibilityTimer > 0; }

    // Атака
    void attack();
    bool isAttacking() const { return attacking; }
    QRectF getAttackBounds() const;

    // Колізії
    QRectF getBounds() const;

    // Доступ до позиції для ворогів
    QPointF* getPositionPtr() { return &position; }

    void setPosition(const QPointF& newPos) { position = newPos; }

    // очищення ключів, щоб після рестарту не було багу з рухом
    void clearKeys();
private:
    void updateSprite(); // Оновлення спрайту залежно від HP

    QPixmap sprite;
    QVector<QPixmap> cursorSprites; // 5 станів: 100, 80, 60, 40, 20 HP
    QSet<int> pressedKeys;

    // нагрузка
    int cpuLoad = 0;
    int maxCpuLoad = 100;
    double invincibilityTimer = 0;
    double invincibilityDuration = 1.0;

    // Атака
    bool attacking = false;
    double attackTimer = 0;
    double attackDuration = 0.2;
    double attackCooldown = 0;
    double attackCooldownTime = 0.5;
};

#endif // PLAYER_H

