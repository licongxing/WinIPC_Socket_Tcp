#include "widget.h"
#include "ui_widget.h"
#include <QFont>
#include <QDebug>
#include <QTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //winsock 初始化
    WSAStartup(MAKEWORD(2,2),&mWasData);
}

Widget::~Widget()
{
    closesocket(mSocket);
    //winsock 释放
    WSACleanup();
    if(mMsgThread!=nullptr)
        mMsgThread->quit();

    delete ui;
}
//连接服务器
void Widget::on_pushButton_clicked()
{
    mSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(mSocket == INVALID_SOCKET){
        ui->label->setText(QString("连接失败！！！"));
        ui->label->setStyleSheet("color:red;");
        return;
    }
    mAddrStr = ui->lineEdit->text();
    char ip[1024] = {0};
    int port = 0;
    sscanf(mAddrStr.toUtf8().data(),"%[^:]:%d",ip,&port);
    qDebug() << ip;
    qDebug() << port;
    SOCKADDR_IN addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons((u_short)port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip);//点分十进制转ip地址
    int val = ::connect(mSocket,(SOCKADDR*)&addr,sizeof(addr));
    int error;
    if( val == SOCKET_ERROR){
        ui->label->setText(QString("连接失败！！！"));
        ui->label->setStyleSheet("color:red;");
        error = WSAGetLastError();
        qDebug() << "connect error:" << error;
        return;
    }
    ui->label->setText(QString("连接成功"));
    ui->label->setStyleSheet("color:orange;");
    //创建处理和服务端通信的线程
    mMsgThread = new MsgThread(mSocket);
    mMsgThread->start();
    //信号槽连接
    connect(this,&Widget::isSend,mMsgThread,&MsgThread::dealSend);
    connect(mMsgThread,&MsgThread::isMsg,this,&Widget::dealMsg);
    connect(mMsgThread,&MsgThread::isClose,this,&Widget::dealClose);
}
//发送按钮槽函数
void Widget::on_pushButton_2_clicked()
{
    if(mSocket == INVALID_SOCKET){
        ui->label_2->setText(QString("发送失败"));
        return;
    }
    QString msg = ui->lineEdit_2->text();
    qDebug() << msg;
    //发送信息
    emit isSend(msg);
}
void Widget::dealMsg(QString msg)
{
    ui->listWidget->addItem(msg);
}

void Widget::dealClose()
{
    QTime time = QTime::currentTime();
    QString str = time.toString("hh:mm:ss");
    ui->listWidget->addItem(QString("%1 [%2]服务端断开").arg(str).arg(mAddrStr));
}

void Widget::on_pushButton_3_clicked()
{
    ui->listWidget->clear();
}
