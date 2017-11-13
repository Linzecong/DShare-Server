#ifndef IMAGESERVER_H
#define IMAGESERVER_H

#include <QtNetwork/QtNetwork>
#include <QList>
#include <QTime>
#include <QFile>
#include <QImage>
#include <QTextStream>
#include <QDataStream>

class ImageServer: public QObject{
public:
    ImageServer(int a);
    ~ImageServer();


    QTcpServer* tcpServer;
    //QList<QTcpSocket* > clientConnection;
    QTcpSocket*  clientConnection;

    //QList<qint64> totalBytes;//存放总大小信息
    //QList<qint64> bytesReceived;//已收到数据的大小
    //QList<qint64> fileNameSize;//文件名的大小信息

    qint64 totalBytes;//存放总大小信息
    qint64 bytesReceived;//已收到数据的大小
    qint64 fileNameSize;//文件名的大小信息

    QString fileName;//存放文件名
    //QList<QFile*> localFile;//本地文件
    QFile* localFile;//本地文件

    QByteArray inBlock;//数据缓冲区

    // QList<int> connectTime;

    int connectTime;

    QTimer socketTimer;
    void removeFromList();
    void socketTimerTimeout();
    void readMessage();
    void sendMessage();

private:


    QFile LogFile;
    QTextStream* LogTextStream;

};

ImageServer::ImageServer(int a){


    LogFile.setFileName("./ImageLog.str");
    LogFile.open(QIODevice::WriteOnly|QIODevice::Append);
    if(LogFile.isOpen()){
        LogTextStream=new QTextStream(&LogFile);
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenSucceed")<<endl;
    }
    else
        qDebug()<<(QTime::currentTime().toString()+"  "+"LogOpenFail")<<endl;

    clientConnection=new QTcpSocket;
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::AnyIPv4,a);
    connect(tcpServer,&QTcpServer::newConnection,this,&ImageServer::readMessage);
    connect(&socketTimer,&QTimer::timeout,this,&ImageServer::socketTimerTimeout);


}

ImageServer::~ImageServer(){


}

void ImageServer::removeFromList()
{
    //    for(int i=0;i<clientConnection.length();i++){
    //        if(clientConnection[i]->state()==QAbstractSocket::UnconnectedState)

    //            bytesReceived.removeAt(i);
    //        totalBytes.removeAt(i);
    //        fileNameSize.removeAt(i);
    //        clientConnection.removeAt(i);
    //        connectTime.removeAt(i);
    //    }
}

void ImageServer::socketTimerTimeout()
{
    connectTime++;
    if(connectTime==10){
        qDebug()<<(QTime::currentTime().toString()+"  "+"Connection Crash  "+clientConnection->peerAddress().toString());
        (*LogTextStream)<<QTime::currentTime().toString()+"  Connection Crash  "+clientConnection->peerAddress().toString()<<endl;

        QString msg="@sendimage@Wait@";
        clientConnection->write(msg.toUtf8());
        clientConnection->disconnectFromHost();
    }


}

void ImageServer::readMessage(){




    if(clientConnection->state()!=QAbstractSocket::UnconnectedState){
        QString msg="@sendimage@Wait@";
        tcpServer->nextPendingConnection()->write(msg.toUtf8());
        return;
    }

    if(socketTimer.isActive()==false)
        socketTimer.start(1000);

    connectTime=0;

    clientConnection=tcpServer->nextPendingConnection();

    bytesReceived=qint64(0);
    totalBytes=qint64(0);
    fileNameSize=qint64(0);

    qDebug()<<(QTime::currentTime().toString()+"  "+"New Connection"+"  "+clientConnection->peerAddress().toString());
    (*LogTextStream)<<(QTime::currentTime().toString()+"  "+"New Connection"+"  "+clientConnection->peerAddress().toString())<<endl;

    connect(clientConnection,&QTcpSocket::readyRead,this,&ImageServer::sendMessage);

    //connect(clientConnection.last(),&QTcpSocket::disconnected,this,&ImageServer::removeFromList);


}

void ImageServer::sendMessage(){


    if(clientConnection->bytesAvailable()>0){
        connectTime=0;
        QDataStream in(clientConnection);
        in.setVersion(QDataStream::Qt_4_6);

        if(bytesReceived<=sizeof(qint64)*3) {//如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
            if((clientConnection->bytesAvailable()>= sizeof(qint64)*3)&&(fileNameSize==0)) {//接收数据总大小信息和文件名大小信息

                qint64 check;

                in>>totalBytes>>fileNameSize;
                in>>check;
                if(check!=1234){
                    qDebug()<<(QTime::currentTime().toString()+"  "+"Unknown Message"+"  "+clientConnection->peerAddress().toString());
                    (*LogTextStream)<<(QTime::currentTime().toString()+"  "+"Unknown Message"+"  "+clientConnection->peerAddress().toString())<<endl;
                    clientConnection->disconnectFromHost();
                    return;
                }
                bytesReceived+=sizeof(qint64)*3;
            }
            else{
                qDebug()<<(QTime::currentTime().toString()+"  "+"Unknown Message"+"  "+clientConnection->peerAddress().toString());
                (*LogTextStream)<<(QTime::currentTime().toString()+"  "+"Unknown Message"+"  "+clientConnection->peerAddress().toString())<<endl;
                clientConnection->disconnectFromHost();
                return;
            }

            if((clientConnection->bytesAvailable()>= fileNameSize) &&(fileNameSize!=0))
            { //接收文件名，并建立文件
                in>>fileName;


                QDir *dir=new QDir("C:\\inetpub\\wwwroot\\userhead");
                QStringList filter;
                filter<<"*.jpg";
                dir->setNameFilters(filter);
                QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));

                for(int i=0;i<fileInfo->count();i++){
                    if(fileInfo->at(i).fileName().indexOf(fileName.left(fileInfo->at(i).fileName().indexOf("$$")))>=0){
                        QFile::remove(fileInfo->at(i).fileName());
                    }
                }

                bytesReceived+=fileNameSize;
                localFile=new QFile(fileName);

                if(!localFile->open(QFile::ReadWrite))
                {
                    qDebug()<<(QTime::currentTime().toString()+"  "+"FileOpenFail..."+"  "+fileName);
                    QString msg="@sendimage@DBError@";
                    clientConnection->write(msg.toUtf8());
                    clientConnection->disconnectFromHost();
                }
                qDebug()<<"Receiving..."+(QTime::currentTime().toString()+"  "+fileName);
                (*LogTextStream)<<"Receiving..."+(QTime::currentTime().toString()+"  "+fileName)<<endl;

            }
            else{
                qDebug()<<(QTime::currentTime().toString()+"  "+"Error Message"+"  "+clientConnection->peerAddress().toString());
                (*LogTextStream)<<(QTime::currentTime().toString()+"  "+"Error Message"+"  "+clientConnection->peerAddress().toString())<<endl;
                QString msg="@sendimage@Error@";
                clientConnection->write(msg.toUtf8());
                clientConnection->disconnectFromHost();
                return;
            }
        }

        if(bytesReceived<totalBytes)
        {//如果接收的数据小于总数据，那么写入文件
            bytesReceived+=clientConnection->bytesAvailable();
            inBlock=clientConnection->readAll();
            localFile->write(inBlock);
            inBlock.resize(0);
        }
        if(bytesReceived==totalBytes){//接收数据完成时

            localFile->close();

            //                QImage img;
            //                qDebug()<<fileName;
            //                img.load(fileName);
            //                double h=640/double(img.width());
            //                h=h*double(img.height());

            //                img=img.scaled(640,h);
            //                QString nnFileName=fileName.left(fileName.lastIndexOf('.'));
            //                nnFileName=nnFileName+"_temp.jpg";
            //                qDebug()<<nnFileName;
            //                img.save(nnFileName,"JPG",50);//非头像注释这里，下同


            QImage img;
            qDebug()<<fileName;
            img.load(fileName);
            QString nnFileName=fileName.left(fileName.indexOf('$'));
            nnFileName=nnFileName+".jpg";
            qDebug()<<nnFileName;
            img.save(nnFileName,"JPG");



            QString msg="@sendimage@Succeed@";
            clientConnection->write(msg.toUtf8());
            qDebug()<<(QTime::currentTime().toString()+"  "+"Receive succeed"+"  "+clientConnection->peerAddress().toString());
            (*LogTextStream)<<(QTime::currentTime().toString()+"  "+"Receive succeed"+"  "+clientConnection->peerAddress().toString())<<endl;
            clientConnection->disconnectFromHost();


        }
    }


}


#endif // IMAGESERVER_H
