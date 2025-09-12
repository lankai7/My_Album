#include "ShowImageInfo.h"
#include <QFileInfo>
#include <QDateTime>

ShowImageInfo::ShowImageInfo()
{

}

// 字节转化为 KB/MB/GB 格式
QString ShowImageInfo:: humanReadableSize(qint64 bytes)
{
    double size = bytes;
    QString unit = "字节";

    if (size >= 1024) {
        size /= 1024.0;
        unit = "KB";
    }
    if (size >= 1024) {
        size /= 1024.0;
        unit = "MB";
    }
    if (size >= 1024) {
        size /= 1024.0;
        unit = "GB";
    }

    return QString("%1 %2").arg(QString::number(size, 'f', 2)).arg(unit);
}


QString ShowImageInfo::getImageInfo(const QImage &image,const QString filePath)
{
    QString formatStr;
    switch (image.format()) {
    case QImage::Format_RGB32:      formatStr = "RGB32"; break;
    case QImage::Format_ARGB32:     formatStr = "ARGB32"; break;
    case QImage::Format_RGB888:     formatStr = "RGB888"; break;
    case QImage::Format_Grayscale8: formatStr = "灰度8位"; break;
    default:                        formatStr = QString("未知(%1)").arg(int(image.format()));
    }

    // 获取文件大小
    QFileInfo fi(filePath);
    QString fileSizeStr = humanReadableSize(fi.size());

    QString info = QString(
                "尺寸：%1 × %2\n"
                "格式：%3\n"
                "位深：%4 位\n"
                "设备像素比：%5\n"
                "每行字节数：%6 字节\n"
                "内存占用：%7 字节\n"
                "文件大小：%8"
                )
            .arg(image.width())
            .arg(image.height())
            .arg(formatStr)
            .arg(image.depth())
            .arg(image.devicePixelRatio())
            .arg(image.bytesPerLine())
            .arg(image.sizeInBytes())   // 内存占用还是字节
            .arg(fileSizeStr);          // 新增文件实际大小

    return info;
}

QString ShowImageInfo::getImageSize(const QString filePath)
{
    QFileInfo fi(filePath);
    QString fileSizeStr = humanReadableSize(fi.size());
    return fileSizeStr;
}

QDateTime ShowImageInfo::getImageCreatedTime(const QString filePath)
{
    QFileInfo fi(filePath);
    QDateTime createdTime = fi.birthTime();       // 创建时间
    return createdTime;
}

QDateTime ShowImageInfo::getImageModifiedTime(const QString filePath)
{
    QFileInfo fi(filePath);
    QDateTime modifiedTime = fi.lastModified(); // 最后修改时间
    return modifiedTime;
}

