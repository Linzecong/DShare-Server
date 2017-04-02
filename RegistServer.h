#ifndef REGISTSERVER_H
#define REGISTSERVER_H
#include <QtNetwork/QtNetwork>
#include <QList>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlError>
#define LOG(x) qDebug()<<(x)<<endl;(*LogTextStream)<<(x)<<endl;

class RegistServer: public QObject{
public:
    RegistServer();
    ~RegistServer();


    QTcpServer* tcpServer;
    QTcpSocket *clientConnection;

    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;



};

RegistServer::RegistServer(){


    LogFile.setFileName("./RegistLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);

        LOG(QDateTime::currentDateTime().toString()+"  "+"LogOpenSucceed");
    }
    else
        LOG(QDateTime::currentDateTime().toString()+"  "+"LogOpenFail");



//登录数据库

       tcpServer = new QTcpServer(this);
       tcpServer->listen(QHostAddress::AnyIPv4,5555);
       connect(tcpServer,&QTcpServer::newConnection,this,&RegistServer::readMessage);


}

RegistServer::~RegistServer(){


}

void RegistServer::readMessage(){

    clientConnection = tcpServer->nextPendingConnection();
    LOG(QDateTime::currentDateTime().toString()+"  "+"New Connection  "+clientConnection->peerAddress().toString());

    connect(clientConnection,&QTcpSocket::readyRead,this,&RegistServer::sendMessage);

}

void RegistServer::sendMessage(){

    QString message=QString::fromUtf8(clientConnection->readAll());
    qDebug()<<message<<endl;

    if(message.indexOf("@zhuce@")!=-1){
        QStringList teststr=message.split("@");


        QStringList temp=message.split("|||");
        if(temp[1].length()<6||temp[2].length()<8||temp[3]==""||teststr.length()>3){

            QString str="@zhuce@DBError@";
            LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

            clientConnection->write(str.toUtf8());
        }
        else{
              QSqlQuery query;
              query.exec("select * from userandpassword where username=\""+temp[1]+"\"");
              if(!query.next()){
                  query.prepare("insert into userandpassword (username,password,name,sex,age) values(?,?,?,?,?)");
                  query.addBindValue(temp[1]);
                  query.addBindValue(temp[2]);
                  query.addBindValue(temp[3]);
                  query.addBindValue(temp[4]=="男"?1:0);
                  query.addBindValue(temp[5].toInt());
                  query.exec();

                  QSqlQuery query1;
                  query1.prepare("insert into relationship (mainuser,userfriend) values(?,?)");
                  query1.addBindValue(temp[1]);
                  query1.addBindValue(temp[1]);
                  query1.exec();
                  if(query.lastError().type()==QSqlError::NoError){

                      QString str="@zhuce@Succeed@";

                      LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                      clientConnection->write(str.toUtf8());
                  }
                  else{
                      QString str="@zhuce@DBError@";

                      LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                      clientConnection->write(str.toUtf8());
                  }

              }
              else{

                  QString str="@zhuce@ExistUsers@";
                  LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection->peerAddress().toString());

                  clientConnection->write(str.toUtf8());

              }
            }
        }

    clientConnection->disconnectFromHost();
}
#endif // REGISTSERVER_H
