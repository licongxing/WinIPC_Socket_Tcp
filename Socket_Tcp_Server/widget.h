#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <winsock2.h>
#include <socketthread.h>
#include <QDebug>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
   void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Widget *ui;
    WSADATA mWasData;
    SOCKET mListen = INVALID_SOCKET;
    SocketThread *mSocketThread;
};

#endif // WIDGET_H
