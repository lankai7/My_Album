#include "AlbumWindow.h"
#include "ui_AlbumWindow.h"
#include <QFileDialog>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QTimer>
#include <windows.h>
#include <windowsx.h>
#include <QGraphicsPixmapItem>


AlbumWindow::AlbumWindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::AlbumWindow)
    ,model(new QFileSystemModel(this))
{
    ui->setupUi(this);

    // 设置无边框（隐藏标题栏，但保留最小化、最大化、关闭等功能）
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, false); // 如果需要透明再改 true

    ui->label_msg->setFixedHeight(60);
    ui->label_msg->setAlignment(Qt::AlignLeft | Qt::AlignTop);


    // 初始化 scene
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 创建一个存放相册的文件夹Album
    QDir dir("Album");
    if (dir.exists()) {
        qDebug()<<"Album is existence!";
    }
    else{
        QDir dir("");
        if(dir.mkpath("Album")){
            qDebug()<<"Album create success!";
        }
        else{
            qDebug()<<"!Album is err";
        }
    }

    // 初始化文件模型
    model = new QFileSystemModel(this);
    QString albumPath = QDir::currentPath() + "/Album"; // 初始文件夹
    model->setRootPath(albumPath);

    // 只显示图片文件
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif";
    model->setNameFilters(filters);
    model->setNameFilterDisables(false);

    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index(albumPath));

    // 按钮初始隐藏或显示都可以
    ui->prevButton->setVisible(false);
    ui->nextButton->setVisible(false);
    ui->buttonContainer->setVisible(false);
    ui->info_widget->setVisible(false);

    // 设置父控件为 imageContainer，让按钮漂浮在 graphicsView 上
    ui->prevButton->setParent(ui->imageContainer);
    ui->nextButton->setParent(ui->imageContainer);
    ui->buttonContainer->setParent(ui->imageContainer);

    // 保证按钮显示在 graphicsView 上层
    ui->prevButton->raise();
    ui->nextButton->raise();
    ui->buttonContainer->raise();

    ui->graphicsView->show();
    resizeEvent(nullptr);

    // 在构造函数里绑定点击信号
    connect(ui->listView, &QListView::clicked,
            this, &AlbumWindow::onListViewClicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &AlbumWindow::onPrevClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &AlbumWindow::onNextClicked);
    connect(ui->listView, &QListView::doubleClicked,
            this, &AlbumWindow::onListViewDoubleClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &AlbumWindow::onBackClicked);
    connect(static_cast<ImageView*>(ui->graphicsView), &ImageView::mouseMoved,
            this, &AlbumWindow::onImageViewMouseMoved);

    setMouseTracking(true);             // 窗口本身捕获鼠标移动
    ui->centralwidget->setMouseTracking(true); // centralwidget 也捕获鼠标移动

}

AlbumWindow::~AlbumWindow()
{
    delete ui;
}

//选择相册打开相册文件夹
void AlbumWindow::on_album_open_clicked()
{
    // 打开文件夹选择对话框
    QString dir = QFileDialog::getExistingDirectory(
                this,                      // 父窗口
                tr("选择文件夹"),          // 对话框标题
                QDir::homePath(),          // 初始路径，这里默认是用户主目录
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );

    // 判断用户是否选择了文件夹
    if (!dir.isEmpty()) {
        // 输出或使用文件夹路径，这里演示打印到调试输出
        qDebug() << "选择的文件夹路径:" << dir;
        // 设置模型的根目录
        QModelIndex index = model->setRootPath(dir);
        ui->listView->setRootIndex(index);
    }
    else {
        qDebug() << "未选择文件夹";
    }
}

// 槽函数：点击 listView 显示图片
void AlbumWindow::onListViewClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    // 获取文件路径
    QString filePath = model->filePath(index);
    QFileInfo info(filePath);

    // 显示图片名字到 QLabel
    ui->pix_name->setText(info.fileName()); // 只显示文件名，不含路径

    // 简单判断是不是图片
    QString suffix = info.suffix().toLower();
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp" || suffix == "gif") {

        currentIndex = index;

        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            scene->clear();
            // 保持原始尺寸加入 scene
            item = scene->addPixmap(pixmap);
            item->setTransformationMode(Qt::SmoothTransformation); // 缩放时高质量
            scene->setSceneRect(pixmap.rect());
            //设置图片围绕中心旋转
            item->setTransformOriginPoint(item->boundingRect().center());

            // 自动缩放让图片完整显示
            ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);
            ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

            // 可选：稍微缩小一点，比如缩小 90%
            ui->graphicsView->scale(0.9, 0.9);
        } else {
            qDebug() << "图片加载失败：" << filePath;
        }
    } else {
        qDebug() << "不是图片文件：" << filePath;
    }
}

//上一张图片
void AlbumWindow::onPrevClicked()
{
    if (!currentIndex.isValid())
        return;

    int row = currentIndex.row() - 1;
    QModelIndex parentIndex = currentIndex.parent();
    if (row >= 0) {
        QModelIndex prev = model->index(row, 0, parentIndex);
        ui->listView->setCurrentIndex(prev);
        onListViewClicked(prev);
    }
}

//下一张图片
void AlbumWindow::onNextClicked()
{
    if (!currentIndex.isValid())
        return;

    int row = currentIndex.row() + 1;
    QModelIndex parentIndex = currentIndex.parent();
    if (row < model->rowCount(parentIndex)) {
        QModelIndex next = model->index(row, 0, parentIndex);
        ui->listView->setCurrentIndex(next);
        onListViewClicked(next);
    }
}

//自动计算按钮位置
void AlbumWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // 计算按钮垂直居中，靠左右
    int midY = ui->imageContainer->height()/2;
    ui->prevButton->move(25, midY - ui->prevButton->height()/2);
    ui->nextButton->move(ui->imageContainer->width() - ui->nextButton->width() - 25,
                         midY - ui->nextButton->height()/2);

    int x = (ui->imageContainer->width() - ui->buttonContainer->width()) / 2; // 横向居中
    int y = ui->imageContainer->height() - ui->buttonContainer->height() - 0; // 距离底部 0 像素
    ui->buttonContainer->move(x, y);
}

//分别处理文件夹与图片
void AlbumWindow::onListViewDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString path = model->filePath(index);
    QFileInfo info(path);

    if (info.isDir()) {
        // 双击文件夹 → 切换 listView 根目录
        QString currentPath = model->filePath(ui->listView->rootIndex());
        if (!currentPath.isEmpty()) {
            backStack.push(currentPath);   // 当前路径入后退栈
        }
        ui->listView->setRootIndex(model->index(path));
    } else {
        // 双击图片 → 显示图片
        onListViewClicked(index);
    }

}

//返回按钮
void AlbumWindow::onBackClicked()
{
    if (backStack.isEmpty())
        return;

    QString prevPath = backStack.pop();
    ui->listView->setRootIndex(model->index(prevPath));
}

//列表隐藏按钮
void AlbumWindow::on_list_hide_clicked()
{
    if (ui->listView->isVisible()) {
        // listView 当前可见 → 隐藏
        ui->listView->setVisible(false); // 隐藏
        ui->list_hide->setText("显示列表");

    } else {
        // listView 当前隐藏 → 显示
        ui->listView->setVisible(true);  // 显示
        ui->list_hide->setText("隐藏列表");
    }
    QTimer::singleShot(0, this, [this]() {
        resizeEvent(nullptr);
    });
}

//最小化按钮
void AlbumWindow::on_min_btn_clicked()
{
    this->showMinimized();
}

//最大化按钮
void AlbumWindow::on_win_btn_clicked()
{
    if (this->isMaximized()) {
        // 如果已经最大化 → 切换为普通窗口
        this->showNormal();
    } else {
        // 如果是普通窗口 → 切换为最大化
        this->showMaximized();
    }
}

//关闭窗口按钮
void AlbumWindow::on_off_btn_clicked()
{
    this->close();
}

//================================鼠标事件================================
void AlbumWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    const int titleBarHeight = 40; // 标题栏高度，可根据UI调整

    // 如果双击位置在标题栏区域
    if (event->y() >= 0 && event->y() <= titleBarHeight) {
        if (isMaximized()) {
            // 还原窗口
            showNormal();
            if (!normalGeometry.isNull())
                setGeometry(normalGeometry);
        } else {
            // 最大化窗口前先保存当前大小
            normalGeometry = geometry();
            showMaximized();
        }
        event->accept();
    } else {
        QWidget::mouseDoubleClickEvent(event);
    }
}

void AlbumWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        Qt::CursorShape currentShape = cursor().shape();
        if (currentShape == Qt::SizeBDiagCursor) {
            resizeEdge = BottomLeft;
        }
        else if (currentShape == Qt::SizeFDiagCursor) {
            resizeEdge = BottomRight;
        }
        else if (currentShape == Qt::SizeHorCursor&&isLeft) {
            resizeEdge = Left;
        }
        else if (currentShape == Qt::SizeHorCursor&&!isLeft) {
            resizeEdge = Right;
        }
        else if (currentShape == Qt::SizeVerCursor) {
            resizeEdge = Bottom;
        }
        else {
            resizeEdge = None;
        }
        if (resizeEdge != None) {
            isResizing = true;
        } else if(event->pos().y() <= 50){
            // 普通拖动
            isDragging = true;
            dragPosition = event->globalPos() - frameGeometry().topLeft();
        }

        event->accept();
    }
}

void AlbumWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QRect rect = this->rect();

    //拉伸窗口大小
    Qt::CursorShape cursorShape = Qt::ArrowCursor;
    bool left = pos.x() <= borderWidth;
    bool right = pos.x() >= rect.width() - borderWidth;
    bool bottom = pos.y() >= rect.height() - borderWidth;
    if(!this->isMaximized()){
        if (bottom && left) cursorShape = Qt::SizeBDiagCursor;
        else if (bottom && right) cursorShape = Qt::SizeFDiagCursor;
        else if (left) {
            cursorShape = Qt::SizeHorCursor;
            isLeft = true;
        }
        else if (right) {
            cursorShape = Qt::SizeHorCursor;
            isLeft = false;
        }
        else if (bottom) cursorShape = Qt::SizeVerCursor;
    }


    setCursor(cursorShape);

    if (!rect.contains(pos)) {
        unsetCursor();
    }
    // 继续原有拖动或调整大小逻辑

    if (isResizing) {
        QRect geo = geometry();

        switch (resizeEdge) {
        case Left: geo.setLeft(event->globalX()); break;
        case Right: geo.setRight(event->globalX()); break;
        case Bottom: geo.setBottom(event->globalY()); break;
        case BottomLeft: geo.setBottomLeft(event->globalPos()); break;
        case BottomRight: geo.setBottomRight(event->globalPos()); break;
        default: break;
        }

        setGeometry(geo);
        event->accept();
        return;
    }

    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        if (isMaximized()) {
            double xRatio = (double)event->pos().x() / width();
            double yRatio = (double)event->pos().y() / height();

            if (normalGeometry.isNull()) normalGeometry = QRect(100,100,1600,900);

            showNormal();
            resize(normalGeometry.size());

            QPoint newTopLeft = event->globalPos() - QPoint(xRatio * width(), yRatio * height());
            move(newTopLeft);

            dragPosition = event->globalPos() - frameGeometry().topLeft();
        } else {
            move(event->globalPos() - dragPosition);
        }

        event->accept();
    }
}

void AlbumWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isDragging = false;
    isResizing = false;
    resizeEdge = None;
}

void AlbumWindow::onImageViewMouseMoved(const QPoint &pos)
{
    // 检查鼠标是否靠近左按钮或右按钮
    int showDistance = 220; // 鼠标靠近 就显示
    if (pos.x() <= showDistance)
        ui->prevButton->setVisible(true);
    else
        ui->prevButton->setVisible(false);

    if (pos.x() >= ui->graphicsView->width() - showDistance)
        ui->nextButton->setVisible(true);
    else
        ui->nextButton->setVisible(false);
    //检查鼠标是否接近工具栏
    if (pos.y() >= ui->graphicsView->height() - showDistance)
        ui->buttonContainer->setVisible(true);
    else
        ui->buttonContainer->setVisible(false);
}
//=======================================================================

//左转按钮
void AlbumWindow::on_turn_left_clicked()
{
    if(item!=nullptr)
        item->setRotation(item->rotation() - 90);
}

//右转按钮
void AlbumWindow::on_turn_right_clicked()
{
    if(item!=nullptr)
        item->setRotation(item->rotation() - 90);
}

//信息按钮
void AlbumWindow::on_info_btn_clicked()
{
    if (ui->info_widget->isVisible()){
        ui->info_widget->setVisible(false);
    }
    else{
        ui->info_widget->setVisible(true);
    }
    QTimer::singleShot(0, this, [this]() {
        resizeEvent(nullptr);
    });
}

void AlbumWindow::on_btn_return_clicked()
{
    on_info_btn_clicked();
}
