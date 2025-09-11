#include "MyListViewMenu.h"
#include "ui_MyListViewMenu.h"
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

MyListViewMenu::MyListViewMenu(QWidget *parent) :
    QWidget(parent, Qt::Popup) // Qt::Popup 保证菜单行为
  ,ui(new Ui::MyListViewMenu)
{
    ui->setupUi(this);
    // 设置菜单为无边框小窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    // 初始化图片类型
    imageSuffixes <<"jpg"<<"jpeg"<<"png"<<"bmp"<<"gif";

}

MyListViewMenu::~MyListViewMenu()
{
    delete ui;
}

void MyListViewMenu::setCurrentIndexes(const QModelIndexList &indexes)
{
    selectedIndexes = indexes;
}

void MyListViewMenu::setCurrentIndex(const QModelIndex &index)
{
    const QFileSystemModel *model = static_cast<const QFileSystemModel*>(index.model());
    filePath = model->filePath(index);
    fileName = model->fileName(index);
}

void MyListViewMenu::setInitPath(const QString &path)
{
    initPath = path;
}

void MyListViewMenu::on_menu_btn_open_clicked()
{
    QFileInfo info(filePath);
    QString dirPath = info.absolutePath();

    // 调用系统默认文件管理器打开文件夹
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void MyListViewMenu::on_menu_btn_delete_clicked()
{
    if (selectedIndexes.isEmpty()) return;

    int deletedCount = 0;
    for (const QModelIndex &index : selectedIndexes) {
        const QAbstractItemModel *abstractModel = index.model();
        const QFileSystemModel *model = qobject_cast<const QFileSystemModel*>(abstractModel);
        if (!model) continue;
        QString filePath = model->filePath(index);

        QFileInfo fi(filePath);

        if (fi.isDir()) {
            QDir dir(filePath);
            if (dir.removeRecursively()) deletedCount++;
        } else if (fi.isFile()) {
            if (QFile::remove(filePath)) deletedCount++;
        }
    }
    QMessageBox::information(this, "提示", QString("已删除 %1 个文件/文件夹").arg(deletedCount));
}

void MyListViewMenu::on_menu_btn_rename_clicked()
{
    // 获取旧文件信息
    QFileInfo fi(filePath);
    QString fileName = fi.completeBaseName();  // 不带后缀的文件名
    QString suffix   = fi.suffix();           // 后缀

    // 弹出输入框，获取新名字（不带后缀）
    bool ok;
    QString newBaseName = QInputDialog::getText(this, "重命名",
                                                "请输入新文件名（不包含后缀）：",
                                                QLineEdit::Normal,
                                                fileName, &ok);
    if (!ok || newBaseName.isEmpty()) return;

    // 拼接新的完整路径（保留后缀）
    QString newPath = fi.absolutePath() + "/" + newBaseName;
    if (!suffix.isEmpty()) {
        newPath += "." + suffix;
    }

    // 执行重命名
    if (QFile::rename(filePath, newPath)) {
        return;
    } else {
        QMessageBox::warning(this, "错误", "重命名失败！");
    }

}

void MyListViewMenu::on_menu_btn_move_clicked()
{
    if (selectedIndexes.isEmpty()) return;

    QString targetDir = QFileDialog::getExistingDirectory(this, "选择目标文件夹", initPath,
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (targetDir.isEmpty()) return;

    int movedCount = 0;
    for (const QModelIndex &index : selectedIndexes) {
        const QAbstractItemModel *abstractModel = index.model();
        const QFileSystemModel *model = qobject_cast<const QFileSystemModel*>(abstractModel);
        if (!model) continue;
        QString filePath = model->filePath(index);


        QFileInfo fi(filePath);
        if (fi.isFile() && imageSuffixes.contains(fi.suffix().toLower())) {
            QString newPath = targetDir + "/" + fi.fileName();
            if (QFile::rename(filePath, newPath)) movedCount++;
        }
    }
    QMessageBox::information(this, "提示", QString("已移动 %1 张图片").arg(movedCount));
}

void MyListViewMenu::on_menu_btn_copy_clicked()
{
    if (selectedIndexes.isEmpty()) return;

    QString targetDir = QFileDialog::getExistingDirectory(this, "选择目标文件夹", initPath,
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (targetDir.isEmpty()) return;

    int copiedCount = 0;
    for (const QModelIndex &index : selectedIndexes) {
        const QAbstractItemModel *abstractModel = index.model();
        const QFileSystemModel *model = qobject_cast<const QFileSystemModel*>(abstractModel);
        if (!model) continue;
        QString filePath = model->filePath(index);


        QFileInfo fi(filePath);
        if (fi.isFile() && imageSuffixes.contains(fi.suffix().toLower())) {
            QString newPath = targetDir + "/" + fi.fileName();
            if (QFile::copy(filePath, newPath)) copiedCount++;
        }
    }
    QMessageBox::information(this, "提示", QString("已复制 %1 张图片").arg(copiedCount));
}
