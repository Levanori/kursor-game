#include "mazescene.h"
#include "gamemanager.h"
#include "player.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>

MazeScene::MazeScene(GameManager* manager, int folderIndex)
    : GameScene()
    , m_gameManager(manager)
    , m_folderIndex(folderIndex)
    , m_wallWidth(0)
    , m_wallHeight(0)
{
    loadSprites();
}

MazeScene::~MazeScene()
{

}

void MazeScene::loadSprites()
{

    m_errorWallSprite = QPixmap(":/maze/error_wall.png");
    

    m_wallWidth = VIRTUAL_WIDTH / GRID_COLS;
    m_wallHeight = VIRTUAL_HEIGHT / GRID_ROWS;
    

    if (!m_errorWallSprite.isNull()) {
        m_scaledWallSprite = m_errorWallSprite.scaled(
            m_wallWidth, m_wallHeight, 
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    

    auto loadAndScale = [](const QString& path, int size) -> QPixmap {
        QPixmap original(path);
        if (original.isNull()) {
            qDebug() << "Failed to load:" << path;
            return original;
        }
        return original.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };
    
    const int elementSize = 30;
    m_virusSprite = loadAndScale(":/virus_green/virus_green_1.png", elementSize);
    m_reloadSprite = loadAndScale(":/maze/reload.png", elementSize);
    m_homeSprite = loadAndScale(":/maze/home.png", m_wallWidth);
    
    qDebug() << "Maze sprites loaded. Wall size:" << m_wallWidth << "x" << m_wallHeight;
}

void MazeScene::generateMaze()
{
    m_walls.clear();
    m_elements.clear();
    

    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
            m_grid[y][x] = 1;
        }
    }
    

    

    int startX = 1, startY = GRID_ROWS - 2;

    int endX = GRID_COLS - 2, endY = 1;
    

    m_grid[startY][startX] = 0;
    m_grid[endY][endX] = 0;
    

    QVector<QPair<int,int>> stack;
    stack.push_back({startX, startY});
    
    while (!stack.isEmpty()) {
        int cx = stack.last().first;
        int cy = stack.last().second;
        

        QVector<QPair<int,int>> neighbors;
        
        if (cx >= 2 && m_grid[cy][cx-2] == 1) neighbors.append({cx-2, cy});
        if (cx <= GRID_COLS-3 && m_grid[cy][cx+2] == 1) neighbors.append({cx+2, cy});
        if (cy >= 2 && m_grid[cy-2][cx] == 1) neighbors.append({cx, cy-2});
        if (cy <= GRID_ROWS-3 && m_grid[cy+2][cx] == 1) neighbors.append({cx, cy+2});
        
        if (!neighbors.isEmpty()) {

            int idx = QRandomGenerator::global()->bounded(neighbors.size());
            int nx = neighbors[idx].first;
            int ny = neighbors[idx].second;
            

            m_grid[(cy + ny) / 2][(cx + nx) / 2] = 0;
            m_grid[ny][nx] = 0;
            
            stack.push_back({nx, ny});
        } else {
            stack.pop_back();
        }
    }
    

    m_grid[endY][endX] = 0;
    m_grid[endY][endX-1] = 0;
    

    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
            if (m_grid[y][x] == 1) {
                MazeWall wall;
                wall.gridX = x;
                wall.gridY = y;
                wall.bounds = QRectF(x * m_wallWidth, y * m_wallHeight, m_wallWidth, m_wallHeight);
                m_walls.append(wall);
            }
        }
    }
    

    m_startPoint = QPointF(startX * m_wallWidth + m_wallWidth / 2 - 16,
                           startY * m_wallHeight + m_wallHeight / 2 - 16);
    

    m_finishPoint = QPointF(endX * m_wallWidth + m_wallWidth / 2,
                            endY * m_wallHeight + m_wallHeight / 2);
    m_finishZone = QRectF(endX * m_wallWidth, endY * m_wallHeight, m_wallWidth, m_wallHeight);
    
    qDebug() << "Maze generated with" << m_walls.size() << "walls";
}

void MazeScene::findMovableRange(int gridX, int gridY, int axis, double& minPos, double& maxPos)
{

    
    if (axis == 0) {

        int leftX = gridX;
        while (leftX > 0 && m_grid[gridY][leftX - 1] == 0) {
            leftX--;
        }

        int rightX = gridX;
        while (rightX < GRID_COLS - 1 && m_grid[gridY][rightX + 1] == 0) {
            rightX++;
        }
        minPos = leftX * m_wallWidth + 15;
        maxPos = rightX * m_wallWidth + m_wallWidth - 45;
    } else {

        int topY = gridY;
        while (topY > 0 && m_grid[topY - 1][gridX] == 0) {
            topY--;
        }

        int bottomY = gridY;
        while (bottomY < GRID_ROWS - 1 && m_grid[bottomY + 1][gridX] == 0) {
            bottomY++;
        }
        minPos = topY * m_wallHeight + 15;
        maxPos = bottomY * m_wallHeight + m_wallHeight - 45;
    }
}

QPointF MazeScene::getRandomEmptyCell()
{

    QVector<QPair<int,int>> emptyCells;
    
    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
            if (m_grid[y][x] == 0) {
                bool isStart = (x == 1 && y == GRID_ROWS - 2);
                bool isFinish = (x == GRID_COLS - 2 && y == 1);
                if (!isStart && !isFinish) {
                    emptyCells.append({x, y});
                }
            }
        }
    }
    
    if (emptyCells.isEmpty()) return m_startPoint;
    
    int idx = QRandomGenerator::global()->bounded(emptyCells.size());
    int cx = emptyCells[idx].first;
    int cy = emptyCells[idx].second;
    
    double centerX = cx * m_wallWidth + m_wallWidth / 2.0;
    double centerY = cy * m_wallHeight + m_wallHeight / 2.0;
    
    return QPointF(centerX, centerY);
}

void MazeScene::spawnVirusMines()
{
    m_virusMines.clear();
    

    int mineCount = QRandomGenerator::global()->bounded(4, 8);
    
    for (int i = 0; i < mineCount; ++i) {
        QPointF pos = getRandomEmptyCell();
        
        VirusMine mine;
        mine.bounds = QRectF(pos.x() - 15, pos.y() - 15, 30, 30);
        mine.sprite = m_virusSprite;
        mine.active = true;
        
        m_virusMines.append(mine);
    }
    
    qDebug() << "Spawned" << mineCount << "virus mines in maze";
}

void MazeScene::checkVirusMineCollisions()
{
    if (!player) return;
    
    QRectF playerBounds = player->getBounds();
    
    for (int i = m_virusMines.size() - 1; i >= 0; --i) {
        VirusMine& mine = m_virusMines[i];
        
        if (!mine.active) continue;
        
        if (playerBounds.intersects(mine.bounds)) {

            int currentLoad = player->getCpuLoad();
            player->setCpuLoad(currentLoad + 8);
            mine.active = false;
            

            m_virusMines.removeAt(i);
            
            qDebug() << "Stepped on virus mine! CPU Load:" << player->getCpuLoad();
        }
    }
}

void MazeScene::spawnSpecialElements()
{

    QPointF pos = getRandomEmptyCell();
    
    MazeElement element;
    element.type = MazeElementType::Reload;
    element.sprite = m_reloadSprite;
    element.animTimer = 0.0;
    element.moveAxis = QRandomGenerator::global()->bounded(2);
    element.moveDirection = 1;
    element.moveSpeed = 40.0;
    
    double centerX = pos.x() - 15;
    double centerY = pos.y() - 15;
    element.bounds = QRectF(centerX, centerY, 30, 30);
    

    int gridX = (int)(pos.x() / m_wallWidth);
    int gridY = (int)(pos.y() / m_wallHeight);
    findMovableRange(gridX, gridY, element.moveAxis, element.minPos, element.maxPos);
    
    m_elements.append(element);
}

void MazeScene::regenerateElements()
{

    m_elements.clear();
    m_virusMines.clear();
    m_elementSpawnTimer = 0.0;
    

    int reloadCount = QRandomGenerator::global()->bounded(1, 3);
    for (int i = 0; i < reloadCount; ++i) {
        spawnSpecialElements();
    }
    

    spawnVirusMines();
}

void MazeScene::updateMovingElements(double deltaTime)
{
    for (MazeElement& elem : m_elements) {

        

        if (elem.moveAxis == 0) {
            double newX = elem.bounds.x() + elem.moveSpeed * elem.moveDirection * deltaTime;
            

            if (newX <= elem.minPos) {
                newX = elem.minPos;
                elem.moveDirection = 1;
            } else if (newX >= elem.maxPos) {
                newX = elem.maxPos;
                elem.moveDirection = -1;
            }
            
            elem.bounds.moveLeft(newX);
        } else {
            double newY = elem.bounds.y() + elem.moveSpeed * elem.moveDirection * deltaTime;
            
            if (newY <= elem.minPos) {
                newY = elem.minPos;
                elem.moveDirection = 1;
            } else if (newY >= elem.maxPos) {
                newY = elem.maxPos;
                elem.moveDirection = -1;
            }
            
            elem.bounds.moveTop(newY);
        }
    }
}

void MazeScene::update(double deltaTime)
{
    if (gameOver || m_mazeCompleted) {
        if (m_mazeCompleted) {
            m_completionTimer += deltaTime;
            if (m_completionTimer >= 1.5) {
                sceneCompleted = true;
                result.completed = true;
                result.nextScene = SceneType::Desktop;
                result.nextSceneIndex = 0;
            }
        }
        return;
    }
    

    updateMovingElements(deltaTime);
    
    if (player) {
        player->update(deltaTime);
        

        QPointF pos = player->getPosition();
        QSizeF psize = player->getSize();
        pos.setX(qBound(0.0, pos.x(), (double)VIRTUAL_WIDTH - psize.width()));
        pos.setY(qBound(0.0, pos.y(), (double)VIRTUAL_HEIGHT - psize.height()));
        player->setPosition(pos);
    }
    

    if (checkWallCollision()) {
        gameOver = true;
        qDebug() << "Hit wall! Game Over!";
        return;
    }
    

    checkVirusMineCollisions();
    

    checkElementInteraction();
    

    checkFinish();
}

bool MazeScene::checkWallCollision()
{
    if (!player) return false;
    
    QRectF playerBounds = player->getBounds();
    
    for (const MazeWall& wall : m_walls) {
        if (playerBounds.intersects(wall.bounds)) {
            return true;
        }
    }
    
    return false;
}

void MazeScene::checkElementInteraction()
{
    if (!player) return;
    
    QRectF playerBounds = player->getBounds();
    
    for (int i = m_elements.size() - 1; i >= 0; --i) {
        MazeElement& element = m_elements[i];
        
        if (playerBounds.intersects(element.bounds)) {
            if (element.type == MazeElementType::Reload) {

                resetPlayerToStart();
                regenerateElements();
                qDebug() << "Reload! Back to start, elements regenerated!";
                return;
            }
        }
    }
}

void MazeScene::resetPlayerToStart()
{
    if (player) {
        player->setPosition(m_startPoint);
        player->clearKeys();
    }
}

void MazeScene::checkFinish()
{
    if (!player || m_mazeCompleted) return;
    
    QRectF playerBounds = player->getBounds();
    
    if (playerBounds.intersects(m_finishZone)) {
        m_mazeCompleted = true;
        m_completionTimer = 0.0;
        score += 200;
        

        if (player) {
            player->setCpuLoad(0);
        }
        
        qDebug() << "Maze completed!";
    }
}

void MazeScene::render(QPainter& painter)
{

    painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(20, 20, 30));
    

    for (const MazeWall& wall : m_walls) {
        if (!m_scaledWallSprite.isNull()) {
            painter.drawPixmap(
                static_cast<int>(wall.bounds.x()),
                static_cast<int>(wall.bounds.y()),
                m_scaledWallSprite);
        } else {

            painter.fillRect(wall.bounds, QColor(150, 30, 30));
            painter.setPen(QPen(Qt::red, 1));
            painter.drawRect(wall.bounds);
        }
    }
    

    for (const MazeElement& element : m_elements) {
        if (!element.sprite.isNull()) {
            painter.drawPixmap(
                static_cast<int>(element.bounds.x()),
                static_cast<int>(element.bounds.y()),
                element.sprite);
        }
    }
    

    for (const VirusMine& mine : m_virusMines) {
        if (mine.active && !mine.sprite.isNull()) {
            painter.drawPixmap(
                static_cast<int>(mine.bounds.x()),
                static_cast<int>(mine.bounds.y()),
                mine.sprite);
        }
    }
    

    if (!m_homeSprite.isNull()) {
        painter.drawPixmap(
            static_cast<int>(m_finishZone.x()),
            static_cast<int>(m_finishZone.y()),
            m_homeSprite);
    } else {

        painter.setBrush(QColor(50, 200, 50, 150));
        painter.setPen(QPen(Qt::green, 2));
        painter.drawRect(m_finishZone);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        painter.drawText(m_finishZone, Qt::AlignCenter, "HOME");
    }
    

    if (player) {
        player->render(painter);
    }
    

    if (m_mazeCompleted) {
        painter.fillRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, QColor(0, 100, 0, 150));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(QRect(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT), 
                        Qt::AlignCenter, "MAZE COMPLETED!");
    }
}

void MazeScene::renderUI(QPainter& painter, double scaleFactor, double offsetX, double offsetY)
{
    QString hint = "MAZE | Reach FINISH! Don't touch walls!";
    renderBaseUI(painter, scaleFactor, offsetX, offsetY, hint);
}

void MazeScene::handleKeyPress(int key)
{
    if (gameOver || m_mazeCompleted) return;
    
    if (player) {
        player->setKeyPressed(key, true);
    }
    
    if (key == Qt::Key_Escape && m_gameManager) {
        sceneCompleted = true;
        result.completed = false;
        result.returnToPrevious = true;
    }
}

void MazeScene::handleKeyRelease(int key)
{
    if (player) {
        player->setKeyPressed(key, false);
    }
}

void MazeScene::onEnter()
{
    sceneCompleted = false;
    gameOver = false;
    m_mazeCompleted = false;
    m_completionTimer = 0.0;
    m_elementSpawnTimer = 0.0;
    

    generateMaze();
    

    regenerateElements();
    

    if (player) {
        player->setPosition(m_startPoint);
        player->clearKeys();
    }
}

void MazeScene::onExit()
{
    if (player) {
        player->clearKeys();
    }
}

void MazeScene::reset()
{
    m_walls.clear();
    m_elements.clear();
    m_virusMines.clear();
    m_mazeCompleted = false;
    m_completionTimer = 0.0;
    m_elementSpawnTimer = 0.0;
    gameOver = false;
    sceneCompleted = false;
    score = 0;
    
    if (player) {
        player->reset();
        player->clearKeys();
    }
}
