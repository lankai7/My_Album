#include "ShowImageInfo.h"


ShowImageInfo::ShowImageInfo()
{

}

QString ShowImageInfo::getImageInfo(const QImage &image)
{
    // 转换格式为中文描述
    QString formatStr;
    switch (image.format()) {
    case QImage::Format_RGB32:      formatStr = "RGB32"; break;
    case QImage::Format_ARGB32:     formatStr = "ARGB32"; break;
    case QImage::Format_RGB888:     formatStr = "RGB888"; break;
    case QImage::Format_Grayscale8: formatStr = "灰度8位"; break;
    default:                        formatStr = QString("未知(%1)").arg(int(image.format()));
    }
    QString info = QString(
            "图片信息：\n"
            "尺寸：%1 × %2\n"
            "格式：%3\n"
            "位深：%4 位\n"
            "设备像素比：%5\n"
            "每行字节数：%6 字节\n"
            "总内存占用：%7 字节"
        )
        .arg(image.width())
        .arg(image.height())
        .arg(formatStr)
        .arg(image.depth())
        .arg(image.devicePixelRatio())
        .arg(image.bytesPerLine())
        .arg(image.sizeInBytes());

    return info;
}

