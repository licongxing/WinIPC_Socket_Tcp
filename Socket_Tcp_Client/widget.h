#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <windows.h>
#include <winsock2.h>
#include <msgthread.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void isSend(QString msg);

public slots:
    void dealMsg(QString msg);
    void dealClose();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Widget *ui;
    WSADATA mWasData;
    SOCKET mSocket;
    MsgThread* mMsgThread;
    QString mAddrStr;
};

#endif // WIDGET_H
