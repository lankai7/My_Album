#include "AlbumWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AlbumWindow w;
    w.setWindowTitle("Album");
    w.showMaximized();
    return a.exec();
}
