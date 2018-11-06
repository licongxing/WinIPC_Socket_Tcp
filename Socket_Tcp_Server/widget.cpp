#include "widget.h"
#include "ui_widget.h"


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
    ::closesocket(mListen);
    WSACleanup();
    delete ui;
}
//启动服务器
void Widget::on_pushButton_clicked()
{
    if(INVALID_SOCKET != mListen)
        return;

    //端口校验
    QString str = ui->lineEdit->text();
    QRegExp reg("[0-9]{1,5}");//0到65535
    QRegExpValidator validator(reg,this);
    int pos = 0;
    if( QValidator::Acceptable != validator.validate(str,pos)){
        ui->label_2->setText("启动失败！！！");
        ui->label_2->setStyleSheet("color:red;");
        return;
    }
    int port = str.toInt();
    if( port < 0 || port > 65535){
        ui->label_2->setText("启动失败！！！");
        ui->label_2->setStyleSheet("color:red;");
        return;
    }

    //创建套接字，正确返回有效的套接字
    mListen = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    int error;
    if(mListen == INVALID_SOCKET){
        error = WSAGetLastError();
        qDebug() << "socket error:" << error;
        return;
    }

    sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons((u_short)port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;//服务端可以这样写，主机的任意合适的地址
    //绑定套接字和地址信息
    int ret = bind(mListen,(sockaddr*)&addr,sizeof(addr));
    if( ret == SOCKET_ERROR){
        error = WSAGetLastError();
         qDebug() << "bind error:" << error;
         return;
    }

    //端口监听
    ret = ::listen(mListen,SOMAXCONN);
    if( ret == SOCKET_ERROR){
        error = WSAGetLastError();
          qDebug() << "listen error:" << error;
         return;
    }

    ui->label_2->setText("启动成功");
    ui->label_2->setStyleSheet("color:orange;");

    //启动后台进程和客户端进行连接，必须这样，因为ui线程不能被阻塞！
    mSocketThread = new SocketThread(mListen);
    mSocketThread->start();

    //信号槽连接
    connect(mSocketThread,&SocketThread::isMsg,[=](QString msg){
        ui->listWidget->addItem(msg);
    });
}
void Widget::closeEvent(QCloseEvent *event){
    if(mSocketThread != nullptr){
        mSocketThread->requestInterruption();
        mSocketThread->terminate();
        qDebug() << "释放 mSocketThread";
        mSocketThread->quit();
        mSocketThread->wait();
        delete mSocketThread;
    }
}

void Widget::on_pushButton_2_clicked()
{
    ui->listWidget->clear();
}
