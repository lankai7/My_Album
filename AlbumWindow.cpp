#include "AlbumWindow.h"
#include "ui_AlbumWindow.h"
#include <QFileDialog>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QTimer>
#include <windows.h>
#include <windowsx.h>
#include <QGraphicsPixmapItem>
#include <QMenu>
#include "MyListViewMenu.h"
#include <QImageReader>
#include "ShowImageInfo.h"
#include "TipLabel.h"
#include <QApplication>
#include <QClipboard>


AlbumWindow::AlbumWindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::AlbumWindow)
    ,model(new QFileSystemModel(this))
{
    ui->setupUi(this);
    //初始化ui与窗口
    WindowInit();
    //初始化默认相册
    albumInit();
    //初始化listview
    listviewInit();
    //初始化悬浮按钮
    buttonViewInit();
    //初始化connect
    connectInit();
    //初始化鼠标事件
    mouseEventInit();

}

AlbumWindow::~AlbumWindow()
{
    delete ui;
}

void AlbumWindow::connectInit()
{
    // 连接信号与槽函数
    connect(ui->listView, &QListView::clicked,
            this, &AlbumWindow::onListViewClicked);       // 单击列表项 -> 触发选中处理
    connect(ui->prevButton, &QPushButton::clicked,
            this, &AlbumWindow::onPrevClicked);           // 点击上一张按钮 -> 显示上一张图片
    connect(ui->nextButton, &QPushButton::clicked,
            this, &AlbumWindow::onNextClicked);           // 点击下一张按钮 -> 显示下一张图片
    connect(ui->listView, &QListView::doubleClicked,
            this, &AlbumWindow::onListViewDoubleClicked); // 双击列表项 -> 打开图片
    connect(ui->backButton, &QPushButton::clicked,
            this, &AlbumWindow::onBackClicked);           // 点击返回按钮 -> 返回上一级界面
    connect(static_cast<ImageView*>(ui->graphicsView), &ImageView::mouseMoved,
            this, &AlbumWindow::onImageViewMouseMoved);   // 图片视图中鼠标移动 -> 更新位置信息
    connect(ui->listView, &QListView::customContextMenuRequested,   //连接listview右键菜单
            this, &AlbumWindow::onListViewContextMenu);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,   //图片的改变->信息栏的改变
            this, &AlbumWindow::pix_info_init);


}

void AlbumWindow::onListViewContextMenu(const QPoint &pos)
{
    QModelIndexList selectedIndexes = ui->listView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) return;
    QModelIndex index = ui->listView->indexAt(pos);
    if (!index.isValid()) return;
    MyListViewMenu *menu = new MyListViewMenu(this);
    menu->setCurrentIndex(index);   // 传入当前点击项
    menu->setCurrentIndexes(selectedIndexes);
    menu->setInitPath(albumPath); // 默认路径
    menu->move(ui->listView->viewport()->mapToGlobal(pos));
    menu->show();
}



void AlbumWindow::mouseEventInit()
{
    setMouseTracking(true);             // 窗口本身捕获鼠标移动
    ui->centralwidget->setMouseTracking(true); // centralwidget 也捕获鼠标移动
}

void AlbumWindow::buttonViewInit()
{
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
}

void AlbumWindow::listviewInit()
{
    // 初始化文件模型
    model = new QFileSystemModel(this);
    albumPath = QDir::currentPath() + "/Album"; // 初始文件夹
    model->setRootPath(albumPath);

    // 只显示图片文件
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif";
    model->setNameFilters(filters);
    model->setNameFilterDisables(false);

    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index(albumPath));

    //支持多选
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void AlbumWindow::albumInit()
{
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
}

void AlbumWindow::WindowInit()
{
    // 设置无边框（隐藏标题栏，但保留最小化、最大化、关闭等功能）
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, false); // 如果需要透明再改 true

    //设置缩放普通化默认大小
    normalGeometry = QRect(100,100,1200,700);

    // 初始化 scene
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->listView->setFocusPolicy(Qt::NoFocus); // 去掉焦点，这样虚线消失
    //设置右键菜单
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    //设置时间按钮隐藏
    ui->createTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->modifiedTime->setButtonSymbols(QAbstractSpinBox::NoButtons);

    qApp->setStyleSheet(
                "QToolTip {"
                "background-color: rgba(0,0,0,180);"
                "color: white;"
                "padding: 5px;"
                "border-radius: 4px;"
                "border: 1px solid white;"
                "}"
                );
    ui->label_name_image->setToolTip("文件名");
    ui->label_road_image->setToolTip("文件路径");
    ui->label_info_image->setToolTip("图片信息");
    ui->label_create_image->setToolTip("创建时间");
    ui->label_modified_image->setToolTip("修改时间");
    ui->label_size_image->setToolTip("文件大小");

}


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


void AlbumWindow::onPrevClicked()
{
    if (!currentIndex.isValid()){
        return;
    }

    int row = currentIndex.row() - 1;
    QModelIndex parentIndex = currentIndex.parent();
    if (row >= 0) {
        QModelIndex prev = model->index(row, 0, parentIndex);
        ui->listView->setCurrentIndex(prev);
        onListViewClicked(prev);
    }
    else{
        TipLabel::showTip(this, "✅已经到达列表顶部！");
    }
}


void AlbumWindow::onNextClicked()
{
    if (!currentIndex.isValid()){
        return;
    }

    int row = currentIndex.row() + 1;
    QModelIndex parentIndex = currentIndex.parent();
    if (row < model->rowCount(parentIndex)) {
        QModelIndex next = model->index(row, 0, parentIndex);
        ui->listView->setCurrentIndex(next);
        onListViewClicked(next);
    }
    else{
        TipLabel::showTip(this, "✅这已经是最后一张了！");
    }
}


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

    //计算悬浮框宽度
    if (ui->buttonContainer) {
            // 保持当前y坐标和高度不变，宽度适应父控件
            ui->buttonContainer->setGeometry(
                0,                                  // x坐标
                ui->buttonContainer->y(),           // y坐标保持不变
                ui->imageContainer->width(),        // 宽度等于父控件宽度
                ui->buttonContainer->height()       // 高度保持不变
            );
        }
}


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


void AlbumWindow::onBackClicked()
{
    if (backStack.isEmpty())
        return;

    QString prevPath = backStack.pop();
    ui->listView->setRootIndex(model->index(prevPath));
}


void AlbumWindow::on_list_hide_clicked()
{
    if (ui->listView->isVisible()) {
        // listView 当前可见 → 隐藏
        ui->listView->setVisible(false); // 隐藏
        ui->list_hide->setText("显示列表");
        ui->list_hide->setIcon(QIcon(":/new/res/hide.png"));

    } else {
        // listView 当前隐藏 → 显示
        ui->listView->setVisible(true);  // 显示
        ui->list_hide->setText("隐藏列表");
        ui->list_hide->setIcon(QIcon(":/new/res/unhide.png"));
    }
    QTimer::singleShot(0, this, [this]() {
        resizeEvent(nullptr);
    });
}


void AlbumWindow::on_min_btn_clicked()
{
    this->showMinimized();
}


void AlbumWindow::on_win_btn_clicked()
{
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
}


void AlbumWindow::on_off_btn_clicked()
{
    this->close();
}

//窗口改变事件
void AlbumWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (this->isMaximized()) {
            ui->win_btn->setIcon(QIcon(":/new/res/window.png"));
        } else {
            ui->win_btn->setIcon(QIcon(":/new/res/maxWin.png"));
        }
    }
    QMainWindow::changeEvent(event);
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


void AlbumWindow::on_turn_left_clicked()
{
    if(item!=nullptr)
        item->setRotation(item->rotation() - 90);
}


void AlbumWindow::on_turn_right_clicked()
{
    if(item!=nullptr)
        item->setRotation(item->rotation() + 90);
}


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

void AlbumWindow::on_album_add_clicked()
{
    // 弹出输入框，让用户输入新文件夹名称
    bool ok;
    QString folderName = QInputDialog::getText(this, "新建相册",
                                               "请输入相册名称：",
                                               QLineEdit::Normal,
                                               "", &ok);
    if (!ok || folderName.isEmpty()) return; // 用户取消或未输入

    // 获取当前 listView 显示的文件夹路径
    QModelIndex currentIndex = ui->listView->rootIndex();
    QString currentDirPath;
    if (currentIndex.isValid()) {
        const QFileSystemModel *model = qobject_cast<const QFileSystemModel*>(ui->listView->model());
        if (model) {
            currentDirPath = model->filePath(currentIndex);
        }
    }

    if (currentDirPath.isEmpty()) {
        TipLabel::showTip(this, "❗无法获取当前文件夹路径！");
        return;
    }

    // 构造新文件夹完整路径
    QString newFolderPath = currentDirPath + "/" + folderName;

    QDir dir;
    if (dir.exists(newFolderPath)) {
        TipLabel::showTip(this, "❌该相册已存在！");
        return;
    }

    // 创建文件夹
    if (dir.mkpath(newFolderPath)) {
        // 刷新模型显示新文件夹
        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->listView->model());
        if (model) model->setRootPath(currentDirPath);
    } else {
        TipLabel::showTip(this, "❌相册创建失败！");
    }
}



void AlbumWindow::on_homeButton_clicked()
{
    ui->listView->setRootIndex(model->index(albumPath));
}


void AlbumWindow::on_pix_add_clicked()
{
    // 获取当前 listView 显示的文件夹路径
    QModelIndex currentIndex = ui->listView->rootIndex();
    QString currentDirPath;
    if (currentIndex.isValid()) {
        const QFileSystemModel *model = qobject_cast<const QFileSystemModel*>(ui->listView->model());
        if (model) {
            currentDirPath = model->filePath(currentIndex);
        }
    }

    if (currentDirPath.isEmpty()) {
        TipLabel::showTip(this, "❌无法获取当前相册路径！");
        return;
    }

    // 弹出文件选择对话框，允许多选
    QStringList fileNames = QFileDialog::getOpenFileNames(
                this,
                "选择要导入的照片",
                QDir::homePath(),
                "图片文件 (*.jpg *.jpeg *.png *.bmp *.gif)");

    if (fileNames.isEmpty()) return;

    int importedCount = 0;
    for (const QString &filePath : fileNames) {
        QFileInfo fi(filePath);
        QString targetPath = currentDirPath + "/" + fi.fileName();

        // 如果目标已经存在同名文件，可以选择覆盖或者跳过
        if (QFile::exists(targetPath)) {
            // 可以选择覆盖，也可以 skip，这里跳过
            continue;
        }

        if (QFile::copy(filePath, targetPath)) {
            importedCount++;
        }
    }
    TipLabel::showTip(this, QString("✅成功导入 %1 张照片").arg(importedCount));

    // 刷新 listView 显示
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->listView->model());
    if (model) model->setRootPath(currentDirPath);
}

void AlbumWindow::pix_info_init()
{
    // 获取当前选中索引
    QModelIndex currentIndex = ui->listView->currentIndex();
    if (!currentIndex.isValid()) {
        qDebug() << "没有选中任何项";
        return;
    }

    // 获取模型
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->listView->model());
    if (!model) return;

    // 获取文件名
    QString fileName = model->fileName(currentIndex);
    // 获取文件地址
    QString filePath = model->filePath(currentIndex);

    ui->lineEdit_name->setText(fileName);
    ui->textEdit_road->setText(filePath);

    QImage img(model->filePath(currentIndex));
    QString info = ShowImageInfo::getImageInfo(img,filePath);
    ui->textEdit_msg->setText(info);

    // 获取创建时间以及修改时间
    QDateTime createTime = ShowImageInfo::getImageCreatedTime(filePath);
    QDateTime modifiedTime = ShowImageInfo::getImageModifiedTime(filePath);

    ui->createTime->setDateTime(createTime);
    ui->modifiedTime->setDateTime(modifiedTime);

    // 获取文件大小
    QString size = ShowImageInfo::getImageSize(filePath);
    ui->lineEdit_size->setText(size);

}



void AlbumWindow::on_copy_btn_clicked()
{
    if(ui->textEdit_road->toPlainText() == nullptr){
        TipLabel::showTip(this, "❎没有地址！");
        return;
    }
    // 获取剪切板
    QClipboard *clipboard = QApplication::clipboard();
    // 设置文本
    clipboard->setText(ui->textEdit_road->toPlainText());
    TipLabel::showTip(this, "✅已复制到剪切板！");
}

void AlbumWindow::on_lineEdit_name_returnPressed()
{
    // 获取当前选中索引
    QModelIndex currentIndex = ui->listView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    // 获取模型
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(ui->listView->model());
    if (!model) return;

    // 当前文件路径
    QString filePath = model->filePath(currentIndex);
    QFileInfo fi(filePath);

    // 获取LineEdit里的新名字（带后缀）
    QString newName = ui->lineEdit_name->text().trimmed();
    if (newName.isEmpty()) return;

    // 拼接新路径
    QString newPath = fi.absolutePath() + "/" + newName;

    // 执行重命名
    if (QFile::rename(filePath, newPath)) {
        TipLabel::showTip(this, "✅重命名成功！");

    } else {
        TipLabel::showTip(this, "❎重命名失败！");
    }
}
