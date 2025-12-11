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

    // Здоров'я
    void takeDamage(int amount);
    void heal(int amount);
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0; }
    bool isInvincible() const { return invincibilityTimer > 0; }

    // Атака
    void attack();
    bool isAttacking() const { return attacking; }
    QRectF getAttackBounds() const;

    // Колізії
    QRectF getBounds() const;

    // Доступ до позиції для ворогів
    QPointF* getPositionPtr() { return &position; }

    void setPosition(const QPointF& newPos) { position = newPos; }

    void resetHealth();
private:
    void updateSprite(); // Оновлення спрайту залежно від HP

    QPixmap sprite;
    QVector<QPixmap> cursorSprites; // 5 станів: 100, 80, 60, 40, 20 HP
    QSet<int> pressedKeys;

    // Здоров'я
    int health = 100;
    int maxHealth = 100;
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
