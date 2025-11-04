#include "ButtonContainer.h"
#include <QDebug>

ButtonContainer::ButtonContainer(QWidget *parent) : QWidget(parent)
{

}

void ButtonContainer::leaveEvent(QEvent *event)
{
    emit mouseLeave();
    QWidget::leaveEvent(event);
}

void ButtonContainer::enterEvent(QEvent *event)
{
    emit mouseMove();
    QWidget::enterEvent(event);
}

