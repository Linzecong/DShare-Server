#ifndef REPORTSERVER_H
#define REPORTSERVER_H
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

class ReportServer: public QObject{
public:
    ReportServer();
    ~ReportServer();

    QTcpServer* tcpServer;
    QList<QTcpSocket* > clientConnection;

    QList<QStringList> clientDietList;
    QList<QStringList> clientExerciseList;

    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;

    void removeSocket();

};

ReportServer::ReportServer(){


    LogFile.setFileName("./ReportLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenFail")<<endl;

    //登录数据库

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,12345);
    connect(tcpServer,&QTcpServer::newConnection,this,&ReportServer::readMessage);


}

ReportServer::~ReportServer(){


}

void ReportServer::readMessage(){

    clientConnection.append(tcpServer->nextPendingConnection());
    QStringList temp;
    clientDietList.append(temp);
    clientExerciseList.append(temp);

    LOG(QDateTime::currentDateTime().toString()+" "+"New Connection "+" "+clientConnection.last()->peerAddress().toString());
    connect(clientConnection.last(),&QTcpSocket::readyRead,this,&ReportServer::sendMessage);
    connect(clientConnection.last(),&QTcpSocket::disconnected,this,&ReportServer::removeSocket);

}

void ReportServer::sendMessage(){
    for(int i=0;i<clientConnection.length();i++){
        if(clientConnection[i]->bytesAvailable()>0){
            QString message=QString::fromUtf8(clientConnection[i]->readAll());


            if(message.indexOf("@getalldiets@")>=0){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){

                    QString str="@getalldiets@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{
                    QStringList temp=message.split("@");
                    QSqlQuery query;
                    query.exec("select * from diets where userid=\""+temp[2]+"\"");

                    if(query.lastError().type()==QSqlError::NoError){
                        while(query.next()){
                            QString str=query.value(1).toDateTime().toString("yyyy-MM-dd")+"|||"+query.value(2).toString()+"|||"+query.value(3).toString()+"|||"+query.value(4).toString()+"|||"+query.value(5).toString()+"|||"+query.value(6).toString()+"|||"+query.value(7).toString();
                            clientDietList[i].append(str);
                        }


                        QString str="@getalldiets@FirstSucceed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@getalldiets@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                }
            }

            if(message.indexOf("@getuniquediets@")>=0){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){

                    QString str="@getalldiets@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{
                    if(clientDietList[i].empty()){

                        QString str="@getuniquediets@NoMore@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@getuniquediets@Succeed@|||"+clientDietList[i].back();
                        clientDietList[i].pop_back();
                        clientConnection[i]->write(str.toUtf8());
                    }
                }
            }

            if(message.indexOf("@getallexercises@")>=0){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){

                    QString str="@getallexercises@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{
                    QStringList temp=message.split("@");
                    QSqlQuery query;
                    query.exec("select * from exercises where userid=\""+temp[2]+"\"");

                    if(query.lastError().type()==QSqlError::NoError){
                        while(query.next()){
                            QString str=query.value(1).toDateTime().toString("yyyy-MM-dd")+"|||"+query.value(2).toString()+"|||"+query.value(4).toString();
                            clientExerciseList[i].append(str);
                        }


                        QString str="@getallexercises@FirstSucceed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@getallexercises@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                }
            }

            if(message.indexOf("@getuniqueexercises@")>=0){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){

                    QString str="@getallexercises@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{
                    if(clientExerciseList[i].empty()){

                        QString str="@getuniqueexercises@NoMore@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString str="@getuniqueexercises@Succeed@|||"+clientExerciseList[i].back();
                        clientExerciseList[i].pop_back();
                        clientConnection[i]->write(str.toUtf8());
                    }
                }
            }


        }
    }


}
void ReportServer::removeSocket(){
    for(int i=0;i<clientConnection.length();i++)
        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState){

            clientConnection.removeAt(i);
            clientDietList.removeAt(i);
            clientExerciseList.removeAt(i);
        }

}
#endif // REPORTSERVER_H
