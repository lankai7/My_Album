#ifndef MYLISTVIEWMENU_H
#define MYLISTVIEWMENU_H

#include <QWidget>
#include <QModelIndex>
#include <QFileSystemModel>

namespace Ui {
class MyListViewMenu;
}

class MyListViewMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MyListViewMenu(QWidget *parent = nullptr);
    ~MyListViewMenu();

    void setCurrentIndexes(const QModelIndexList &indexes); // 设置选中的项目
    void setCurrentIndex(const QModelIndex &index); // 设置选中的项目
    void setInitPath(const QString &path);                  // 设置默认初始路径

private slots:
    //打开所选项目文件夹
    void on_menu_btn_open_clicked();
    //删除
    void on_menu_btn_delete_clicked();
    //重命名
    void on_menu_btn_rename_clicked();
    //移动
    void on_menu_btn_move_clicked();
    //复制
    void on_menu_btn_copy_clicked();

private:
    Ui::MyListViewMenu *ui;
    QModelIndexList selectedIndexes;   // 当前选中的文件索引
    QString initPath;                  // 默认路径
    QStringList imageSuffixes;         // 支持的图片后缀
    QString filePath;                  //单选地址
    QString fileName;                  //单选名称
    QWidget *parent;
};

#endif // MYLISTVIEWMENU_H
