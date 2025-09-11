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

}

MyListViewMenu::~MyListViewMenu()
{
    delete ui;
}

void MyListViewMenu::setCurrentIndex(const QModelIndex &index)
{
    currentIndex = index;
    const QFileSystemModel *model = static_cast<const QFileSystemModel*>(currentIndex.model());
    filePath = model->filePath(currentIndex);
    fileName = model->fileName(currentIndex);
}

void MyListViewMenu::setInitPath(const QString &albumPath)
{
    initPath = albumPath;
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
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  "你确定要删除这个文件吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if(QFile::remove(filePath)){
            return;
        }
        else{
            QMessageBox::critical(this, "错误", "删除文件失败！");
        }
    } else {
        qDebug() << "用户取消删除";
    }

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
    // 打开文件夹选择对话框，初始路径是 initPath
    QString targetDir = QFileDialog::getExistingDirectory(
                this,
                "选择目标文件夹",
                initPath,                  // 默认初始文件夹
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    if (targetDir.isEmpty()) return;

    // 定义允许移动的图片后缀
    QStringList imageSuffixes = {"jpg", "jpeg", "png", "bmp", "gif"};

    // 判断选中的 filePath 是否是文件夹还是单个文件
    QFileInfo fi(filePath);

    if (fi.isDir()) {
        // 如果是文件夹，遍历文件夹下的内容
        QDir dir(filePath);
        QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        int movedCount = 0;
        for (const QFileInfo &entry : entries) {
            QString suffix = entry.suffix().toLower();
            if (imageSuffixes.contains(suffix)) {
                QString newPath = targetDir + "/" + entry.fileName();
                if (QFile::rename(entry.absoluteFilePath(), newPath)) {
                    movedCount++;
                }
            }
        }
        QMessageBox::information(this, "提示",
                                 QString("已移动 %1 张图片").arg(movedCount));
    } else if (fi.isFile()) {
        // 单个文件
        QString newPath = targetDir + "/" + QFileInfo(filePath).fileName();
        if (QFile::rename(filePath, newPath)) {
            return;
        } else {
            QMessageBox::warning(this, "错误", "移动失败！");
        }
    }
}

void MyListViewMenu::on_menu_btn_copy_clicked()
{
    // 打开文件夹选择对话框，初始路径是 initPath
    QString targetDir = QFileDialog::getExistingDirectory(
                this,
                "选择目标文件夹",
                initPath,                  // 默认初始文件夹
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    if (targetDir.isEmpty()) return;

    // 定义允许复制的图片后缀
    QStringList imageSuffixes = {"jpg", "jpeg", "png", "bmp", "gif"};

    // 判断选中的 filePath 是否是文件夹还是单个文件
    QFileInfo fi(filePath);

    if (fi.isDir()) {
        // 如果是文件夹，遍历文件夹下的内容
        QDir dir(filePath);
        QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        int copiedCount = 0;
        for (const QFileInfo &entry : entries) {
            QString suffix = entry.suffix().toLower();
            if (imageSuffixes.contains(suffix)) {
                QString newPath = targetDir + "/" + entry.fileName();
                if (QFile::copy(entry.absoluteFilePath(), newPath)) {
                    copiedCount++;
                }
            }
        }
        QMessageBox::information(this, "提示",
                                 QString("已复制 %1 张图片").arg(copiedCount));
    } else if (fi.isFile()) {
        // 单个文件
        QString newPath = targetDir + "/" + fi.fileName();
        if (QFile::copy(fi.absoluteFilePath(), newPath)) {
            return;
        } else {
            QMessageBox::warning(this, "错误", "图片复制失败！");
        }
    }

}
