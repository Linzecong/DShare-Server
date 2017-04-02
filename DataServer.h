#ifndef DATASERVER_H
#define DATASERVER_H
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

class DataServer: public QObject{
public:
    DataServer();
    ~DataServer();


    QTcpServer* tcpServer;
    QList<QTcpSocket* > clientConnection;

    void readMessage();
    void sendMessage();
    void removeSocket();

private:


    QFile LogFile;
    QTextStream* LogTextStream;



};

DataServer::DataServer(){


    LogFile.setFileName("./DataLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenFail")<<endl;



    //登录数据库


    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,8889);
    connect(tcpServer,&QTcpServer::newConnection,this,&DataServer::readMessage);



}

DataServer::~DataServer(){


}

void DataServer::readMessage(){

    clientConnection.append(tcpServer->nextPendingConnection());
    LOG(QDateTime::currentDateTime().toString()+" "+"New Connection"+" "+clientConnection.last()->peerAddress().toString());
    connect(clientConnection.last(),&QTcpSocket::readyRead,this,&DataServer::sendMessage);
    connect(clientConnection.last(),&QTcpSocket::disconnected,this,&DataServer::removeSocket);

}

void DataServer::sendMessage(){
    for(int i=0;i<clientConnection.length();i++){
        if(clientConnection[i]->bytesAvailable()>0){
            QString message=QString::fromUtf8(clientConnection[i]->readAll());
            qDebug()<<message<<endl;


            if(message.indexOf("@getname@")!=-1){
                QStringList teststr=message.split("@");

                QStringList temp=message.split("|||");
                if(temp[1].length()<6||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getname@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getname@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from userandpassword where username=\""+temp[1]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"getname@dberror"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getname@DBError@").toUtf8());
                    }
                    else{
                        LOG(QDateTime::currentDateTime().toString()+" "+"getname@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getname@Succeed@"+query.value(3).toString()).toUtf8());
                    }


                }
            }

            if(message.indexOf("@gethead@")!=-1){
                QStringList teststr=message.split("@");

                QStringList temp=message.split("|||");
                if(temp[1].length()<6||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"gethead@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@gethead@DBError@").toUtf8());
                }
                else{
                    QDir *dir=new QDir("C:\\inetpub\\wwwroot\\userhead");
                    QStringList filter;
                    filter<<"*.jpg";
                    dir->setNameFilters(filter);
                    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));

                    for(int i=0;i<fileInfo->count();i++){
                        if(fileInfo->at(i).fileName().indexOf(temp[1])>=0){
                            temp[1]= "http://119.29.15.43/userhead/"+fileInfo->at(i).fileName();
                            break;
                        }
                    }

                    LOG(QDateTime::currentDateTime().toString()+" "+"gethead@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@gethead@Succeed@"+temp[1]).toUtf8());
                }
            }





            if(message.indexOf("@changename@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<6||temp[2].length()>8||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"changename@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@changename@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from userandpassword where username=\""+temp[1]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"changename@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@changename@DBError@").toUtf8());
                    }
                    else{
                        QSqlQuery query2;
                        query2.exec("UPDATE userandpassword SET name = \""+temp[2]+"\"  WHERE userandpassword.username =\""+temp[1]+"\"");
                        if(query2.lastError().type()==QSqlError::NoError){
                            LOG(QDateTime::currentDateTime().toString()+" "+"changename@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@changename@Succeed@").toUtf8());
                        }
                        else{
                            LOG(QDateTime::currentDateTime().toString()+" "+"changename@DBError"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@changename@DBError@").toUtf8());
                        }
                    }


                }
            }

            if(message.indexOf("@changepassword@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<6||temp[2].length()>20||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"changepassword@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@changepassword@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from userandpassword where username=\""+temp[1]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"changepassword@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@changepassword@DBError@").toUtf8());
                    }
                    else{
                        QSqlQuery query2;
                        query2.exec("UPDATE userandpassword SET password = \""+temp[2]+"\"  WHERE userandpassword.username =\""+temp[1]+"\"");
                        if(query2.lastError().type()==QSqlError::NoError){
                            LOG(QDateTime::currentDateTime().toString()+" "+"changepassword@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@changepassword@Succeed@").toUtf8());
                        }
                        else{
                            LOG(QDateTime::currentDateTime().toString()+" "+"changepassword@DBError"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@changepassword@DBError@").toUtf8());
                        }
                    }


                }
            }




            if(message.indexOf("@addfollowing@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<5||temp[2].length()<5||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"addfollowing@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@addfollowing@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.prepare("insert into relationship (mainuser,userfriend) values(?,?)");
                    query.addBindValue(temp[1]);
                    query.addBindValue(temp[2]);
                    query.exec();
                    if(query.lastError().type()==QSqlError::NoError){
                        LOG(QDateTime::currentDateTime().toString()+" "+"addfollowing@Succeed"+" "+clientConnection[i]->peerAddress().toString());


                        if(temp[1]!=temp[2]){
                            QSqlQuery query2;
                            query2.prepare("insert into notices(sender,receiver,type,postid) values(?,?,?,?)");
                            query2.addBindValue(temp[1]);
                            query2.addBindValue(temp[2]);
                            query2.addBindValue("关注了");
                            query2.addBindValue(0);
                            query2.exec();
                        }


                        clientConnection[i]->write(QString("@addfollowing@Succeed@").toUtf8());
                    }
                    else{
                        LOG(QDateTime::currentDateTime().toString()+" "+"addfollowing@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@addfollowing@DBError@").toUtf8());
                    }
                }
            }




            if(message.indexOf("@deletefollowing@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<5||temp[2].length()<5||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"deletefollowing@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@deletefollowing@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.prepare("DELETE FROM relationship WHERE relationship.mainuser = ? AND relationship.userfriend = ?");
                    query.addBindValue(temp[1]);
                    query.addBindValue(temp[2]);
                    query.exec();
                    if(query.lastError().type()==QSqlError::NoError){
                        LOG(QDateTime::currentDateTime().toString()+" "+"deletefollowing@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@deletefollowing@Succeed@").toUtf8());
                    }
                    else{
                        LOG(QDateTime::currentDateTime().toString()+" "+"deletefollowing@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@deletefollowing@DBError@").toUtf8());
                    }
                }
            }


            if(message.indexOf("@getfollowings@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<5||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfollowings@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getfollowings@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from relationship where mainuser=\""+temp[1]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"getfollowings@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getfollowings@DBError@").toUtf8());
                    }
                    else{

                        QString str="@getfollowings@Succeed@";
                        do{
                            QSqlQuery query1;
                            query1.exec("select * from userandpassword where username=\""+query.value(1).toString()+"\"");
                            query1.next();
                            str=str+query.value(1).toString()+"@"+query1.value(3).toString()+"@";
                        }while(query.next());
                        LOG(QDateTime::currentDateTime().toString()+" "+"getfollowings@success"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }


                }
            }


            if(message.indexOf("@getfollowers@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<5||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfollowers@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getfollowers@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from relationship where userfriend=\""+temp[1]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"getfollowers@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getfollowers@DBError@").toUtf8());
                    }
                    else{

                        QString str="@getfollowers@Succeed@";
                        do{
                            QSqlQuery query1;
                            query1.exec("select * from userandpassword where username=\""+query.value(0).toString()+"\"");
                            query1.next();
                            str=str+query.value(0).toString()+"@"+query1.value(3).toString()+"@";
                        }while(query.next());
                        LOG(QDateTime::currentDateTime().toString()+" "+"getfollowers@succeed"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }


                }
            }

            if(message.indexOf("@searchuser@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QString sstr="%";
                for(int i=0;i<temp[1].length();i++)
                    sstr=sstr+temp[1][i]+"%";
                QSqlQuery query;
                query.exec("select * from userandpassword where username like \""+sstr+"\" OR name like \""+ sstr+"\"");
                if(!query.next()||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"searchuser@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@searchuser@DBError@").toUtf8());
                }
                else{
                    QString str="@searchuser@Succeed@";
                    do{
                        str=str+query.value(1).toString()+"@"+query.value(3).toString()+"@";
                    }while(query.next());
                    LOG(QDateTime::currentDateTime().toString()+" "+"searchuser@succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }

            if(message.indexOf("@searchfood@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QString sstr="%";

                sstr=sstr+temp[1]+"%";

                QSqlQuery query;
                query.exec("select * from fooddes where foodname like \""+sstr+"\"");
                if(!query.next()||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"searchfood@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@searchfood@DBError@").toUtf8());
                }
                else{
                    QString str="@searchfood@Succeed@";
                    do{
                        str=str+query.value(1).toString()+"@"+query.value(0).toString()+"@"+query.value(3).toString()+"@";
                    }while(query.next());
                    LOG(QDateTime::currentDateTime().toString()+" "+"searchfood@succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }

            if(message.indexOf("@searchfunc@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");

                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"searchfunc@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@searchfunc@DBError@").toUtf8());
                    break;
                }

                QStringList foodlist;

                bool isfull=0;

                QString sstr="%";
                sstr=sstr+temp[1]+"%";

                QSqlQuery query;
                query.exec("select * from fooddes where fooddes like \""+sstr+"\" limit 0,5");


                if(query.size()==5)
                    isfull=1;

                if(query.next()){
                    do{
                        foodlist.append(query.value(0).toString());
                    }while(query.next());
                }


                QSqlQuery query2;
                query2.exec("select * from foodrelation where reason like \""+sstr+"\" limit 0,7");

                if(query2.size()==7)
                    isfull=1;

                if(query2.next()){
                    do{
                        foodlist.append(query2.value(0).toString());
                        foodlist.append(query2.value(1).toString());
                    }while(query2.next());
                }

               for(int i=0;i<foodlist.length();i++)
                   for(int j=i+1;j<foodlist.length();j++)
                       if(foodlist[i]==foodlist[j])
                           foodlist.removeAt(j);

               if(foodlist.length()==0){
                   LOG(QDateTime::currentDateTime().toString()+" "+"searchfunc@no result"+" "+clientConnection[i]->peerAddress().toString());

                   clientConnection[i]->write(QString("@searchfunc@DBError@").toUtf8());
                   break;
               }


               QString str="@searchfunc@Succeed@";
               if(isfull)
                     str="full@searchfunc@Succeed@";
                str=str+foodlist.join("、")+"、";
                LOG(QDateTime::currentDateTime().toString()+" "+"searchfunc@Succeed"+" "+clientConnection[i]->peerAddress().toString());


                clientConnection[i]->write(str.toUtf8());
            }



            if(message.indexOf("@getfoodmsg@")!=-1){
                QStringList teststr=message.split("@");

                QStringList temp=teststr[2].split("、");

                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfoodmsg@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getfoodmsg@DBError@").toUtf8());
                }

                QString returnstr="@getfoodmsg@Succeed@";

                for(int i=0;i<temp.length()-1;i++){
                    QSqlQuery query;
                    query.exec("select * from fooddes where foodname = \""+temp[i]+"\"");
                    if(query.next()){
                        returnstr=returnstr+query.value(1).toString()+"@"+query.value(0).toString()+"@"+query.value(3).toString()+"@";

                    }
                }
                LOG(QDateTime::currentDateTime().toString()+" "+"getfoodmsg@succeed"+" "+clientConnection[i]->peerAddress().toString());

                clientConnection[i]->write(returnstr.toUtf8());



            }



            if(message.indexOf("@checkin@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("@");
                if(temp[2].length()<6||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"checkin@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@checkin@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from userandpassword where username=\""+temp[2]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"checkin@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@checkin@DBError@").toUtf8());
                    }
                    else{
                        int day=query.value(4).toInt();
                        QDate date=query.value(5).toDate();
                        if(date.daysTo(QDate::currentDate())==1){
                            QSqlQuery query2;
                            query2.exec("UPDATE userandpassword SET checkinday = "+QString::number(day+1)+",lastdate=\""+QDate::currentDate().toString("yyyy-MM-dd")+ "\" WHERE userandpassword.username =\""+temp[2]+"\"");
                            if(query2.lastError().type()==QSqlError::NoError){
                                LOG(QDateTime::currentDateTime().toString()+" "+"checkin@succeed"+" "+clientConnection[i]->peerAddress().toString());

                                clientConnection[i]->write(QString("@checkin@Succeed@").toUtf8());
                            }
                            else{
                                LOG(QDateTime::currentDateTime().toString()+" "+"checkin@DBError"+" "+clientConnection[i]->peerAddress().toString());

                                clientConnection[i]->write(QString("@checkin@DBError@").toUtf8());
                            }
                        }
                        if(date.daysTo(QDate::currentDate())==0){

                            LOG(QDateTime::currentDateTime().toString()+" "+"checkin@DBError"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@checkin@DBError@").toUtf8());

                        }

                        if(date.daysTo(QDate::currentDate())>1){
                            QSqlQuery query2;
                            query2.exec("UPDATE userandpassword SET checkinday = 1,lastdate=\""+QDate::currentDate().toString("yyyy-MM-dd")+ "\" WHERE userandpassword.username =\""+temp[2]+"\"");
                            if(query2.lastError().type()==QSqlError::NoError){
                                LOG(QDateTime::currentDateTime().toString()+" "+"checkin@succeed"+" "+clientConnection[i]->peerAddress().toString());

                                clientConnection[i]->write(QString("@checkin@Succeed@").toUtf8());
                            }
                            else{
                                LOG(QDateTime::currentDateTime().toString()+" "+"checkin@DBError"+" "+clientConnection[i]->peerAddress().toString());

                                clientConnection[i]->write(QString("@checkin@DBError@").toUtf8());
                            }
                        }
                    }


                }
            }



            if(message.indexOf("@getcheckinday@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("@");
                if(temp[2].length()<6||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getcheckinday@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getcheckinday@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from userandpassword where username=\""+temp[2]+"\"");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"getcheckinday@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getcheckinday@DBError@").toUtf8());
                    }
                    else{
                        int day=query.value(4).toInt();
                        QDate date=query.value(5).toDate();
                        if(date.daysTo(QDate::currentDate())==1||date.daysTo(QDate::currentDate())==0){
                            LOG(QDateTime::currentDateTime().toString()+" "+"getcheckinday@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@getcheckinday@Succeed@"+QString::number(day)).toUtf8());
                        }
                        else{
                            LOG(QDateTime::currentDateTime().toString()+" "+"getcheckinday@Succeed"+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(QString("@getcheckinday@Succeed@0").toUtf8());
                        }
                    }
                }
            }


            if(message.indexOf("@getnotices@")!=-1){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                if(temp[1].length()<5||teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getnotices@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getnotices@DBError@").toUtf8());
                }
                else{
                    QSqlQuery query;
                    query.exec("select * from notices where receiver=\""+temp[1]+"\" order by notices.time DESC");
                    if(!query.next()){
                        LOG(QDateTime::currentDateTime().toString()+" "+"getnotices@DBError"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(QString("@getnotices@DBError@").toUtf8());
                    }
                    else{

                        QString str="@getnotices@Succeed@|||";



                        do{
                            QSqlQuery query1;
                            query1.exec("select * from userandpassword where username=\""+query.value(0).toString()+"\"");
                            query1.next();
                            str=str+query1.value(3).toString()+"{|}"+query.value(2).toString()+"{|}"+query.value(4).toDateTime().toString("hh:mm MM-dd")+"{|}"+QString::number(query.value(3).toInt())+"{|}"+QString::number(query.value(5).toInt())+"|||";
                        }while(query.next());

                        QSqlQuery query3;
                        query3.exec("UPDATE notices SET isread = 1  WHERE notices.receiver =\""+temp[1]+"\"");


                        LOG(QDateTime::currentDateTime().toString()+" "+"getnotices@succed"+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }


                }
            }

            if(message.indexOf("@getfoodrelation@")!=-1){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfoodrelation@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getfoodrelation@DBError@").toUtf8());
                }
                else{
                    QString returnstr="@getfoodrelation@Succeed@";

                    QStringList foodlist=teststr[2].split("、");
                    for(int i=0;i<foodlist.length()-1;i++)
                        for(int j=i+1;j<foodlist.length()-1;j++){
                            QSqlQuery query;
                            query.exec("select * from foodrelation where firstfood=\""+foodlist[i]+"\" and secondfood=\""+foodlist[j]+"\"");
                            if(query.next()){
                                returnstr+=query.value(2).toString()+"|||"+query.value(3).toString()+"{|}";
                            }
                            else{
                                returnstr+="NoRelation|||NoRelation{|}";
                            }
                        }
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfoodrelation@succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }
            }


            if(message.indexOf("@getfooddetail@")!=-1){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getfooddetail@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getfooddetail@DBError@").toUtf8());
                }
                else{
                    QString returnstr="@getfooddetail@Succeed@";

                    QSqlQuery query;
                    query.exec("select * from fooddes where foodname=\""+teststr[2]+"\"");
                    if(query.next()){
                        returnstr+=query.value(2).toString();
                    }
                    else{
                        returnstr+="暂无数据";
                    }

                    LOG(QDateTime::currentDateTime().toString()+" "+"getfooddetail@succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }
            }


            if(message.indexOf("@getgoodrelation@")!=-1){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getgoodrelation@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getgoodrelation@DBError@").toUtf8());
                }
                else{
                    QString returnstr="@getgoodrelation@Succeed@";

                    QSqlQuery query;
                    query.exec("select * from foodrelation where firstfood=\""+teststr[2]+"\" and type=\"1\"");


                    while(query.next()){
                        returnstr+=query.value(1).toString()+"|||"+query.value(3).toString()+"{|}";
                    }

                    LOG(QDateTime::currentDateTime().toString()+" "+"getgoodrelation@succed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }
            }

            if(message.indexOf("@getbadrelation@")!=-1){
                QStringList teststr=message.split("@");
                if(teststr.length()>3){
                    LOG(QDateTime::currentDateTime().toString()+" "+"getbadrelation@MSGError"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(QString("@getbadrelation@DBError@").toUtf8());
                }
                else{
                    QString returnstr="@getbadrelation@Succeed@";

                    QSqlQuery query;
                    query.exec("select * from foodrelation where firstfood=\""+teststr[2]+"\" and type=\"0\"");


                    while(query.next()){
                        returnstr+=query.value(1).toString()+"|||"+query.value(3).toString()+"{|}";
                    }

                    LOG(QDateTime::currentDateTime().toString()+" "+"getbadrelation@succeed"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(returnstr.toUtf8());

                }
            }


        }
    }
}

void DataServer::removeSocket(){
    for(int i=0;i<clientConnection.length();i++)
        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState)
            clientConnection.removeAt(i);

}



#endif // DATASERVER_H
