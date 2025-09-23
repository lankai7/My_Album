#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QPushButton>

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);

    void openVideo(const QString &filePath); // 打开视频

private slots:
    void play();
    void pause();

private:
    QMediaPlayer *player;
    QGraphicsVideoItem *videoItem;
    QGraphicsScene *scene;
    QGraphicsView *view;

    QPushButton *playButton;
    QPushButton *pauseButton;
};

#endif // VIDEOPLAYER_H
