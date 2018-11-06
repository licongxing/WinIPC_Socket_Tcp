#include "socketthread.h"

SocketThread::SocketThread(SOCKET mListen,QWidget *parent) : QThread(parent)
{
    this->mListen = mListen;
    this->parent = parent;
}
SocketThread::~SocketThread()
{
    qDebug() << "SocketThread 析构函数";
    closesocket(mListen);
    emit isClose();
}
void SocketThread::run()
{
        sockaddr_in clientAddr;
        int size = sizeof(clientAddr);
        while(!isInterruptionRequested()){
            //每次接受新客户端将之前的地址信息清0
            memset(&clientAddr,0,sizeof(clientAddr));
            //等待新客户端连接 阻塞函数，结束线程 使用requestInterruption打断线程并没有用，只能使用terminate 终止线程
            SOCKET client = ::accept(mListen,(sockaddr*)&clientAddr,&size);
            char* clientIp = inet_ntoa(clientAddr.sin_addr);
            int clientPort = ntohs(clientAddr.sin_port);
            QTime time = QTime::currentTime();
            QString str = time.toString("hh:mm:ss");
            QString msg = QString("%1 [%2:%3] connect success").arg(str).arg(clientIp).arg(clientPort);
            //新客户端连接，通知 UI 更新界面
            emit isMsg(msg);
            //开启新线程和客户端进行通信
            MsgThread* msgThread = new MsgThread(client,clientAddr,parent);
            msgThread->start();

            connect(msgThread,&MsgThread::isMsg,this,[=](QString msg){
                //转发消息给 UI进程，UI进行界面更新
                emit isMsg(msg);
            });
            connect(this,&SocketThread::isClose,this,[=](){
                msgThread->terminate();
                msgThread->quit();
                delete msgThread;
            });
        }
}
