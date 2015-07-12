#ifndef COMPORTLIBRARY_H
#define COMPORTLIBRARY_H

#include "comportlibrary_global.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QtSerialPort>
#include <QTimer>
#include <QMainWindow>


enum State{
    WAIT,
    READ,
    WRITE,
    FINISH,
    NOT_CON,
    CONNECT
};


extern bool stopTransmit;

class COMPORTLIBRARYSHARED_EXPORT PortSettings{
public:
    qint32 setBaudRate;
    QSerialPort::FlowControl setFlowControl;
    QSerialPort::Parity setParity;
    QSerialPort::DataBits setDataBits;
    QSerialPort::StopBits setStopBits;
    PortSettings();
    PortSettings(const qint32 &setBaud,const QSerialPort::FlowControl &setFlow,const QSerialPort::Parity &setPar,const QSerialPort::DataBits &setData,const QSerialPort::StopBits &setStop);
};


class COMPORTLIBRARYSHARED_EXPORT ComPortLibrary:public QMainWindow
{
     Q_OBJECT
private:
    State state_port;
    QSerialPort* serial;
    QTimer* timer;
    QByteArray data_byte;
    QByteArray string_error;
    int dataInt;
    QSerialPortInfo curSerPort;
    QByteArray data_state;
    QString desireSerPort;
    bool error_disconnect;
public:
    void copyArray(char* buf){strcpy(buf,data_byte.data());}
    char* getData() {return data_byte.data();}
    char* getError() {return string_error.data();}
    void write_command(char *buf_com);
    ComPortLibrary(QWidget* parent = 0);
    char* get_state();
    void portSettings(QSerialPort* serialPort, PortSettings portSetting = PortSettings());
    bool findPort(QString descripton,QSerialPortInfo &findPortInfo);
    bool checkSerCon() {return error_disconnect;}
    void init_device(QString findedPort);
    void stopTrans() {stopTransmit = true;}
    void startTrans() {stopTransmit = false;}
    int getDataInt() {return dataInt;}
    ~ComPortLibrary();
public slots:
    bool readFromPort();
    void update();
    void amIhere();
};

#endif // COMPORTLIBRARY_H
