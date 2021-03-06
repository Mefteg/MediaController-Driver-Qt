#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QSerialPortInfo>

#include "windowsactionmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_model(nullptr)
    , m_serialPortList(nullptr)
    , m_connectButton(nullptr)
    , m_serial(nullptr)
{
    m_ui->setupUi(this);
    m_model = new QStringListModel();
    m_serialPortList = m_ui->serialPortList;
    m_connectButton = m_ui->ConnectButton;

    // Connect UI events.
    connect(m_connectButton, SIGNAL (released()), this, SLOT (handleConnectButton()));

    m_serial = new QSerialPort();

    // Connect serial events.
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    QStringList serialPortNames;
    // List serial port.
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo serialPort, serialPorts)
    {
        serialPortNames.append(serialPort.portName());
    }
    m_model->setStringList(serialPortNames);

    m_serialPortList->setModel(m_model);
}

MainWindow::~MainWindow()
{
    if (m_serial != nullptr)
    {
        closeSerialPort();
        delete m_serial;
    }

    delete m_model;
    delete m_ui;
}

void MainWindow::openSerialPort()
{
    QModelIndex selectedSerialPort = m_serialPortList->currentIndex();
    if (selectedSerialPort.isValid() == false)
    {
        return;
    }

    QStringList serialPortNames = m_model->stringList();
    QString selectedSerialPortName = serialPortNames[selectedSerialPort.internalId()];

    if (m_serial->isOpen())
    {
        m_serial->close();
    }

    m_serial->setPortName(selectedSerialPortName);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    if (m_serial->open(QIODevice::ReadWrite))
    {
        qDebug("Connection opened successfully !");
    }
    else
    {
        qDebug("Error while trying to open a connection...");
    }
}

/*********
 * SLOTS *
 * *******/

void MainWindow::handleConnectButton()
{
    qDebug("Connect !");

    // TODO : Add UI to do that.
    // Open serial port COM4 (hardcoded)
    openSerialPort();
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
        qDebug("Connection closed.");
    }
}

void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    qDebug(data.toStdString().c_str());

    m_mediaManager.handleData(data);
}

void MainWindow::writeData(const QByteArray &data)
{
    if (m_serial != nullptr && m_serial->isOpen() == true)
    {
        m_serial->write(data);
    }
}
