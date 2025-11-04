#ifndef BUTTONCONTAINER_H
#define BUTTONCONTAINER_H

#include <QObject>
#include <QWidget>

class ButtonContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonContainer(QWidget *parent = nullptr);
signals:
    void mouseLeave();
    void mouseMove();
private:
    void leaveEvent(QEvent *event)override;
    void enterEvent(QEvent *event)override;
};

#endif // ButtonContainer_H
