#include <QApplication>
#include "gamewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameWidget gameWidget;
    gameWidget.setWindowTitle("test");
    gameWidget.resize(1000, 600);
    gameWidget.show();

    return a.exec();
}
