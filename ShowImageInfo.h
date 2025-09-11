#ifndef SHOWIMAGEINFO_H
#define SHOWIMAGEINFO_H
#include <QString>
#include <QImage>

class ShowImageInfo
{
public:
    ShowImageInfo();
    static QString getImageInfo(const QImage &image);
};

#endif // SHOWIMAGEINFO_H
