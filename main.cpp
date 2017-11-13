#include <QCoreApplication>
#include <QDate>
#include <iostream>
#include <QString>
#include <QStringList>
#include "LoginServer.h"
#include "RegistServer.h"
#include "ImageServer.h"
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
    std::cin>>a;
    //分开两个服务器……与图片有关的，必须放在119.29.15.43
    if(a==1){
        //方便维护，所有服务区放一起了
        //139.199.197.177
        RegistServer* rs=new RegistServer;//5555
        LoginServer* ls=new LoginServer;//6666
        NewsServer* news=new NewsServer;//4567
        RecordServer* rcs=new RecordServer;//6789
        ReportServer* rps=new ReportServer;//12345
        RecommendServer* rec=new RecommendServer;//24567
    }
    else{
        //119.29.15.43

        if(a==1235)
            ImageServer* is=new ImageServer(a);//head
        else
            if(a==1234)
                ImageServer* is=new ImageServer(a);//projectimage
            else{
                DataServer* ds=new DataServer;//8889
                PostServer* ps=new PostServer;//8520
            }
    }


    //    int a;
    //    std::cout<<"1:RegistServer,LoginServer,PostServer"<<std::endl;
    //    std::cout<<"2:DataServer,RecordServer"<<std::endl;
    //    std::cout<<"3:NewsServer"<<std::endl;
    //    std::cout<<"4:ReportServer,RecommendServer"<<std::endl;
    //    std::cin>>a;
    //    //139.199.197.177
    //    if(a==1){
    //    RegistServer* rs=new RegistServer;//5555
    //    LoginServer* ls=new LoginServer;//6666
    //    NewsServer* news=new NewsServer;//4567
    //    }
    //    //123.207.249.175
    //    if(a==2){
    //    RecordServer* rcs=new RecordServer;//6789
    //    ReportServer* rps=new ReportServer;//12345
    //    }
    //    //119.29.15.43
    //    if(a==3){
    //    DataServer* ds=new DataServer;//8889
    //    PostServer* ps=new PostServer;//8520
    //    }
    //    //119.29.3.67
    //    if(a==4){
    //    RecommendServer* rec=new RecommendServer;//24567
    //    }

    return qwe.exec();
}






