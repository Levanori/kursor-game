#include "miniboss.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>

MiniBoss::MiniBoss()
    : m_position(362, 225)
    , m_windowOffset(0)
    , m_targetWindowOffset(0)
    , m_phase(Phase::Hidden)
    , m_health(100)
    , m_maxHealth(100)
    , m_phaseTimer(0)
    , m_attackTimer(0)
    , m_attackCooldown(1.5)
    , m_hitTimer(0)
    , m_invulnerableTimer(0)
    , m_hammerSwinging(false)
    , m_hammerAngle(0)
{
    loadSprites();
    

    for (int i = 0; i < MAX_WAVES; ++i) {
        m_waves[i].active = false;
    }
}

MiniBoss::~MiniBoss()
{
}

void MiniBoss::loadSprites()
{

    const double scale = 0.035;
    
    auto scalePixmap = [scale](const QPixmap& src) -> QPixmap {
        if (src.isNull()) return src;
        int newW = static_cast<int>(src.width() * scale);
        int newH = static_cast<int>(src.height() * scale);
        return src.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };
    
    m_windowUpper = scalePixmap(QPixmap(":/miniboss/window_upper.png"));
    m_windowLower = scalePixmap(QPixmap(":/miniboss/window_lower.png"));
    m_cheekbone1 = scalePixmap(QPixmap(":/miniboss/cheekbone_1.png"));
    m_cheekbone2 = scalePixmap(QPixmap(":/miniboss/cheekbone_2.png"));
    m_bossFace = scalePixmap(QPixmap(":/miniboss/boss_face.png"));
    m_bossFaceHit = scalePixmap(QPixmap(":/miniboss/boss_face_hit.png"));
    m_bossFaceDead = scalePixmap(QPixmap(":/miniboss/boss_face_dead.png"));
    m_hammer = scalePixmap(QPixmap(":/miniboss/hammer.png"));
    m_hammerAttack = scalePixmap(QPixmap(":/miniboss/hammer_attack.png"));
    

    const double waveScale = 0.03;
    auto scaleWave = [waveScale](const QPixmap& src) -> QPixmap {
        if (src.isNull()) return src;
        int newW = static_cast<int>(src.width() * waveScale);
        int newH = static_cast<int>(src.height() * waveScale);
        return src.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };
    m_attackWave1 = scaleWave(QPixmap(":/miniboss/attackWave_1.png"));
    m_attackWave2 = scaleWave(QPixmap(":/miniboss/attackWave_2.png"));
    
    if (m_windowUpper.isNull()) qDebug() << "Failed to load window_upper.png";
    if (m_windowLower.isNull()) qDebug() << "Failed to load window_lower.png";
    if (m_bossFace.isNull()) qDebug() << "Failed to load boss_face.png";
    
    qDebug() << "MiniBoss sprites loaded. Window size:" << m_windowUpper.width() << "x" << m_windowUpper.height();
}

void MiniBoss::startAppear()
{
    if (m_phase == Phase::Hidden) {
        m_phase = Phase::WindowAppear;
        m_phaseTimer = 0;
        qDebug() << "MiniBoss starting to appear";
    }
}

void MiniBoss::update(double deltaTime)
{
    m_phaseTimer += deltaTime;
    

    if (m_invulnerableTimer > 0) {
        m_invulnerableTimer -= deltaTime;
    }
    
    switch (m_phase) {
        case Phase::Hidden:

            break;
            
        case Phase::WindowAppear:

            if (m_phaseTimer >= 0.5) {
                m_phase = Phase::WindowOpen;
                m_phaseTimer = 0;
                m_targetWindowOffset = 15;
            }
            break;
            
        case Phase::WindowOpen:

            m_windowOffset += (m_targetWindowOffset - m_windowOffset) * deltaTime * 5;
            if (m_phaseTimer >= 0.5) {
                m_phase = Phase::CheekbonesAppear;
                m_phaseTimer = 0;
            }
            break;
            
        case Phase::CheekbonesAppear:

            if (m_phaseTimer >= 0.3) {
                m_phase = Phase::FaceAppear;
                m_phaseTimer = 0;
            }
            break;
            
        case Phase::FaceAppear:

            if (m_phaseTimer >= 0.3) {
                m_phase = Phase::Idle;
                m_phaseTimer = 0;
                m_attackTimer = 0;
            }
            break;
            
        case Phase::Idle:

            m_attackTimer += deltaTime;
            if (m_attackTimer >= m_attackCooldown) {
                m_phase = Phase::Attack;
                m_phaseTimer = 0;
                m_hammerSwinging = true;
                m_hammerAngle = 0;
            }
            break;
            
        case Phase::Attack:

            updateAnimation(deltaTime);
            if (m_phaseTimer >= 0.5) {

                spawnAttackWave();
                m_phase = Phase::Idle;
                m_phaseTimer = 0;
                m_attackTimer = 0;
                m_hammerSwinging = false;
            }
            break;
            
        case Phase::Hit:

            m_hitTimer -= deltaTime;
            if (m_hitTimer <= 0) {
                if (m_health <= 0) {
                    m_phase = Phase::Dead;
                    m_phaseTimer = 0;
                } else {
                    m_phase = Phase::Idle;
                    m_attackTimer = 0;
                }
            }
            break;
            
        case Phase::Dead:

            break;
    }
    

    for (int i = 0; i < MAX_WAVES; ++i) {
        if (m_waves[i].active) {
            m_waves[i].position += m_waves[i].velocity * deltaTime;
            

            if (m_waves[i].position.x() < -50 || m_waves[i].position.x() > 775 ||
                m_waves[i].position.y() < -50 || m_waves[i].position.y() > 500) {
                m_waves[i].active = false;
            }
        }
    }
}

void MiniBoss::updateAnimation(double deltaTime)
{
    if (m_hammerSwinging) {
        m_hammerAngle += deltaTime * 720;
        if (m_hammerAngle > 45) {
            m_hammerAngle = 45;
        }
    }
}

void MiniBoss::spawnAttackWave()
{

    int waveCount = QRandomGenerator::global()->bounded(2, 5);
    

    int centerX = static_cast<int>(m_position.x());
    int centerY = static_cast<int>(m_position.y());
    int windowWidth = m_windowUpper.width();
    int hammerX = centerX - windowWidth / 2 - m_hammerAttack.width() / 2 - 10;
    int hammerY = centerY + m_hammerAttack.height() / 2;
    QPointF hammerBottom(hammerX, hammerY);
    
    for (int w = 0; w < waveCount; ++w) {

        for (int i = 0; i < MAX_WAVES; ++i) {
            if (!m_waves[i].active) {
                m_waves[i].active = true;
                m_waves[i].position = hammerBottom;
                m_waves[i].type = QRandomGenerator::global()->bounded(1, 3);
                

                QPointF direction = m_playerPosition - hammerBottom;
                double baseAngle = atan2(direction.y(), direction.x());
                

                double spread = (QRandomGenerator::global()->bounded(30) - 15) * M_PI / 180.0;
                double angle = baseAngle + spread;
                
                double speed = 150 + QRandomGenerator::global()->bounded(100);
                m_waves[i].velocity = QPointF(cos(angle) * speed, sin(angle) * speed);
                
                break;
            }
        }
    }
}

void MiniBoss::render(QPainter& painter)
{
    if (m_phase == Phase::Hidden) return;
    
    int centerX = static_cast<int>(m_position.x());
    int centerY = static_cast<int>(m_position.y());
    

    int windowWidth = m_windowUpper.width();
    int windowX = centerX - windowWidth / 2;
    
    int upperY = centerY - m_windowUpper.height() - static_cast<int>(m_windowOffset);
    int lowerY = centerY + static_cast<int>(m_windowOffset);
    

    if (!m_windowLower.isNull()) {
        painter.drawPixmap(windowX, lowerY, m_windowLower);
    }
    

    if (m_phase >= Phase::CheekbonesAppear && m_phase != Phase::Dead) {
        int cheekY = centerY - m_cheekbone1.height() / 2;
        if (!m_cheekbone1.isNull()) {
            painter.drawPixmap(centerX - windowWidth / 3 - m_cheekbone1.width() / 2, cheekY, m_cheekbone1);
        }
        if (!m_cheekbone2.isNull()) {
            painter.drawPixmap(centerX + windowWidth / 3 - m_cheekbone2.width() / 2, cheekY, m_cheekbone2);
        }
    }
    

    if (m_phase >= Phase::FaceAppear) {
        QPixmap* faceSprite = &m_bossFace;
        
        if (m_phase == Phase::Hit) {
            faceSprite = &m_bossFaceHit;
        } else if (m_phase == Phase::Dead) {
            faceSprite = &m_bossFaceDead;
        }
        
        if (!faceSprite->isNull()) {
            painter.drawPixmap(centerX - faceSprite->width() / 2, 
                              centerY - faceSprite->height() / 2, 
                              *faceSprite);
        }
    }
    

    if (m_phase >= Phase::Idle && m_phase != Phase::Dead) {
        QPixmap* hammerSprite = m_hammerSwinging ? &m_hammerAttack : &m_hammer;
        if (!hammerSprite->isNull()) {
            painter.save();

            int hammerX = centerX - windowWidth / 2 - hammerSprite->width() - 10;
            int hammerY = centerY;
            
            if (m_hammerSwinging) {

                painter.translate(hammerX + hammerSprite->width(), hammerY);
                painter.rotate(-m_hammerAngle);
                painter.drawPixmap(-hammerSprite->width() / 2, -hammerSprite->height() / 2, *hammerSprite);
            } else {
                painter.drawPixmap(hammerX, 
                                  hammerY - hammerSprite->height() / 2, 
                                  *hammerSprite);
            }
            painter.restore();
        }
    }
    

    if (!m_windowUpper.isNull()) {
        painter.drawPixmap(windowX, upperY, m_windowUpper);
    }
    

    for (int i = 0; i < MAX_WAVES; ++i) {
        if (m_waves[i].active) {
            QPixmap* waveSprite = (m_waves[i].type == 1) ? &m_attackWave1 : &m_attackWave2;
            if (!waveSprite->isNull()) {
                painter.drawPixmap(
                    static_cast<int>(m_waves[i].position.x()) - waveSprite->width() / 2,
                    static_cast<int>(m_waves[i].position.y()) - waveSprite->height() / 2,
                    *waveSprite
                );
            }
        }
    }
    

    if (m_phase >= Phase::FaceAppear && m_phase != Phase::Dead) {
        int barWidth = 100;
        int barHeight = 8;
        int barX = centerX - barWidth / 2;
        int barY = upperY - 20;
        

        painter.fillRect(barX, barY, barWidth, barHeight, QColor(50, 50, 50));
        

        int healthWidth = static_cast<int>(barWidth * m_health / m_maxHealth);
        painter.fillRect(barX, barY, healthWidth, barHeight, QColor(200, 50, 50));
        

        painter.setPen(QPen(Qt::white, 1));
        painter.drawRect(barX, barY, barWidth, barHeight);
    }
}

bool MiniBoss::takeDamage(int damage)
{

    if (m_phase == Phase::Dead || m_phase == Phase::Hit || m_phase == Phase::Attack) return false;
    

    if (m_invulnerableTimer > 0) return false;
    
    m_health -= damage;
    m_phase = Phase::Hit;
    m_hitTimer = 0.3;
    m_invulnerableTimer = 2.0;
    
    qDebug() << "MiniBoss took" << damage << "damage, health:" << m_health;
    
    return m_health <= 0;
}

QRectF MiniBoss::getHitBox() const
{
    if (m_phase < Phase::FaceAppear || m_phase == Phase::Dead) {
        return QRectF();
    }
    

    int faceW = m_bossFace.width();
    int faceH = m_bossFace.height();
    return QRectF(m_position.x() - faceW / 2, m_position.y() - faceH / 2, faceW, faceH);
}

bool MiniBoss::checkAttackWaveHit(const QPointF& point) const
{
    for (int i = 0; i < MAX_WAVES; ++i) {
        if (m_waves[i].active) {

            int waveSize = (m_waves[i].type == 1) ? m_attackWave1.width() : m_attackWave2.width();
            if (waveSize == 0) waveSize = 30;
            QRectF waveRect(
                m_waves[i].position.x() - waveSize / 2,
                m_waves[i].position.y() - waveSize / 2,
                waveSize, waveSize
            );
            if (waveRect.contains(point)) {
                return true;
            }
        }
    }
    return false;
}

void MiniBoss::reset()
{
    m_phase = Phase::Hidden;
    m_health = m_maxHealth;
    m_windowOffset = 0;
    m_targetWindowOffset = 0;
    m_phaseTimer = 0;
    m_attackTimer = 0;
    m_hitTimer = 0;
    m_invulnerableTimer = 0;
    m_hammerSwinging = false;
    m_hammerAngle = 0;
    
    for (int i = 0; i < MAX_WAVES; ++i) {
        m_waves[i].active = false;
    }
}
