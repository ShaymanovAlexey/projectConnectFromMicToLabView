#include "comportlibrary.h"
#include <QApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QtSerialPort>
#include <QFile>
#include <windows.h>
#include <stdint.h>

static QApplication* a;
static ComPortLibrary* comPortLibrary;

char* command ;
int size_command;

char empthy_mas[2048] = {};

bool stopTransmit = false;
bool readyWrite = false;


PortSettings::PortSettings():
    setBaudRate(1000000),
    setFlowControl(QSerialPort::NoFlowControl),
    setParity(QSerialPort::NoParity),
    setDataBits(QSerialPort::Data8),
    setStopBits(QSerialPort::OneStop)
{}

PortSettings::PortSettings(const qint32 &setBaud,
                           const QSerialPort::FlowControl &setFlow,
                           const QSerialPort::Parity &setPar,
                           const QSerialPort::DataBits &setData,
                           const QSerialPort::StopBits &setStop):
    setBaudRate(setBaud),
    setFlowControl(setFlow),
    setParity(setPar),
    setDataBits(setData),
    setStopBits(setStop)
{}



ComPortLibrary::ComPortLibrary(QWidget *parent):
    QMainWindow(parent),state_port(WAIT),dataInt(0)
{
   timer = new QTimer(this);
   //init_Device("BioTech");
   serial = new QSerialPort;
   init_device("BioTech");
   //serial->open(QIODevice::ReadWrite);
   //connect();

   //data = new char[2048];
   //timer = new QTimer(this);
   //connect(serial,SIGNAL(bytesWritten()),this,SLOT(readFromPort()));
   QObject::connect(timer,SIGNAL(timeout()),this,SLOT(update()));

   timer->start(10);
}

void ComPortLibrary::amIhere(){
    dataInt = serial->bytesAvailable();
    if (dataInt){

        //memcpy(data,empthy_mas,2048);
//        QByteArray readData = serial->readAll();
//        QDataStream in(&readData,QIODevice::ReadOnly);
//        int sizeReceivedData = 0;

//        while(!in.atEnd()){
//            char block[512];
//            qint64 toFile = in.readRawData(block, sizeof(block));
//            sizeReceivedData += toFile;
//            data_byte.append(block,512);
//        }
        //QByteArray::fromRawData(serial->readAll().data(), 2048);
        data_byte = serial->readAll().data();
        //serial->read(data,2048);
    }
}


void ComPortLibrary::portSettings(QSerialPort* serialPort, PortSettings portSetting){

    if(!(serialPort->setBaudRate(portSetting.setBaudRate)
         &&(serialPort->setFlowControl(portSetting.setFlowControl))
         &&(serialPort->setDataBits(portSetting.setDataBits))
         &&(serialPort->setParity(portSetting.setParity))
         &&(serialPort->setStopBits(portSetting.setStopBits))))
    {
        string_error = "port_settings";
        return;
    }
}

bool ComPortLibrary::findPort(QString descripton,QSerialPortInfo &findPortInfo){
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.description() == descripton){
            findPortInfo = serialPortInfo;
            return true;
        }
    }
    string_error = "not_find_port";
    return false;
}

void ComPortLibrary::init_device(QString findedPort)
{
    QSerialPortInfo findedPortInfo;
    if (findPort(findedPort,findedPortInfo)) {
        serial->close();
        delete serial;
        serial = new QSerialPort(findedPortInfo);
        curSerPort = findedPortInfo;
        desireSerPort  = findedPort;
        if(!serial->open(QIODevice::ReadWrite)) {
            string_error = "not_init_device";
            return;
        }       
        portSettings(serial);
        connect(serial,SIGNAL(readyRead()),this,SLOT(amIhere()));
    }
    else {
        string_error = "not_find_port";
        return;
    }
}

void ComPortLibrary::update(){
    State cur_state = state_port;
    switch(cur_state)
    {
    case WAIT:
        if(!curSerPort.isValid()){
            //serial->close();
            state_port = NOT_CON;
        }
        else {
            if(!serial->isOpen()){
                state_port = CONNECT;
            }
            else {
                if(readyWrite){
                    state_port = WRITE;
                }
                else if(this->readFromPort()){
                    state_port = READ;
                }
                else
                    state_port = WAIT;
            }
        }
        break;
    case CONNECT:
        init_device(desireSerPort);
        state_port = WAIT;
        break;
    case NOT_CON:
        serial->close();
        //init_device(desireSerPort);
        //memcpy(data,empthy_mas,2048);
        error_disconnect = true;

        state_port = WAIT;
        break;
    case WRITE:
        if(curSerPort.isValid()){
            serial->setDataTerminalReady(false);
            serial->clear(QSerialPort::Input);
            serial->write(command,size_command);
            serial->setDataTerminalReady(true);
        }
        else
            string_error = "not valid data";
        readyWrite = false;
        //command = "";
        state_port = WAIT;
        //emit serial->readyRead();
        break;
    case READ:

        if(curSerPort.isValid()){
            //data = serial->readAll().toHex().data();
            //memcpy(data,empthy_mas,2048);
            //serial->read(data,2048);
            //wrapper = QByteArray::fromRawData(serial->readAll().data(), 2048);
            //data = wrapper.data();
        }
        else
            string_error = "not valid data";
        state_port = WAIT;
        break;
    case FINISH:
        state_port = WAIT;
        break;
    }
    //data = "boom";
}

char* ComPortLibrary::get_state(){
    if(state_port == WAIT){
        data_state.clear();
        data_state.append("WAIT");
        return data_state.data();
    }
    else if(state_port == READ){
        data_state.clear();
        data_state.append("READ");
        return data_state.data();
    }
    else if(state_port == WRITE){
        data_state.clear();
        data_state.append("WRITE");
        return data_state.data();
    }
    else if(state_port == FINISH){
        data_state.clear();
        data_state.append("FINISH");
        return data_state.data();
        }
    else if(state_port == NOT_CON){
        data_state.clear();
        data_state.append("NOT_CON");
        return data_state.data();
        }
    else if(state_port == CONNECT){
        data_state.clear();
        data_state.append("CONNECT");
        return data_state.data();
        }
    else {
        data_state.clear();
        data_state.append("INVALID_STATE");
        return data_state.data();
    }

}




ComPortLibrary::~ComPortLibrary()
{
   serial->close();
   delete serial;
   //delete data;
   delete timer;
}

void ComPortLibrary::write_command(char *buf_com){
    QString string_buf(buf_com);
    if(string_buf.length() != 5){
        string_error = "Invalid length";
        return ;
    }
    //if(serial->isOpen()){
    //if(serial->waitForBytesWritten(10))
        //serial->setDataTerminalReady(false);
        //serial->setRequestToSend(true);
        size_command = string_buf.length();
        command = buf_com;
        readyWrite = true;
        //serial->write(buf_com,string_buf.length());
        //serial->open(QIODevice::ReadOnly);
        //serial->setRequestToSend(false);
    string_error = serial->errorString().toUtf8().data();
}

bool ComPortLibrary::readFromPort(){
    //serial->setRequestToSend(false);
    //serial->setDataTerminalReady(true);
    string_error = serial->errorString().toUtf8().data();
    if(serial->waitForReadyRead(0)){
        //data = "read";
        return true;
    }
    return false;
}


BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL,
                    DWORD  ul_reason_for_call,
                    _In_ LPVOID    lpvReserved){
    // Perform actions based on the reason for calling.
    int  argc = 1;
    char* argv[] = { "dummy.exe", NULL };
    switch( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:

        a = new QApplication(argc,argv) ;
        comPortLibrary = new ComPortLibrary;
        a->processEvents();
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        break;

    case DLL_PROCESS_DETACH:
        delete a;
        delete comPortLibrary;
        // Perform any necessary cleanup.
        break;
    }
        return TRUE;
}




extern "C"
{
COMPORTLIBRARYSHARED_EXPORT char* getStatus()
{
    return comPortLibrary->get_state();
}
COMPORTLIBRARYSHARED_EXPORT void read_available(){
    comPortLibrary->readFromPort();
}
//COMPORTLIBRARYSHARED_EXPORT void read_available(){
//    if(serial->waitForReadyRead(10)){
//        //file->write(serial->readAll());
//        data = new char[2048];
//        data = serial->readAll().data();
//    }
//}
//COMPORTLIBRARYSHARED_EXPORT void mainFunc(){
//    start();
//}
COMPORTLIBRARYSHARED_EXPORT void startTransm(){
    comPortLibrary->startTrans();
}
COMPORTLIBRARYSHARED_EXPORT void stopTransm(){
    comPortLibrary->stopTrans();
}
COMPORTLIBRARYSHARED_EXPORT void setData(char* buf){
    comPortLibrary->write_command(buf);
}
COMPORTLIBRARYSHARED_EXPORT char* geterror(){
    return comPortLibrary->getError();
}
COMPORTLIBRARYSHARED_EXPORT char* getData(char* buf){
    //comPortLibrary->copyArray(buf);
    //data_buf = comPortLibrary->getData();
    //buf = comPortLibrary->getData();
    memcpy(buf,empthy_mas,2048);
    memcpy(buf,comPortLibrary->getData(),2048);
    return comPortLibrary->getData();
}
COMPORTLIBRARYSHARED_EXPORT int getDataInt(){
    //memcpy(buf,comPortLibrary->getData(),2048);
    return comPortLibrary->getDataInt();
}
COMPORTLIBRARYSHARED_EXPORT bool getReadWriteSt(){
    //memcpy(buf,comPortLibrary->getData(),2048);
    return readyWrite;
}
}




