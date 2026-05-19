#include "main_window.h"
#include "./ui_main_window.h"
#include "../serial/serial_manager.h"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialManager(new SerialManager(this))
{
    ui->setupUi(this);
    initSerialUi();
    initSerialConnections();
    refreshSerialPorts();
    updateSerialUiState(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshSerialPorts()
{
    // 虚拟串口不额外扫描；需要测试 socat 时直接手动输入 /dev/pts/X。
    const QString currentPort = ui->comboBoxSerialPort->currentText().trimmed();

    ui->comboBoxSerialPort->clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : ports) {
        ui->comboBoxSerialPort->addItem(portInfo.systemLocation());
    }

    if (!currentPort.isEmpty()) {
        const int index = ui->comboBoxSerialPort->findText(currentPort);
        if (index >= 0) {
            ui->comboBoxSerialPort->setCurrentIndex(index);
        } else {
            ui->comboBoxSerialPort->setEditText(currentPort);
        }
    }
}

void MainWindow::toggleSerialPort()
{
    if (m_serialManager->isOpen()) {
        closeSerialPort();
        return;
    }

    openSerialPort();
}

void MainWindow::openSerialPort()
{
    // 下拉框可编辑，因此既支持 Qt 枚举端口，也支持手动输入虚拟串口路径。
    const QString portName = ui->comboBoxSerialPort->currentText().trimmed();
    if (portName.isEmpty()) {
        handleSerialError(tr("请选择或输入串口号"));
        return;
    }

    const bool opened = m_serialManager->open(portName,
                                              ui->comboBoxBaudRate->currentText().toInt(),
                                              currentDataBits(),
                                              currentParity(),
                                              currentStopBits(),
                                              currentFlowControl());
    if (opened) {
        statusBar()->showMessage(tr("串口已打开：%1").arg(portName));
    }
}

void MainWindow::closeSerialPort()
{
    m_serialManager->close();
    statusBar()->showMessage(tr("串口已关闭"));
}

void MainWindow::sendSerialData()
{
    const QByteArray data = currentSendData();
    if (data.isEmpty()) {
        return;
    }

    const qint64 written = m_serialManager->writeData(data);
    if (written >= 0) {
        ui->plainTextEditReceiveLog->appendPlainText(tr("TX: %1").arg(QString::fromUtf8(data.toHex(' ').toUpper())));
    }
}

void MainWindow::handleSerialDataReceived(const QByteArray &data)
{
    ui->plainTextEditReceiveLog->appendPlainText(tr("RX: %1").arg(QString::fromUtf8(data.toHex(' ').toUpper())));
}

void MainWindow::handleSerialError(const QString &message)
{
    statusBar()->showMessage(message);
    ui->plainTextEditReceiveLog->appendPlainText(tr("ERR: %1").arg(message));
}

void MainWindow::updateSerialUiState(bool open)
{
    ui->pushButtonOpenConnection->setText(open ? tr("关闭串口") : tr("打开串口"));
    ui->pushButtonSend->setEnabled(open);
}

void MainWindow::initSerialUi()
{
    ui->comboBoxSerialPort->setEditable(true);
    ui->comboBoxBaudRate->setCurrentText("115200");
    ui->pushButtonOpenConnection->setText(tr("打开串口"));
    ui->pushButtonSend->setEnabled(false);
    statusBar()->showMessage(tr("串口未打开"));
}

void MainWindow::initSerialConnections()
{
    connect(ui->pushButtonRefreshPorts, &QPushButton::clicked,
            this, &MainWindow::refreshSerialPorts);
    connect(ui->pushButtonOpenConnection, &QPushButton::clicked,
            this, &MainWindow::toggleSerialPort);
    connect(ui->pushButtonSend, &QPushButton::clicked,
            this, &MainWindow::sendSerialData);
    connect(ui->pushButtonClearLog, &QPushButton::clicked,
            ui->plainTextEditReceiveLog, &QPlainTextEdit::clear);

    connect(m_serialManager, &SerialManager::dataReceived,
            this, &MainWindow::handleSerialDataReceived);
    connect(m_serialManager, &SerialManager::errorOccurred,
            this, &MainWindow::handleSerialError);
    connect(m_serialManager, &SerialManager::openStateChanged,
            this, &MainWindow::updateSerialUiState);
}

QByteArray MainWindow::currentSendData() const
{
    const QString text = ui->lineEditSendCommand->text().trimmed();
    if (text.isEmpty()) {
        return {};
    }

    if (ui->checkBoxHexSend->isChecked()) {
        // HEX 模式允许输入形如 "01 03 00 00" 的命令。
        return QByteArray::fromHex(text.toUtf8());
    }

    return text.toUtf8();
}

QSerialPort::DataBits MainWindow::currentDataBits() const
{
    // UI 下拉框保存显示文本，这里统一转换为 QSerialPort 枚举值。
    const int value = ui->comboBoxDataBits->currentText().toInt();
    switch (value) {
    case 5:
        return QSerialPort::Data5;
    case 6:
        return QSerialPort::Data6;
    case 7:
        return QSerialPort::Data7;
    default:
        return QSerialPort::Data8;
    }
}

QSerialPort::Parity MainWindow::currentParity() const
{
    const QString value = ui->comboBoxParity->currentText();
    if (value == "Even") {
        return QSerialPort::EvenParity;
    }
    if (value == "Odd") {
        return QSerialPort::OddParity;
    }
    return QSerialPort::NoParity;
}

QSerialPort::StopBits MainWindow::currentStopBits() const
{
    const QString value = ui->comboBoxStopBits->currentText();
    if (value == "1.5") {
        return QSerialPort::OneAndHalfStop;
    }
    if (value == "2") {
        return QSerialPort::TwoStop;
    }
    return QSerialPort::OneStop;
}

QSerialPort::FlowControl MainWindow::currentFlowControl() const
{
    const QString value = ui->comboBoxFlowControl->currentText();
    if (value == "Hardware") {
        return QSerialPort::HardwareControl;
    }
    if (value == "Software") {
        return QSerialPort::SoftwareControl;
    }
    return QSerialPort::NoFlowControl;
}
