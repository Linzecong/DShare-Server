#ifndef RECORDSERVER_H
#define RECORDSERVER_H
#define LOG(x) qDebug()<<(x)<<endl;(*LogTextStream)<<(x)<<endl;

#include <QtNetwork/QtNetwork>
#include <QList>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlError>

class RecordServer: public QObject{
public:
    RecordServer();
    ~RecordServer();


    QTcpServer* tcpServer;
    QList<QTcpSocket* > clientConnection;

    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;

    void removeSocket();

};

RecordServer::RecordServer(){


    LogFile.setFileName("./RecordLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenFail")<<endl;



    //登录数据库

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,6789);
    connect(tcpServer,&QTcpServer::newConnection,this,&RecordServer::readMessage);


}

RecordServer::~RecordServer(){


}

void RecordServer::readMessage(){

    clientConnection.append(tcpServer->nextPendingConnection());
    LOG(QDateTime::currentDateTime().toString()+" "+"New Connection"+" "+clientConnection.last()->peerAddress().toString());
    connect(clientConnection.last(),&QTcpSocket::readyRead,this,&RecordServer::sendMessage);
    connect(clientConnection.last(),&QTcpSocket::disconnected,this,&RecordServer::removeSocket);

}

void RecordServer::sendMessage(){
    for(int i=0;i<clientConnection.length();i++){
        if(clientConnection[i]->bytesAvailable()>0){
            QString message=QString::fromUtf8(clientConnection[i]->readAll());
            qDebug()<<(message)<<endl;


            if(message.indexOf("@getdiets@")>=0){
                QString returnstr="|||";
                QStringList temp=message.split("@");
                QSqlQuery query;
                query.exec("select * from diets where userid=\""+temp[2]+"\" "+"and date=\""+temp[3]+"\"");
                if(query.lastError().type()==QSqlError::NoError&&query.next()&&temp.length()==4){

                    returnstr=returnstr+query.value(2).toString()+"|||"+query.value(3).toString()+"|||"+query.value(4).toString()+"|||"+query.value(5).toString()+"|||"+query.value(6).toString()+"|||"+query.value(7).toString();
                }

                else{

                    QString str="@getdiets@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }

                QString str="@getdiets@Succeed@"+returnstr;
                LOG(QDateTime::currentDateTime().toString()+" "+"@getdiets@Succeed@"+" "+clientConnection[i]->peerAddress().toString());

                clientConnection[i]->write(str.toUtf8());
            }




            if(message.indexOf("@getexercises@")>=0){
                QString returnstr="|||";
                QStringList temp=message.split("@");
                QSqlQuery query;
                query.exec("select * from exercises where userid=\""+temp[2]+"\" "+"and date=\""+temp[3]+"\"");
                if(query.lastError().type()==QSqlError::NoError&&temp.length()==4){
                    while(query.next()){
                        returnstr=returnstr+query.value(2).toString()+"{|}"+query.value(3).toTime().toString("hh:mm:ss")+"{|}"+QString::number(query.value(4).toInt())+"|||";
                    }
                }

                else{

                    QString str="@getexercises@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }
                LOG("Succeed");
                QString str="@getexercises@Succeed@"+returnstr;
                LOG(QDateTime::currentDateTime().toString()+" "+"@getexercises@Succeed@"+" "+clientConnection[i]->peerAddress().toString());

                clientConnection[i]->write(str.toUtf8());
            }


            if(message.indexOf("@uploaddiet@")>=0){
                QStringList temp=message.split("@");
                QSqlQuery query;
                query.exec("select * from diets where userid=\""+temp[2]+"\" "+"and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                if(query.lastError().type()==QSqlError::NoError&&temp.length()==5){
                    QSqlQuery query1;
                    if(query.next()){
                        switch(temp[4].toInt()){
                        case 0:
                            query1.exec("update diets set breakfast=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;
                        case 1:
                            query1.exec("update diets set lunch=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;
                        case 2:
                            query1.exec("update diets set dinner=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;
                        case 3:
                            query1.exec("update diets set snack=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;
                        case 4:
                            query1.exec("update diets set dessert=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;
                        case 5:
                            query1.exec("update diets set others=\""+temp[3]+"\" where userid=\""+temp[2]+"\" and date=\""+QDateTime::currentDateTime().toString("yyyy-MM-dd")+"\"");
                            break;

                        }
                    }
                    else{
                        switch(temp[4].toInt()){
                        case 0:
                            query1.prepare("insert into diets(userid,date,breakfast) values(?,?,?)");
                            break;
                        case 1:
                            query1.prepare("insert into diets(userid,date,lunch) values(?,?,?)");
                            break;
                        case 2:
                            query1.prepare("insert into diets(userid,date,dinner) values(?,?,?)");
                            break;
                        case 3:
                            query1.prepare("insert into diets(userid,date,snack) values(?,?,?)");
                            break;
                        case 4:
                            query1.prepare("insert into diets(userid,date,dessert) values(?,?,?)");
                            break;
                        case 5:
                            query1.prepare("insert into diets(userid,date,others) values(?,?,?)");
                            break;
                        }
                        query1.addBindValue(temp[2]);
                        query1.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
                        query1.addBindValue(temp[3]);
                        query1.exec();
                    }


                    if(query1.lastError().type()==QSqlError::NoError){

                        QString str="@uploaddiet@Succeed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@uploaddiet@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                        return;
                    }

                }

                else{

                    QString str="@uploaddiet@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }

            }





            if(message.indexOf("@uploadexercise@")>=0){
                QStringList temp=message.split("@");
                QSqlQuery query;
                query.prepare("insert into exercises(userid,date,type,begintime,lasttime) values(?,?,?,\""+temp[4]+"\",?)");
                query.addBindValue(temp[2]);
                query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
                query.addBindValue(temp[3]);
                query.addBindValue(temp[5].toInt());
                query.exec();
                    if(query.lastError().type()==QSqlError::NoError&&temp.length()==6){

                        QString str="@uploadexercise@Succeed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@uploadexercise@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                        return;
                    }

                }




        }
    }


}
void RecordServer::removeSocket(){
    for(int i=0;i<clientConnection.length();i++)
        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState)
            clientConnection.removeAt(i);
}
#endif // SENDPOSTSYSTEM_H

