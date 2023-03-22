#include "SocketUnix.h"
#include <QObject>

SocketUnix::SocketUnix(const QString& strHost, QObject* parent)
    : QObject(parent)
    , strHost_(strHost)
{}

QString SocketUnix::RequestToServer(QByteArray arrBlock){
    answear_.clear();
    slotSendToServer(std::move(arrBlock));
    return std::move(answear_);
}

void SocketUnix::Read(){
    QByteArray arr = localSocket->readAll();
    if(arr.isEmpty()){
        answear_ = "fail";
    }
    else{
        answear_ = QString::fromLocal8Bit(arr);
    }
}

void SocketUnix::slotError(QLocalSocket::LocalSocketError err){
    QString strError =
            "Error: " + (err == QLocalSocket::ServerNotFoundError ?
                             "The server was not found." :
                             err == QLocalSocket::PeerClosedError ?
                                 "The server is closed." :
                                 err == QLocalSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(localSocket->errorString())
                                     );
    emit signalError(std::move(strError));
}

void SocketUnix::slotSendToServer(QByteArray arrBlock){
    localSocket = new QLocalSocket;
    localSocket->setServerName(strHost_);
    connect(localSocket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &SocketUnix::slotError);
    connect(localSocket, SIGNAL(connected()), SIGNAL(signalConnected()));

    localSocket->connectToServer();
    arrBlock += '\n';
    localSocket->write(arrBlock);
    if(localSocket->waitForReadyRead()){
        Read();
    }
    localSocket->disconnectFromServer();
    delete localSocket;
}
