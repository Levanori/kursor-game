QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bossscene.cpp \
    desktopscene.cpp \
    enemy.cpp \
    fileicon.cpp \
    finalbossscene.cpp \
    foldericon.cpp \
    folderscene.cpp \
    game.cpp \
    gamemanager.cpp \
    gameobject.cpp \
    gamescene.cpp \
    gamewidget.cpp \
    level.cpp \
    main.cpp \
    mazescene.cpp \
    miniboss.cpp \
    player.cpp \
    projectile.cpp \
    testobject.cpp \
    virusgreen.cpp \
    virusviolet.cpp

HEADERS += \
    bossscene.h \
    desktopscene.h \
    enemy.h \
    fileicon.h \
    finalbossscene.h \
    foldericon.h \
    folderscene.h \
    game.h \
    gamemanager.h \
    gameobject.h \
    gamescene.h \
    gamewidget.h \
    level.h \
    mazescene.h \
    miniboss.h \
    player.h \
    projectile.h \
    testobject.h \
    virusgreen.h \
    virusviolet.h

RESOURCES += \
    resources.qrc

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
