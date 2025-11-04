#include "ImageView.h"
#include <QDebug>
#include <QMimeData>
#include <QFileInfo>
#include <QShortcut>
#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include "TipLabel.h"


ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);      //抗锯齿
    setRenderHint(QPainter::Antialiasing);      // 平滑渲染
    setDragMode(QGraphicsView::ScrollHandDrag); // 可以拖动
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);   //鼠标位置为缩放中心
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

void ImageView::enterFullscreen()
{
    if (m_isFullscreen) {
        return; // 如果已经全屏，直接返回
    }

    m_isFullscreen = true;

    // 创建全屏窗口
    QMainWindow *fullscreenWindow = new QMainWindow();
    fullscreenWindow->setWindowState(Qt::WindowFullScreen);
    fullscreenWindow->setWindowTitle("全屏图片查看 - 按 ESC 或双击退出");
    fullscreenWindow->setStyleSheet("QMainWindow { background-color: #2b2b2b; }");

    QGraphicsView *fullscreenView = new ImageView(fullscreenWindow);
    fullscreenView->setScene(scene());

    // 隐藏滚动条
    fullscreenView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fullscreenView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    fullscreenView->setStyleSheet("QGraphicsView { background-color: #2b2b2b; border: none; }");

    // 启用鼠标跟踪
    fullscreenView->setMouseTracking(true);
    fullscreenView->viewport()->setMouseTracking(true);

    // 让图片适应视图大小，铺满屏幕
    if (fullscreenView->scene() && !fullscreenView->scene()->items().isEmpty()) {
        // 获取屏幕尺寸
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        // 获取图片的边界矩形
        QRectF imageRect = fullscreenView->scene()->itemsBoundingRect();

        // 计算适应屏幕的缩放比例
        qreal xScale = screenGeometry.width() / imageRect.width();
        qreal yScale = screenGeometry.height() / imageRect.height();
        qreal scale = qMin(xScale, yScale);

        // 应用缩放
        fullscreenView->resetTransform(); // 重置变换
        fullscreenView->scale(scale, scale);

        // 居中显示
        fullscreenView->centerOn(imageRect.center());
    }

    // 安装事件过滤器处理双击
    fullscreenView->viewport()->installEventFilter(this);

    // 保存指针
    m_fullscreenWindow = fullscreenWindow;
    m_fullscreenView = fullscreenView;

    fullscreenWindow->setCentralWidget(fullscreenView);
    fullscreenWindow->show();

    // ESC 快捷键 - 直接调用 toggleFullscreen
    QShortcut *escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), fullscreenWindow);
    connect(escapeShortcut, &QShortcut::activated, this, &ImageView::toggleFullscreen);

    // F11 快捷键 - 直接调用 toggleFullscreen
    QShortcut *f11Shortcut = new QShortcut(QKeySequence(Qt::Key_F11), fullscreenWindow);
    connect(f11Shortcut, &QShortcut::activated, this, &ImageView::toggleFullscreen);

    // 窗口销毁时重置状态
    connect(fullscreenWindow, &QMainWindow::destroyed, [this]() {
        if (m_isFullscreen) {
            m_isFullscreen = false;
            m_fullscreenWindow = nullptr;
            m_fullscreenView = nullptr;
            emit fullscreenChanged(false);
        }
    });

    emit fullscreenChanged(true);
}

void ImageView::exitFullscreen()
{
    if (!m_isFullscreen) return;

    m_isFullscreen = false;

    // 清理事件过滤器
    if (m_fullscreenView && m_fullscreenView->viewport()) {
        m_fullscreenView->viewport()->removeEventFilter(this);
    }

    // 关闭窗口
    if (m_fullscreenWindow) {
        m_fullscreenWindow->close();
        m_fullscreenWindow->deleteLater();
    }

    m_fullscreenWindow = nullptr;
    m_fullscreenView = nullptr;

    emit fullscreenChanged(false);
}

bool ImageView::eventFilter(QObject *obj, QEvent *event)
{
    if (m_isFullscreen && m_fullscreenView && obj == m_fullscreenView->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            // 处理左键和右键双击
            if (mouseEvent->button() == Qt::LeftButton || mouseEvent->button() == Qt::RightButton) {
                toggleFullscreen();
                return true;
            }
        }
    }
    return QGraphicsView::eventFilter(obj, event);
}

void ImageView::toggleFullscreen()
{
    if (m_isFullscreen) {
        exitFullscreen();
    } else {
        enterFullscreen();
    }
}

bool ImageView::isFullscreen() const
{
    return m_isFullscreen;
}

void ImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 处理左键和右键双击
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        toggleFullscreen();
        event->accept();
    } else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}

void ImageView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        toggleFullscreen();
        event->accept();
    } else if (event->key() == Qt::Key_Escape && m_isFullscreen) {
        toggleFullscreen(); // 统一使用 toggleFullscreen
        event->accept();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void ImageView::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
    QWidget *w = QApplication::widgetAt(QCursor::pos());
    // 如果鼠标还在按钮上，就不要发信号
    if (w && (w->objectName() == "prevButton" ||
              w->objectName() == "nextButton" ||
              w->objectName() == "buttonContainer"))
        return;

    emit mouseLeave();
}

