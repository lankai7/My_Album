#include "TipLabel.h"
#include <QTimer>

TipLabel::TipLabel(QWidget *parent, const QString &text, int duration)
    : QLabel(parent)
{
    setText(text);
    setStyleSheet("background-color: rgba(0,0,0,180,0.5); color: white; padding: 5px; border-radius: 4px;font: 13px;");
    setAttribute(Qt::WA_ShowWithoutActivating); // 不抢焦点
    adjustSize();

    // 居中偏下 100 像素
    int x = (parent->width() - width()) / 2;
    int y = 100;
    move(x, y);

    show();

    // duration 毫秒后自动关闭
    QTimer::singleShot(duration, this, &TipLabel::deleteLater);
}

// 静态方法实现
void TipLabel::showTip(QWidget *parent, const QString &text, int duration)
{
    new TipLabel(parent, text, duration);
}
