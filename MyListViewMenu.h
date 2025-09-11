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

    //获取索引
    void setCurrentIndex(const QModelIndex &index);
    //获取初始地址
    void setInitPath(const QString &albumPath);

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
    //保存所选项目
    QModelIndex currentIndex;
    //保存所选项目地址,名字
    QString filePath;
    QString fileName;
    //初始地址
    QString initPath;
};

#endif // MYLISTVIEWMENU_H
