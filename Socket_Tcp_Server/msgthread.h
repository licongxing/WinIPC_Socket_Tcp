#ifndef MSGTHREAD_H
#define MSGTHREAD_H

#include <QThread>
#include <QWidget>
#include <winsock2.h>
#include <QTime>
#include <QDebug>

class MsgThread : public QThread
{
    Q_OBJECT
public:
    explicit MsgThread(SOCKET mClient,sockaddr_in mAddr,QWidget *parent = nullptr);
    ~MsgThread();
    void run();
signals:
    void isMsg(QString msg);//客户端socket关闭了 客户端发消息来了
public slots:
private:
    //和客户端通信的 socket
    SOCKET mClient;
    //客户端地址
    sockaddr_in mAddr;
    char resp[1024] = {0};
};

#endif // MSGTHREAD_H
