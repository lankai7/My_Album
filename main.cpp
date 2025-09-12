#include "AlbumWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 启用高 DPI 缩放（Qt 5.14+）
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    AlbumWindow w;
    w.setWindowTitle("Album");
    w.setWindowIcon(QIcon(":/new/res/icon.png"));
    w.showMaximized();
    return a.exec();
}
