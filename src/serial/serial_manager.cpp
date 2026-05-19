#include "serial_manager.h"

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
{
    // QSerialPort 的 readyRead/errorOccurred 转为业务层信号。
    connect(&m_serialPort, &QSerialPort::readyRead,
            this, &SerialManager::handleReadyRead);
    connect(&m_serialPort, &QSerialPort::errorOccurred,
            this, &SerialManager::handleErrorOccurred);
}

bool SerialManager::open(const QString &portName,
                         qint32 baudRate,
                         QSerialPort::DataBits dataBits,
                         QSerialPort::Parity parity,
                         QSerialPort::StopBits stopBits,
                         QSerialPort::FlowControl flowControl)
{
    if (m_serialPort.isOpen()) {
        // 重新打开前先关闭旧端口，保证参数切换时状态一致。
        m_serialPort.close();
        emit openStateChanged(false);
    }

    // 打开前一次性写入 UI 选择的串口参数，避免打开后再修改导致设备状态不一致。
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(baudRate);
    m_serialPort.setDataBits(dataBits);
    m_serialPort.setParity(parity);
    m_serialPort.setStopBits(stopBits);
    m_serialPort.setFlowControl(flowControl);

    const bool opened = m_serialPort.open(QIODevice::ReadWrite);
    if (!opened) {
        emit errorOccurred(m_serialPort.errorString());
    }

    // 无论成功或失败都通知 UI，按钮状态以实际打开结果为准。
    emit openStateChanged(opened);
    return opened;
}

void SerialManager::close()
{
    if (!m_serialPort.isOpen()) {
        return;
    }

    m_serialPort.close();
    emit openStateChanged(false);
}

bool SerialManager::isOpen() const
{
    return m_serialPort.isOpen();
}

qint64 SerialManager::writeData(const QByteArray &data)
{
    // 所有发送都走这里，便于统一处理未打开、写入失败等状态。
    if (!m_serialPort.isOpen()) {
        emit errorOccurred(tr("串口未打开"));
        return -1;
    }

    return m_serialPort.write(data);
}

void SerialManager::handleReadyRead()
{
    // readAll() 读取当前缓冲区已有数据；协议粘包/拆包后续交给解析层处理。
    emit dataReceived(m_serialPort.readAll());
}

void SerialManager::handleErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    emit errorOccurred(m_serialPort.errorString());
}
