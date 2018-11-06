#include "msgthread.h"

MsgThread::MsgThread(SOCKET mClient,sockaddr_in mAddr,QWidget *parent) : QThread(parent)
{
    this->mClient = mClient;
    this->mAddr = mAddr;
}
MsgThread::~MsgThread(){
    qDebug() << "msgThread 析构函数";
    int ret = closesocket(mClient);
    if( ret == 0 ){
        qDebug()<< "关闭成功";
    }else{
        qDebug()<< "关闭失败";
    }
}
void MsgThread::run(){
        //inet_addr点分十进制转网络ip地址 ,inet_ntoa网络转点分十进制
        char* clientIp = inet_ntoa(mAddr.sin_addr);
        int clientPort = ntohs(mAddr.sin_port);
        while(true){
            memset(resp,0,1024);
            char buf[1024] = {0};
            //阻塞等待 接受信息
            int ret = recv(mClient,buf,1024,0);
            QTime time = QTime::currentTime();
            QString str = time.toString("hh:mm:ss");
            if(ret == 0){//连接断开
                emit isMsg(QString("%1 [%2:%3] is closed!!!").arg(str).arg(clientIp).arg(clientPort));
                break;
            }
            QString msg = QString("%1 [%2:%3]:%4").arg(str).arg(clientIp).arg(clientPort).arg(buf);
            //接受到消息，通知UI 界面更新
            emit isMsg(msg);

            //给用户进行响应消息，小写变大写
            strcpy(resp,QString(buf).toUpper().toUtf8().data());
            qDebug() << "给客户端发送消息：" << resp;
            send(mClient,resp,strlen(resp)+1,0);
        }
}
