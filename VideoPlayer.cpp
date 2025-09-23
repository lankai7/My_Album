#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    player = new QMediaPlayer(this);

    // 视频显示对象
    videoItem = new QGraphicsVideoItem;
    scene = new QGraphicsScene(this);
    scene->addItem(videoItem);

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background-color: black;"); // 背景黑色

    // 播放/暂停按钮
    playButton = new QPushButton("播放");
    pauseButton = new QPushButton("暂停");

    connect(playButton, &QPushButton::clicked, this, &VideoPlayer::play);
    connect(pauseButton, &QPushButton::clicked, this, &VideoPlayer::pause);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);
    setLayout(layout);

    // 绑定视频输出
    player->setVideoOutput(videoItem);
}

void VideoPlayer::openVideo(const QString &filePath)
{
    player->setMedia(QUrl::fromLocalFile(filePath));
}

void VideoPlayer::play()
{
    player->play();
}

void VideoPlayer::pause()
{
    player->pause();
}
