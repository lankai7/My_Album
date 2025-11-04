#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QPushButton>
#include <QWidget>
#include <QLayout>
#include <QLayoutItem>
#include <QMainWindow>

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = nullptr);
    // 添加全屏相关方法
    void enterFullscreen();
    void exitFullscreen();
    bool isFullscreen() const;
protected:
    void wheelEvent(QWheelEvent *event) override;
    //双击事件
    void mouseDoubleClickEvent(QMouseEvent *event)override;
    //F11快捷键放大
    void keyPressEvent(QKeyEvent *event)override;

signals:
     // 鼠标移动信号
    void mouseMoved(const QPoint &pos);
    // 当用户拖入图片文件时发出信号
    void imageDropped(const QString &filePath);
    //全屏
    void fullscreenChanged(bool isFullscreen);

public slots:
    //全屏信号
    void toggleFullscreen();

private:
    void mouseMoveEvent(QMouseEvent *event)override;
    bool m_isFullscreen = false;
    QMainWindow *m_fullscreenWindow = nullptr;  // 保存全屏窗口指针
    QGraphicsView *m_fullscreenView = nullptr;  // 保存全屏视图指针
    QWidget *m_originalParent = nullptr;
    QByteArray m_originalGeometry;
    QLayout *m_originalLayout = nullptr;    // 保存原始布局
    QLayoutItem *m_layoutItem = nullptr;    // 保存布局项
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // IMAGEVIEW_H
