#ifndef NEWSSERVER_H
#define NEWSSERVER_H

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


class NewsServer: public QObject{
public:
    NewsServer();
    ~NewsServer();


    QTcpServer* tcpServer;
    QList<QTcpSocket* > clientConnection;

    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;

    void removeSocket();

};

NewsServer::NewsServer(){


    LogFile.setFileName("./NewsLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenFail")<<endl;



    //登录数据库

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,4567);
    connect(tcpServer,&QTcpServer::newConnection,this,&NewsServer::readMessage);


}

NewsServer::~NewsServer(){


}

void NewsServer::readMessage(){

    clientConnection.append(tcpServer->nextPendingConnection());


    LOG(QDateTime::currentDateTime().toString()+" "+"New Connection"+" "+clientConnection.last()->peerAddress().toString());
    connect(clientConnection.last(),&QTcpSocket::readyRead,this,&NewsServer::sendMessage);
    connect(clientConnection.last(),&QTcpSocket::disconnected,this,&NewsServer::removeSocket);

}

void NewsServer::sendMessage(){
    for(int i=0;i<clientConnection.length();i++){
        if(clientConnection[i]->bytesAvailable()>0){
            QString message=QString::fromUtf8(clientConnection[i]->readAll());
            qDebug()<<(message)<<endl;




            if(message.indexOf("@getnews@")>=0){
                QStringList temp=message.split("@");

                if(temp.length()>3){

                    QString str="@getnews@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;

                }
                QSqlQuery query;
                query.exec("SELECT * FROM news ORDER BY news.posttime DESC limit "+QString::number(7*temp[2].toInt())+","+QString::number(7));
                if(query.lastError().type()==QSqlError::NoError){
                    QString returnstr="@getnews@Succeed@";
                    while(query.next()){
                        QString likenum=QString::number(query.value(7).toInt());
                        returnstr+=query.value(0).toString()+"|||"+query.value(1).toString()+"|||"+query.value(2).toString()+"|||"+query.value(5).toDateTime().toString("MM-dd hh:mm")+"|||"+query.value(6).toString()+"|||"+likenum+"{|}";
                    }


                    LOG(QDateTime::currentDateTime().toString()+" "+"@getnews@Succeed@"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }

            }


            if(message.indexOf("@getcontent@")>=0){
                QStringList temp=message.split("@");

                if(temp.length()>3){

                    QString str="@getcontent@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;

                }
                QSqlQuery query;
                query.exec("SELECT * FROM news where newsid=\""+temp[2]+"\"");
                if(query.next()){
                    QString returnstr="@getcontent@Succeed@";

                    returnstr+=query.value(4).toString()+"@"+query.value(7).toString()+"@"+query.value(8).toString()+"@"+query.value(1).toString()+"@"+query.value(5).toDateTime().toString("MM-dd hh:mm")+"@"+query.value(3).toString()+"@"+"end";
                    LOG(QDateTime::currentDateTime().toString()+" "+"@getcontent@Succeed@"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }
                else{

                    QString str="@getcontent@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }

            }


            if(message.indexOf("@likenews@")>=0){
                QStringList temp=message.split("@");

                if(temp.length()>3){

                    QString str="@likenews@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;

                }
                QSqlQuery query;
                query.exec("update news set likecount=likecount+1 where newsid =" +temp[2]);
                if(query.lastError().type()==QSqlError::NoError){


                    QString str="@likenews@Succeed@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());

                }
                else{

                    QString str="@likenews@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }

            if(message.indexOf("@dislikenews@")>=0){
                QStringList temp=message.split("@");

                if(temp.length()>3){

                    QString str="@dislikenews@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;

                }
                QSqlQuery query;
                query.exec("update news set dislikecount=dislikecount+1 where newsid =" +temp[2]);
                if(query.lastError().type()==QSqlError::NoError){


                    QString str="@dislikenews@Succeed@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());

                }
                else{

                    QString str="@dislikenews@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }





            if(message.indexOf("@sendcomment@")>=0){
                QStringList temp11=message.split("comment@");
                QStringList temp=message.split("|||");
                QSqlQuery query;
                query.prepare("insert into newscomments(newsid,commentatorid,becommentatorid,message) values(?,?,?,?)");
                query.addBindValue(temp[1]);
                query.addBindValue(temp[2]);
                query.addBindValue(temp[3]);
                query.addBindValue(temp[4]);
                if(temp11.length()<=2)
                    query.exec();
                else{

                    QString str="@sendcomment@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                    LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }



                if(query.lastError().type()==QSqlError::NoError&&temp11.length()<=2){
                    LOG(QDateTime::currentDateTime().toString()+" "+"SendComment Succeed"+" "+clientConnection[i]->peerAddress().toString());

                    QSqlQuery query1;
                    query1.exec("update news set commentcount=commentcount+1 where newsid =" +temp[1]);


                    QSqlQuery query51;
                    query51.exec("select * from news where newsid="+temp[1]);

                    if(!query51.next()){


                        QString str="@sendcomment@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                        return;
                    }


                    if(temp[3]==""){
                        QSqlQuery query5;
                        query5.exec("select * from news where newsid="+temp[1]);
                        query5.next();
                        temp[3]=query5.value(1).toString();
                    }

                    if(temp[2]!=temp[3]){
                        QSqlQuery query2;
                        query2.prepare("insert into notices(sender,receiver,type,postid) values(?,?,?,?)");
                        query2.addBindValue(temp[2]);
                        query2.addBindValue(temp[3]);
                        query2.addBindValue("在资讯中评论了");
                        query2.addBindValue(temp[1]);
                        query2.exec();
                    }


                    QString str="@sendcomment@Succeed@";
                    clientConnection[i]->write(str.toUtf8());
                }
                else{

                    QString str="@sendcomment@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }


            if(message.indexOf("@deletecomment@")>=0){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QSqlQuery query;


                if(teststr.length()<=3){
                    QSqlQuery query2;
                    query2.exec("select * from newscomments where id="+temp[1]);
                    query2.next();
                    int dc=query2.value(1).toInt();

                    query.exec("DELETE FROM newscomments WHERE newscomments.id = "+temp[1]);
                    if(query.lastError().type()==QSqlError::NoError){
                        LOG(QDateTime::currentDateTime().toString()+" "+"delete comment succeed"+" "+clientConnection[i]->peerAddress().toString());




                        QSqlQuery query1;
                        query1.exec("update news set commentcount=commentcount-1 where newsid =" +QString::number(dc));

                        QString str="@deletecomment@Succeed@";
                        clientConnection[i]->write(str.toUtf8());

                    }
                    else{

                        QString str="@deletecomment@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                }

            }

            if(message.indexOf("@getcomments@")>=0){
                QStringList temp=message.split("@");
                //temp以后有用

                QString allstr="|||";

                QSqlQuery query;
                query.exec("select * from newscomments where newsid ="+temp[2]+" order by newscomments.commenttime ASC");
                if(query.lastError().type()==QSqlError::NoError&&temp.length()<=3){
                    while(query.next()){
                        QString tempstr;
                        Comment temp;
                        temp.ID=query.value(0).toInt();
                        temp.CommentatorID=query.value(2).toString();
                        temp.BeCommentatorID=query.value(3).toString();
                        temp.Message=query.value(4).toString();

                        QSqlQuery query1;
                        query1.exec("select * from userandpassword where username=\""+temp.CommentatorID+"\"");
                        query1.next();
                        temp.CommentatorName=query1.value(3).toString();

                        if(temp.BeCommentatorID!=""){
                            QSqlQuery query2;
                            query2.exec("select * from userandpassword where username=\""+temp.BeCommentatorID+"\"");
                            query2.next();
                            temp.BeCommentatorName=query2.value(3).toString();
                        }
                        else{
                            temp.BeCommentatorName="";
                        }
                        tempstr=QString::number(temp.ID)+"{|}"+temp.CommentatorID+"{|}"+temp.CommentatorName+"{|}"+temp.BeCommentatorName+"{|}"+temp.Message;
                        allstr=allstr+tempstr+"|||";
                    }


                    QString str="@getcomments@Succeed@"+allstr;
                    LOG(QDateTime::currentDateTime().toString()+" "+"@getcomments@Succeed@"+" "+clientConnection[i]->peerAddress().toString());




                    clientConnection[i]->write(str.toUtf8());
                }
                else{

                    QString str="@getcomments@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }


            }



        }
    }


}


void NewsServer::removeSocket(){
    for(int i=0;i<clientConnection.length();i++)
        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState){
            clientConnection.removeAt(i);
        }

}


#endif // NEWSSERVER_H
