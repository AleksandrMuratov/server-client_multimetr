#pragma once

#include <QString>
#include <QLocalSocket>

class SocketUnix : public QObject{
    Q_OBJECT
private:
    const QString strHost_;
    QLocalSocket* localSocket;
    QString answear_;
public:
    SocketUnix(const QString& strHost, QObject* parent = 0);
    QString RequestToServer(QByteArray arrBlock);
public slots:
    void slotSendToServer(QByteArray arrBlock);
private slots:
    void slotError(QLocalSocket::LocalSocketError);
private:
    void Read();
signals:
    void signalConnected();
    void signalError(QString err);
};
