#include "ImageView.h"
#include <QDebug>
#include <QMimeData>
#include <QFileInfo>


ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);      // 平滑渲染
    setDragMode(QGraphicsView::ScrollHandDrag); // 可以拖动
    setMouseTracking(true); // 必须开启鼠标追踪，否则没有鼠标移动事件
    setAcceptDrops(false);
    viewport()->setAcceptDrops(false);
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    const double scaleFactor = 1.1;    // 每次缩放系数
    const double minScale = 0.1;        // 最小缩放倍数
    const double maxScale = 5.0;        // 最大缩放倍数

    // 当前视图矩阵的缩放值（假设 x、y 方向相同）
    double currentScale = transform().m11();

    if (event->angleDelta().y() > 0) {
        // 放大
        if (currentScale < maxScale) {
            scale(scaleFactor, scaleFactor);
        }
    } else {
        // 缩小
        if (currentScale > minScale) {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    emit mouseMoved(pos); // 发信号给 AlbumWindow 更新按钮
    QGraphicsView::mouseMoveEvent(event);
}
