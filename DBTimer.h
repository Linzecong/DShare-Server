#ifndef DBTIMER_H
#define DBTIMER_H

#include<QTimer>
#include<QTime>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlError>

class MyDB:public QObject{
public:
    QTimer timer;
    QSqlDatabase* db;
    MyDB();
    ~MyDB(){}
    void reconnect();
    int min;
};


MyDB::MyDB()
{
    min=0;
    timer.start(1000);
    db=new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName("10.66.145.51");
    db->setDatabaseName("project");
    db->setUserName("root");
    db->setPassword("0325518lrzlzc");
    db->setPort(3306);
    if(!db->open())
        qDebug() << "Failed to connect to root mysql admin";
    connect(&timer,&QTimer::timeout,this,&MyDB::reconnect);

}

void MyDB::reconnect()
{
    min++;
    if(min>=60*60){
        min=0;
        if(QTime::currentTime().hour()==3){
            db->close();
            db->setHostName("10.66.145.51");
            db->setDatabaseName("project");
            db->setUserName("root");
            db->setPassword("0325518lrzlzc");
            db->setPort(3306);
            db->open();
        }
    }
}

#endif // DBTIMER_H


