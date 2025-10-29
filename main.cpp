#include "AlbumWindow.h"
#include <QCommandLineParser>
#include <QApplication>

/**
 * @brief   : 程序入口，支持文件关联打开图片
 * @date    : 2025.10.29
 **/

#include "AlbumWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDir>

int main(int argc, char *argv[])
{
    // 启用高 DPI 支持
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Windows 多媒体兼容
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "windowsmediafoundation");

    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("MyAlbum 图片查看器");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "要打开的图片文件路径");
    parser.process(app);

    AlbumWindow w;
    w.setWindowIcon(QIcon(":/new/res/icon.png"));

    const QStringList args = parser.positionalArguments();

    if (!args.isEmpty()) {
        QString imagePath = QDir::toNativeSeparators(args.first());
        if (QFile::exists(imagePath)) {
            w.setGeometry(QRect(100,100,1200,700));
            w.showNormal();
            w.openImage(imagePath);
        } else {
            w.showMaximized();
        }
    } else {
        w.showMaximized();
    }

    return app.exec();
}

