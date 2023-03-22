#pragma once
#include <QtCore>
#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>
#include <vector>
#include <string>
#include "SocketUnix.h"

class Multimetr : public QWidget{
    Q_OBJECT
public:
    Multimetr(QString path_to_server, int count_channels, QWidget* parent = 0);
public slots:
    void slotStart();
    void slotStop();
    void slotGetValue();
    void slotGetValues();
    void slotGetStatus();
    void slotGetStatuses();
    void slotSetRange();

private slots:

    void slotPrintError(QString err);
    void slotShowDisplay();

private:

    void PrintLog(QString str);
    std::string GetNumChannelStr();
    QString RequestForOneChannel(QByteArray request);
    QString RequestForMultChannels(QByteArray request);

    enum class StatusChannel{
        ERROR_STATE,
        IDLE_STATE,
        MEASURE_STATE,
        BUSY_STATE
    };

    QString StatusToString(StatusChannel status) const;
    StatusChannel StringToStatus(QStringRef status) const;

private://fields

    QQuickWidget* widget;
    QObject* button_start_measure;
    QObject* button_stop_measure;
    QObject* button_get_value;
    QObject* button_get_values;
    QObject* button_get_status;
    QObject* button_get_statuses;
    QObject* button_set_range;
    QObject* display;
    QObject* num_of_channel;
    QObject* range;
    QObject* log_info;
    QObject* scroll;
    SocketUnix* socket;
    std::vector<float> buf_values;
    std::vector<StatusChannel> buf_statuses;
    std::vector<int> buf_ranges;
};
