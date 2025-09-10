#include "ImageView.h"
#include <QDebug>

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);      // 平滑渲染
    setDragMode(QGraphicsView::ScrollHandDrag); // 可以拖动
    setMouseTracking(true); // 必须开启鼠标追踪，否则没有鼠标移动事件
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    const double scaleFactor = 1.15; // 缩放系数
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);       // 放大
    } else {
        scale(1.0/scaleFactor, 1.0/scaleFactor); // 缩小
    }
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    emit mouseMoved(pos); // 发信号给 AlbumWindow 更新按钮
    QGraphicsView::mouseMoveEvent(event);
}
