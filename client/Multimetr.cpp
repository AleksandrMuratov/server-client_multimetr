#include "Multimetr.h"

Multimetr::Multimetr(QString path_to_server, int count_channels, QWidget* parent):
    QWidget(parent)
    , widget(new QQuickWidget(QUrl("qrc:/main.qml"), this))
    , socket(new SocketUnix(std::move(path_to_server), this))
    , buf_values(count_channels, 0.0f)
    , buf_statuses(count_channels, StatusChannel::IDLE_STATE)
    , buf_ranges(count_channels, 0)
{
    QQuickItem* root = widget->rootObject();
    if(root){
        log_info = root->findChild<QObject*>("Log info");
        button_start_measure = root->findChild<QObject*>("Start");
        if(button_start_measure){
            QObject::connect(button_start_measure, SIGNAL(clicked()), this, SLOT(slotStart()));
        }
        button_stop_measure = root->findChild<QObject*>("Stop");
        if(button_stop_measure){
            QObject::connect(button_stop_measure, SIGNAL(clicked()), this, SLOT(slotStop()));
        }
        button_get_value = root->findChild<QObject*>("Get_value");
        if(button_get_value){
            QObject::connect(button_get_value, SIGNAL(clicked()), this, SLOT(slotGetValue()));
        }
        button_get_values = root->findChild<QObject*>("Get_values");
        if(button_get_values){
            QObject::connect(button_get_values, SIGNAL(clicked()), this, SLOT(slotGetValues()));
        }
        button_get_status = root->findChild<QObject*>("Get_status");
        if(button_get_status){
            QObject::connect(button_get_status, SIGNAL(clicked()), this, SLOT(slotGetStatus()));
        }
        button_get_statuses = root->findChild<QObject*>("Get_statuses");
        if(button_get_statuses){
            QObject::connect(button_get_statuses, SIGNAL(clicked()), this, SLOT(slotGetStatuses()));
        }
        button_set_range = root->findChild<QObject*>("Set_range");
        if(button_set_range){
            QObject::connect(button_set_range, SIGNAL(clicked()), this, SLOT(slotSetRange()));
        }
        display = root->findChild<QObject*>("Display");
        num_of_channel = root->findChild<QObject*>("Num_of_channel");
        if(num_of_channel){
            num_of_channel->setProperty("to", std::max(count_channels - 1, 0));
            connect(num_of_channel, SIGNAL(valueModified()), this, SLOT(slotShowDisplay()));
        }
        range = root->findChild<QObject*>("Range");
        scroll = root->findChild<QObject*>("Scroll");
        connect(socket, SIGNAL(signalError(QString)), this, SLOT(slotPrintError(QString)));
        slotShowDisplay();
    }
}

void Multimetr::slotStart(){
    QString answear = RequestForOneChannel("start_measure channel");
    answear = answear.trimmed();
    QString split_ok = answear.left(2);
    if(split_ok == "ok"){
        slotGetStatus();
    }
    slotShowDisplay();
}

void Multimetr::slotStop(){
    QString answear = RequestForOneChannel("stop_measure channel");
    answear = answear.trimmed();
    QString split_ok = answear.left(2);
    if(split_ok == "ok"){
        slotGetStatus();
    }
    slotShowDisplay();
}

void Multimetr::slotGetValue(){
    QString answear = RequestForOneChannel("get_result channel");
    QVector<QStringRef> words = answear.splitRef(QRegExp("[\\s,]+"));
    if(words.size() == 2 && words.front() == "ok"){
        int channel = num_of_channel->property("value").toInt();
        buf_values[channel] = words[1].toFloat();
        slotGetStatus();
    }
    slotShowDisplay();
}

void Multimetr::slotGetValues(){
    QString answear = RequestForMultChannels("get_result ");
    QVector<QStringRef> words = answear.splitRef(QRegExp("[\\s,]+"));
    if(words.size() == static_cast<int>(buf_values.size()) + 1 && words.front() == "ok"){
        for(int i = 1; i < words.size(); ++i){
            buf_values[i - 1] = words[i].toFloat();
        }
        slotGetStatuses();
    }
    slotShowDisplay();
}

void Multimetr::slotGetStatus(){
    QString answear = RequestForOneChannel("get_status channel");
    QVector<QStringRef> words = answear.splitRef(QRegExp("[\\s,]+"));
    if(words.size() == 2 && words.front() == "ok"){
        int channel = num_of_channel->property("value").toInt();
        buf_statuses[channel] = StringToStatus(words[1]);
    }
    slotShowDisplay();
}

void Multimetr::slotGetStatuses(){
    QString answear = RequestForMultChannels("get_status ");
    QVector<QStringRef> words = answear.splitRef(QRegExp("[\\s,]+"));
    if(words.size() == static_cast<int>(buf_statuses.size()) + 1 && words.front() == "ok"){
        for(int i = 1; i < words.size(); ++i){
            buf_statuses[i - 1] = StringToStatus(words[i]);
        }
    }
    slotShowDisplay();
}

void Multimetr::slotSetRange(){
    QByteArray request = "set_range channel";
    request += GetNumChannelStr().c_str();
    request += ", range";
    int current_index = range->property("currentIndex").toInt();
    request += std::to_string(current_index).c_str();
    PrintLog("request: " + request);
    QString answear = socket->RequestToServer(request);
    PrintLog(answear);
    QVector<QStringRef> words = answear.splitRef(QRegExp("[\\s,]+"));
    if(words.size() == 2 && words.front() == "ok"){
        int cnt_digit = 0;
        QStringRef range = words[1];
        for(int i = range.size() - 1; i >=0; --i){
            if(range[i].isDigit()){
                ++cnt_digit;
            }
            else{
                break;
            }
        }
        QStringRef num_range = range.right(cnt_digit);
        int channel = num_of_channel->property("value").toInt();
        buf_ranges[channel] = num_range.toInt();
        slotGetValue();
    }
}

void Multimetr::slotPrintError(QString err){
    PrintLog(err);
}

void Multimetr::slotShowDisplay(){
    int channel = num_of_channel->property("value").toInt();
    QString out;
    if(buf_statuses[channel] != StatusChannel::MEASURE_STATE){
        out = "---------\nSTATE: ";
    }
    else{
        out = std::to_string(buf_values[channel]).c_str();
        out += "\nSTATE: ";
    }
    out += StatusToString(buf_statuses[channel]);
    out += "\nrange";
    out += std::to_string(buf_ranges[channel]).c_str();
    QMetaObject::invokeMethod(display, "clear");
    QMetaObject::invokeMethod(display, "append", Q_ARG(QString, out));
}

void Multimetr::PrintLog(QString str){
    QMetaObject::invokeMethod(log_info, "append", Q_ARG(QString, str));
    QMetaObject::invokeMethod(scroll, "scrollToDown");
}

std::string Multimetr::GetNumChannelStr(){
    int channel = num_of_channel->property("value").toInt();
    return std::to_string(channel);
}

QString Multimetr::RequestForOneChannel(QByteArray request){
    request += GetNumChannelStr().c_str();
    PrintLog("request: " + request);
    QString answear = socket->RequestToServer(request);
    PrintLog(answear);
    return answear;
}

QString Multimetr::RequestForMultChannels(QByteArray request){
    bool is_first = true;
    for(int n = 0; n < static_cast<int>(buf_values.size()); ++n){
        if(!is_first){
            request += ", ";
        }
        is_first = false;
        request += "channel";
        request += std::to_string(n).c_str();
    }
    PrintLog("request: " + request);
    QString answear = socket->RequestToServer(request);
    PrintLog(answear);
    return answear;
}

QString Multimetr::StatusToString(StatusChannel status) const{
    switch (status) {
    case StatusChannel::IDLE_STATE:
        return "IDLE";
    case StatusChannel::MEASURE_STATE:
        return "MEASURE";
    case StatusChannel::BUSY_STATE:
        return "BUSY";
    case StatusChannel::ERROR_STATE:
        return "ERROR";
    default:
        return "";
    }
}

Multimetr::StatusChannel Multimetr::StringToStatus(QStringRef status) const{
    if(status == "measure_state"){
        return StatusChannel::MEASURE_STATE;
    }
    else if(status == "busy_state"){
        return StatusChannel::BUSY_STATE;
    }
    else if(status == "idle_state"){
        return StatusChannel::IDLE_STATE;
    }
    else{
        return StatusChannel::ERROR_STATE;
    }
}
