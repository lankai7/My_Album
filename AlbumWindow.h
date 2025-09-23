#ifndef ALBUMWINDOW_H
#define ALBUMWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <QFileSystemModel>
#include <QGraphicsScene>
#include <QStack>
#include "VideoPlayer.h"
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QGraphicsProxyWidget>

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
    QString albumPath;  //初始文件夹
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
    QMediaPlayer *player = nullptr;
    QGraphicsVideoItem *videoItem = nullptr;

    //connect 初始化
    void connectInit();
    //鼠标事件初始化
    void mouseEventInit();
    //初始化悬浮按钮
    void buttonViewInit();
    //初始化listView显示
    void listviewInit();
    //初始化默认相册
    void albumInit();
    //初始化窗口设置与ui设置
    void WindowInit();
    //自动计算按钮位置
    void resizeEvent(QResizeEvent *event) override;
    //鼠标事件
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void mouseDoubleClickEvent(QMouseEvent *event)override;
    //窗口事件
    void changeEvent(QEvent *event);
private slots:
    //上一张图片
    void onPrevClicked();
    //下一张图片
    void onNextClicked();
    // 槽函数：点击 listView 显示图片
    void onListViewClicked(const QModelIndex &index);
    //选择相册打开相册文件夹
    void on_album_open_clicked();
    //分别处理文件夹与图片
    void onListViewDoubleClicked(const QModelIndex &index);
    //返回按钮
    void onBackClicked();
    //列表隐藏按钮
    void on_list_hide_clicked();
    //最小化按钮
    void on_min_btn_clicked();
    //最大化按钮
    void on_win_btn_clicked();
    //关闭窗口按钮
    void on_off_btn_clicked();
    // 接收ImageView的鼠标移动
    void onImageViewMouseMoved(const QPoint &pos);
    //左转按钮
    void on_turn_left_clicked();
    //右转按钮
    void on_turn_right_clicked();
    //信息按钮
    void on_info_btn_clicked();
    //info页面关闭按钮
    void on_btn_return_clicked();
    //创建新相册
    void on_album_add_clicked();
    //右键菜单
    void onListViewContextMenu(const QPoint &pos);
    //返回初始目录
    void on_homeButton_clicked();
    //导入照片
    void on_pix_add_clicked();
    //照片信息
    void pix_info_init();
    //复制路径按钮
    void on_copy_btn_clicked();
    //修改文件名
    void on_lineEdit_name_returnPressed();

signals:

};
#endif // ALBUMWINDOW_H
