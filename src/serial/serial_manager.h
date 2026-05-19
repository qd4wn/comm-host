#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <QObject>
#include <QSerialPort>

// 封装 QSerialPort，避免主窗口直接处理底层串口状态。
class SerialManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialManager(QObject *parent = nullptr);

    bool open(const QString &portName,
              qint32 baudRate,
              QSerialPort::DataBits dataBits,
              QSerialPort::Parity parity,
              QSerialPort::StopBits stopBits,
              QSerialPort::FlowControl flowControl);
    void close();
    bool isOpen() const;
    qint64 writeData(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);
    void openStateChanged(bool open);

private slots:
    void handleReadyRead();
    void handleErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serialPort;
};

#endif // SERIAL_MANAGER_H
