#include "msgthread.h"

MsgThread::MsgThread(SOCKET mSocket,QWidget *parent) : QThread(parent)
{
    this->mSocket = mSocket;
}

void MsgThread::run(){

    char buf[1024] = {0};
    while(true){
        if(mIsSend){
            qDebug() << "msg:"<< mMsg;
            //发送
            ::send(mSocket,mMsg.toUtf8().data(),mMsg.length() +1,0);
            int errorCode = WSAGetLastError();
            //连接超时
            if( errorCode == WSAECONNABORTED){
                emit isClose();
                return;
            }
            //等待服务端回应
            int ret = ::recv(mSocket,buf,1024,0);
            //服务端关闭
            if( ret == 0){
                emit isClose();
                return;
            }
            QTime time = QTime::currentTime();
            QString str = time.toString("hh:mm:ss");
            qDebug() << "buf:" << buf ;
            QString msg = QString("%1 收到消息[%2]").arg(str).arg(buf);
            emit isMsg(msg);
            mIsSend = false;
        }
    }

}
void MsgThread::dealSend(QString msg){
    mIsSend = true;
    mMsg = msg;
}
