#include <QCoreApplication>
#include <QDate>
#include <iostream>
#include <QString>
#include <QStringList>
#include "LoginServer.h"
#include "RegistServer.h"
#include "DBTimer.h"
#include "DataServer.h"
#include "PostSystem.h"
#include "RecordServer.h"
#include "ReportServer.h"
#include "NewsServer.h"
#include "RecommendServer.h"


int main(int argc, char *argv[])
{
    QCoreApplication qwe(argc, argv);
    MyDB* abc=new MyDB;

    int a;
    std::cout<<"1:RegistServer,LoginServer,PostServer"<<std::endl;
    std::cout<<"2:DataServer,RecordServer"<<std::endl;
    std::cout<<"3:NewsServer"<<std::endl;
    std::cout<<"4:ReportServer,RecommendServer"<<std::endl;

    std::cin>>a;

    //139.199.197.177
    if(a==1){
    RegistServer* rs=new RegistServer;//5555
    LoginServer* ls=new LoginServer;//6666
    NewsServer* news=new NewsServer;//4567
    }

    //123.207.249.175
    if(a==2){
    RecordServer* rcs=new RecordServer;//6789
    ReportServer* rps=new ReportServer;//12345
    }

    //119.29.15.43
    if(a==3){
    DataServer* ds=new DataServer;//8889
    PostServer* ps=new PostServer;//8520
    }

    //119.29.3.67
    if(a==4){
    RecommendServer* rec=new RecommendServer;//24567
    }







//    QFile file12("123.txt");
//    file12.open(QIODevice::ReadOnly);
//    QTextStream in1(&file12);

//    QList<QString> IDList;
//    QList<QString> NameList;

//    for(int i=0;i<50;i++){
//        QString temp;
//        in1>>temp;
//        IDList.append(temp);
//        in1>>temp;
//        NameList.append(temp);
//    }

//    int qqq=1;
//    while(qqq<11){
//    QStringList foodlist;
//    QFile file123(QString::number(qqq)+".txt");
//    file123.open(QIODevice::ReadOnly);
//    QTextStream in13(&file123);

//    for(int i=0;i<216;i++){
//        QString temp;
//        temp=in13.readLine();
//        if(temp!=""){
//            if(temp=="空")
//                foodlist.append("");
//            else
//                foodlist.append(temp);
//        }
//    }
//    for(int i=0;i<31;i++){
//        QSqlQuery query;
//        query.prepare("insert into diets (userid,date,breakfast,lunch,dinner,snack,dessert,others) values(?,?,?,?,?,?,?,?)");
//        query.addBindValue(IDList[qqq]);

//        query.addBindValue("2016-10-"+QString::number(i+1));

//        query.addBindValue(foodlist[(i*6)]);
//        query.addBindValue(foodlist[(i*6)+1]);
//        query.addBindValue(foodlist[(i*6)+2]);
//        query.addBindValue(foodlist[(i*6)+3]);
//        query.addBindValue(foodlist[(i*6)+4]);
//        query.addBindValue(foodlist[(i*6)+5]);
//        query.exec();
//    }
//    qqq++;
//    }




//    qDebug()<<"done!"<<endl;


    return qwe.exec();
}






