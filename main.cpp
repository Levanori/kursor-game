#include <QApplication>
#include "gamewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameWidget gameWidget;
    gameWidget.setWindowTitle("test");
    gameWidget.show();

    return a.exec();
}
