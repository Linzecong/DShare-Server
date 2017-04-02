#ifndef SENDPOSTSYSTEM_H
#define SENDPOSTSYSTEM_H
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

struct Post{
    int ID;
    QString PublisherID;
    QString Publisher;
    QString PostTime;
    QString Message;
    QString ImageURL;
    QString HeadURL;
    QString LikersString;
    bool HasImage;
    int CommentCount;
};

struct Comment{

    int ID;
    QString Message;

    QString CommentatorName;
    QString BeCommentatorName;

    QString CommentatorID;
    QString BeCommentatorID;

};

class PostServer: public QObject{
public:
    PostServer();
    ~PostServer();


    QTcpServer* tcpServer;
    QList<QTcpSocket* > clientConnection;
    QList<QList<Post> > clientPostList;


    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;

    void removeSocket();

};

PostServer::PostServer(){


    LogFile.setFileName("./SendPostLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QDateTime::currentDateTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QDateTime::currentDateTime().toString()+"  "+"LogOpenFail")<<endl;



    //登录数据库

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,8520);
    connect(tcpServer,&QTcpServer::newConnection,this,&PostServer::readMessage);


}

PostServer::~PostServer(){


}

void PostServer::readMessage(){

    clientConnection.append(tcpServer->nextPendingConnection());
    QList<Post> temp;
    clientPostList.append(temp);

    LOG(QDateTime::currentDateTime().toString()+" New Connection "+clientConnection.last()->peerAddress().toString());

    connect(clientConnection.last(),&QTcpSocket::readyRead,this,&PostServer::sendMessage);
    connect(clientConnection.last(),&QTcpSocket::disconnected,this,&PostServer::removeSocket);

}

void PostServer::sendMessage(){
    for(int i=0;i<clientConnection.length();i++){
        if(clientConnection[i]->bytesAvailable()>0){
            QString message=QString::fromUtf8(clientConnection[i]->readAll());
            qDebug()<<(message)<<endl;

            if(message.indexOf("@sendpost@")>=0){
                QStringList temp=message.split("|||");
                QSqlQuery query;
                query.prepare("insert into mainposts(userid,message,hasimage,photo) values(?,?,?,?)");
                query.addBindValue(temp[1]);
                query.addBindValue(temp[2]);
                query.addBindValue(temp[3]);
                query.addBindValue(temp[4]);
                query.exec();
                if(query.lastError().type()==QSqlError::NoError){


                    QString str="@sendpost@Succeed@";

                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{


                    QString str="@sendpost@DBError@";

                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                    LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }

            if(message.indexOf("@sendcomment@")>=0){
                QStringList temp11=message.split("comment@");
                QStringList temp=message.split("|||");
                QSqlQuery query;
                query.prepare("insert into comments(postid,commentatorid,becommentatorid,message) values(?,?,?,?)");
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

                    QSqlQuery query1;
                    query1.exec("update mainposts set commentcount=commentcount+1 where id =" +temp[1]);


                    QSqlQuery query51;
                    query51.exec("select * from mainposts where id="+temp[1]);

                    if(!query51.next()){

                        QString str="@sendcomment@DBError@";

                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                        LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                        return;
                    }


                    if(temp[3]==""){
                        QSqlQuery query5;
                        query5.exec("select * from mainposts where id="+temp[1]);
                        query5.next();
                        temp[3]=query5.value(1).toString();
                    }

                    if(temp[2]!=temp[3]){
                        QSqlQuery query2;
                        query2.prepare("insert into notices(sender,receiver,type,postid) values(?,?,?,?)");
                        query2.addBindValue(temp[2]);
                        query2.addBindValue(temp[3]);
                        query2.addBindValue("评论了");
                        query2.addBindValue(temp[1]);
                        query2.exec();
                    }


                    QString str="@sendcomment@Succeed@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{

                    QString str="@sendcomment@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                    LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }


            if(message.indexOf("@getfriendsposts@")>=0){
                clientPostList[i].clear();
                QStringList temp=message.split("@");
                //temp以后有用
                QSqlQuery query;
                query.exec("select * from mainposts where userid in(select userfriend from relationship where mainuser=\""+temp[2]+"\") order by mainposts.posttime DESC");
                if(query.lastError().type()==QSqlError::NoError&&temp.length()<=3){
                    while(query.next()){
                        Post temp;
                        temp.ID=query.value(0).toInt();
                        temp.PublisherID=query.value(1).toString();
                        temp.Publisher=query.value(1).toString();

                        QDir dir;
                        dir.setPath("C:\\inetpub\\wwwroot\\userhead");
                        QStringList filter;
                        filter<<"*.jpg";
                        dir.setNameFilters(filter);
                        QList<QFileInfo> fileInfo;
                        fileInfo.append(dir.entryInfoList(filter));

                        for(int i=0;i<fileInfo.count();i++){
                            if(fileInfo.at(i).fileName().indexOf(temp.Publisher+"$$")>=0){
                                temp.HeadURL= "http://119.29.15.43/userhead/"+fileInfo.at(i).fileName();
                                break;
                            }
                        }

                        temp.CommentCount=query.value(8).toInt();
                        QSqlQuery query1;
                        query1.exec("select * from userandpassword where username=\""+temp.Publisher+"\"");
                        query1.next();
                        temp.Publisher=query1.value(3).toString();

                        temp.Message=query.value(2).toString();
                        temp.HasImage=query.value(3).toBool();
                        temp.LikersString=query.value(4).toString();

                        if(!temp.LikersString.isEmpty()){
                            QStringList likername=temp.LikersString.split("|");
                            temp.LikersString="❤ ";
                            for(int i=1;i<likername.length();i++){
                                QSqlQuery query3;
                                query3.exec("select * from userandpassword where username=\""+likername[i]+"\"");
                                query3.next();
                                temp.LikersString=temp.LikersString+query3.value(3).toString()+",";
                            }
                            temp.LikersString=temp.LikersString.left(temp.LikersString.lastIndexOf(","));
                        }

                        temp.ImageURL=query.value(5).toString();

                        int day=query.value(7).toDateTime().daysTo(QDateTime::currentDateTime());
                        if(day==0)
                        temp.PostTime="今天"+query.value(7).toDateTime().toString("hh:mm");
                        if(day==1)
                        temp.PostTime="昨天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day==2)
                        temp.PostTime="前天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day>2)
                        temp.PostTime=QString::number(day)+"天前 "+query.value(7).toDateTime().toString("hh:mm");

                        if(day>30)
                        temp.PostTime=query.value(7).toDateTime().toString("MM-dd hh:mm");

                        if(day>300)
                        temp.PostTime=query.value(7).toDateTime().toString("yyyy-MM-dd hh:mm");


                        clientPostList[i].append(temp);
                    }
                }
                else{

                    QString str="@getfriendsposts@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }

                QString str="@getfriendsposts@Succeed@";
                LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                clientConnection[i]->write(str.toUtf8());
            }


            if(message.indexOf("@getuserposts@")>=0){
                clientPostList[i].clear();
                QStringList temp=message.split("@");
                //temp以后有用
                QSqlQuery query;
                query.exec("select * from mainposts where userid=\""+temp[2]+"\" order by mainposts.posttime DESC");
                if(query.lastError().type()==QSqlError::NoError&&temp.length()<=3){
                    while(query.next()){
                        Post temp;
                        temp.ID=query.value(0).toInt();
                        temp.PublisherID=query.value(1).toString();
                        temp.Publisher=query.value(1).toString();
                        //temp.HeadURL= "http://119.29.15.43/userhead/"+temp.Publisher+".jpg";

                        QDir dir;
                        dir.setPath("C:\\inetpub\\wwwroot\\userhead");
                        QStringList filter;
                        filter<<"*.jpg";
                        dir.setNameFilters(filter);
                        QList<QFileInfo> fileInfo;
                        fileInfo.append(dir.entryInfoList(filter));

                        for(int i=0;i<fileInfo.count();i++){
                            if(fileInfo.at(i).fileName().indexOf(temp.Publisher+"$$")>=0)
                                temp.HeadURL= "http://119.29.15.43/userhead/"+fileInfo.at(i).fileName();
                        }


                        temp.CommentCount=query.value(8).toInt();

                        QSqlQuery query1;
                        query1.exec("select * from userandpassword where username=\""+temp.Publisher+"\"");
                        query1.next();
                        temp.Publisher=query1.value(3).toString();

                        temp.Message=query.value(2).toString();
                        temp.HasImage=query.value(3).toBool();
                        temp.LikersString=query.value(4).toString();

                        if(!temp.LikersString.isEmpty()){
                            QStringList likername=temp.LikersString.split("|");
                            temp.LikersString="❤ ";
                            for(int i=1;i<likername.length();i++){
                                QSqlQuery query3;
                                query3.exec("select * from userandpassword where username=\""+likername[i]+"\"");
                                query3.next();
                                temp.LikersString=temp.LikersString+query3.value(3).toString()+",";
                            }
                            temp.LikersString=temp.LikersString.left(temp.LikersString.lastIndexOf(","));
                        }

                        temp.ImageURL=query.value(5).toString();

                        int day=query.value(7).toDateTime().daysTo(QDateTime::currentDateTime());
                        if(day==0)
                        temp.PostTime="今天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day==1)
                        temp.PostTime="昨天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day==2)
                        temp.PostTime="前天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day>2)
                        temp.PostTime=QString::number(day)+"天前 "+query.value(7).toDateTime().toString("hh:mm");

                        if(day>30)
                        temp.PostTime=query.value(7).toDateTime().toString("MM-dd hh:mm");

                        if(day>300)
                        temp.PostTime=query.value(7).toDateTime().toString("yyyy-MM-dd hh:mm");

                        clientPostList[i].append(temp);
                    }
                }
                else{

                    QString str="@getuserposts@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                    return;
                }

                QString str="@getuserposts@Succeed@";
                LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                clientConnection[i]->write(str.toUtf8());
            }




            if(message.indexOf("@getmorefriendsposts@")>=0){
                QStringList temp=message.split("@");
                int index=temp[2].toInt();

                if(index>=clientPostList[i].length()||temp.length()>3){

                    QString str="@getmorefriendsposts@NoMore@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
                else{
                    Post tp=clientPostList[i][index];

                    QString str="@getmorefriendsposts@Succeed@|||"+tp.Publisher+"|||"+tp.HeadURL+"|||"+tp.Message+"|||"+QString::number(tp.HasImage)+"|||"+tp.LikersString+"|||"+tp.ImageURL+"|||"+tp.PostTime+"|||"+QString::number(tp.ID)+"|||"+tp.PublisherID+"|||"+QString::number(tp.CommentCount);
                    LOG(QDateTime::currentDateTime().toString()+" "+"@getmorefriendsposts@Succeed@"+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }

            if(message.indexOf("@likepost@")>=0){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QSqlQuery query;

                query.exec("select * from mainposts where id="+temp[1]);

                if(query.lastError().type()==QSqlError::NoError&&teststr.length()<=3&&query.next()){

                    QStringList liketemp=query.value(4).toString().split("|");

                    bool islike=0;
                    for(int i=1;i<liketemp.length();i++)
                        if(liketemp[i]==temp[2])
                            islike=1;

                    if(islike){

                        QString str="@likepost@Liked@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                    else{

                        QString tempstr=query.value(4).toString()+"|"+temp[2];

                        QSqlQuery query1;
                        query1.exec("update mainposts set liker=\""+tempstr+"\" where id =" +temp[1]);
                        if(query1.lastError().type()==QSqlError::NoError){


                            if(temp[2]!=query.value(1).toString()){
                                QSqlQuery query2;
                                query2.prepare("insert into notices(sender,receiver,type,postid) values(?,?,?,?)");
                                query2.addBindValue(temp[2]);
                                query2.addBindValue(query.value(1).toString());
                                query2.addBindValue("点赞了");
                                query2.addBindValue(temp[1]);
                                query2.exec();
                            }


                            QString str="@likepost@Succeed@";
                            LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(str.toUtf8());

                        }
                        else{


                            QString str="@likepost@DBError@";
                            LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                            LOG(QDateTime::currentDateTime().toString()+" "+query1.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                            clientConnection[i]->write(str.toUtf8());
                        }
                    }

                }
                else{

                    QString str="@likepost@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                    LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }



            if(message.indexOf("@deletepost@")>=0){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QSqlQuery query;


                if(teststr.length()<=3){
                    query.exec("DELETE FROM mainposts WHERE mainposts.id = "+temp[1]);
                    if(query.lastError().type()==QSqlError::NoError){

                        QString str="@deletepost@Succeed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());

                    }
                    else{


                        QString str="@deletepost@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                        LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                }

            }

            if(message.indexOf("@deletecomment@")>=0){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QSqlQuery query;


                if(teststr.length()<=3){
                    QSqlQuery query2;
                    query2.exec("select * from comments where id="+temp[1]);
                    query2.next();
                    int dc=query2.value(1).toInt();

                    query.exec("DELETE FROM comments WHERE comments.id = "+temp[1]);
                    if(query.lastError().type()==QSqlError::NoError){




                        QSqlQuery query1;
                        query1.exec("update mainposts set commentcount=commentcount-1 where id =" +QString::number(dc));

                        QString str="@deletecomment@Succeed@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());

                    }
                    else{

                        QString str="@deletecomment@DBError@";
                        LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                        LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                        clientConnection[i]->write(str.toUtf8());
                    }
                }

            }

            if(message.indexOf("@getuniquepost@")>=0){
                QStringList teststr=message.split("@");
                QStringList temp=message.split("|||");
                QSqlQuery query;

                query.exec("select * from mainposts where id="+temp[1]);

                if(query.lastError().type()==QSqlError::NoError&&teststr.length()<=3&&query.next()){


                        QString likerstring=query.value(4).toString();

                        if(!likerstring.isEmpty()){
                            QStringList likername=likerstring.split("|");
                            likerstring="❤ ";
                            for(int i=1;i<likername.length();i++){
                                QSqlQuery query3;
                                query3.exec("select * from userandpassword where username=\""+likername[i]+"\"");
                                query3.next();
                                likerstring=likerstring+query3.value(3).toString()+",";
                            }
                            likerstring=likerstring.left(likerstring.lastIndexOf(","));
                        }
                        QString PostTime;
                        int day=query.value(7).toDateTime().daysTo(QDateTime::currentDateTime());
                        if(day==0)
                        PostTime="今天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day==1)
                        PostTime="昨天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day==2)
                        PostTime="前天"+query.value(7).toDateTime().toString("hh:mm");

                        if(day>2)
                        PostTime=QString::number(day)+"天前 "+query.value(7).toDateTime().toString("hh:mm");

                        if(day>30)
                        PostTime=query.value(7).toDateTime().toString("MM-dd hh:mm");

                        if(day>300)
                        PostTime=query.value(7).toDateTime().toString("yyyy-MM-dd hh:mm");


                        QString str="@getuniquepost@Succeed@{|}|||"+query.value(3).toString()+"|||http://119.29.15.43/userhead/"+query.value(1).toString()+".jpg|||"+query.value(1).toString()+"|||"+PostTime+"|||"+query.value(2).toString()+"|||"+query.value(5).toString()+"|||"+likerstring+"|||"+query.value(0).toString();
                        LOG(QDateTime::currentDateTime().toString()+" "+"@getuniquepost@Succeed@"+" "+clientConnection[i]->peerAddress().toString());


                        clientConnection[i]->write(str.toUtf8());


                }
                else{

                    QString str="@getuniquepost@DBError@";
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());
                    LOG(QDateTime::currentDateTime().toString()+" "+query.lastError().text()+" "+clientConnection[i]->peerAddress().toString());

                    clientConnection[i]->write(str.toUtf8());
                }
            }





            if(message.indexOf("@getcomments@")>=0){
                QStringList temp=message.split("@");
                //temp以后有用

                QString allstr="|||";

                QSqlQuery query;
                query.exec("select * from comments where postid ="+temp[2]+" order by comments.commenttime ASC");
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
                    clientConnection[i]->write(str.toUtf8());
                    LOG(QDateTime::currentDateTime().toString()+" "+str+" "+clientConnection[i]->peerAddress().toString());

                    return;
                }


            }





        }
    }


}
void PostServer::removeSocket(){
    for(int i=0;i<clientConnection.length();i++)
        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState){
            clientConnection.removeAt(i);
            clientPostList.removeAt(i);
        }

}
#endif // SENDPOSTSYSTEM_H
