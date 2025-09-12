#ifndef SHOWIMAGEINFO_H
#define SHOWIMAGEINFO_H
#include <QString>
#include <QImage>

class ShowImageInfo
{
public:
    ShowImageInfo();
    static QString getImageInfo(const QImage &image,const QString filePath);
    static QString getImageSize(const QString filePath);
    static QDateTime getImageCreatedTime(const QString filePath);
    static QDateTime getImageModifiedTime(const QString filePath);
private:
    static QString humanReadableSize(qint64 bytes);
};

#endif // SHOWIMAGEINFO_H
