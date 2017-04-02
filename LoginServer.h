#ifndef LOGINSERVER_H
#define LOGINSERVER_H
#define LOG(x) qDebug()<<(x)<<endl;(*LogTextStream)<<(x)<<endl;


#include <QtNetwork/QtNetwork>
#include <QList>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlError>

class LoginServer: public QObject{
public:
    LoginServer();
    ~LoginServer();


    QTcpServer* tcpServer;
    QTcpSocket *clientConnection;

    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;



};

LoginServer::LoginServer(){


    LogFile.setFileName("./LoginLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QDateTime::currentDateTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QDateTime::currentDateTime().toString()+"  "+"LogOpenFail")<<endl;



    //登录数据库


    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,6666);
    connect(tcpServer,&QTcpServer::newConnection,this,&LoginServer::readMessage);


}

LoginServer::~LoginServer(){


}

void LoginServer::readMessage(){

    clientConnection = tcpServer->nextPendingConnection();
    LOG(QDateTime::currentDateTime().toString()+"  "+"New Connection  "+clientConnection->peerAddress().toString());

    connect(clientConnection,&QTcpSocket::readyRead,this,&LoginServer::sendMessage);

}

void LoginServer::sendMessage(){

    QString message=QString::fromUtf8(clientConnection->readAll());
    qDebug()<<message<<endl;

    if(message.indexOf("@denglu@")!=-1){
        QStringList teststr=message.split("@");
        QStringList temp=message.split("|||");
        if(temp[1].length()<6||temp[2].length()<8||teststr.length()>3){

            QString str="@denglu@DBError@";

            LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());


            clientConnection->write(str.toUtf8());
        }
        else{
            QSqlQuery query;
            query.exec("select * from userandpassword where username=\""+temp[1]+"\"");
            if(!query.next()){

                QString str="@denglu@NoUsers@";
                LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                clientConnection->write(str.toUtf8());
            }
            else{
                if(query.value(2).toString()==temp[2]){

                    QString str="@denglu@Succeed@1.3";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                    clientConnection->write(str.toUtf8());
                }
                else{

                    QString str="@denglu@WrongPassword@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                    clientConnection->write(str.toUtf8());
                }

            }

        }
    }

    clientConnection->disconnectFromHost();
}





#endif // LOGINSERVER_H
