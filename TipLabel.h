/*
 * 作者: 樊晓亮
 * 日期: 2025-09-12
 * 功能: 提示标签（支持居中显示，自动消失，带渐隐下移动画）
 * 使用方法:
 *   TipLabel::showTip(parent, "提示文字", 2000);
 */

#ifndef TIPLABEL_H
#define TIPLABEL_H

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class TipLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TipLabel(QWidget *parent, const QString &text, int duration = 2000);

    // 静态调用方法
    static void showTip(QWidget *parent, const QString &text, int duration = 2000);

private:

};

#endif // TIPLABEL_H
