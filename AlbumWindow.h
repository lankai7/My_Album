#ifndef ALBUMWINDOW_H
#define ALBUMWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <QFileSystemModel>
#include <QGraphicsScene>
#include <QStack>

QT_BEGIN_NAMESPACE
namespace Ui { class AlbumWindow; }
QT_END_NAMESPACE

class AlbumWindow : public QMainWindow
{
    Q_OBJECT

public:
    AlbumWindow(QWidget *parent = nullptr);
    ~AlbumWindow();
protected:

private:
    Ui::AlbumWindow *ui;
    QGraphicsPixmapItem *item = nullptr; //图片展示
    QFileSystemModel *model;  // 文件系统模型
    QGraphicsScene *scene;         // 场景对象，用于显示图片
    QModelIndex currentIndex;   // 当前显示的图片索引
    QStack<QString> backStack;   // 后退历史
    QPoint dragPosition;
    QRect normalGeometry; // 记录窗口普通大小
    bool isDragging = false; //是否拖动位置
    bool isResizing = false;   // 是否在调整大小
    enum ResizeEdge { None, Left, Right, Top, Bottom, TopLeft, TopRight, BottomLeft, BottomRight };
    ResizeEdge resizeEdge = None;
    const int borderWidth = 5; // 调整大小的边缘宽度
    bool isLeft;

    void onPrevClicked();
    void onNextClicked();
    void updateButtonPosition();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void mouseDoubleClickEvent(QMouseEvent *event)override;
private slots:

    void onListViewClicked(const QModelIndex &index);
    void on_album_open_clicked();
    void onListViewDoubleClicked(const QModelIndex &index);
    void onBackClicked();
    void on_list_hide_clicked();
    void on_min_btn_clicked();
    void on_win_btn_clicked();
    void on_off_btn_clicked();
    void onImageViewMouseMoved(const QPoint &pos); // 接收鼠标移动
    void on_turn_left_clicked();
    void on_turn_right_clicked();
    void on_info_btn_clicked();
    void on_btn_return_clicked();

signals:

};
#endif // ALBUMWINDOW_H
