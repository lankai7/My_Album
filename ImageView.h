#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QPushButton>

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = nullptr);
    // 提供接口设置按钮
protected:
    void wheelEvent(QWheelEvent *event) override;

signals:
    void mouseMoved(const QPoint &pos); // 鼠标移动信号

private:
    void mouseMoveEvent(QMouseEvent *event)override;
};

#endif // IMAGEVIEW_H
