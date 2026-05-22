#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <QObject>
#include <QSerialPort>

// 串口通信封装层：集中管理 QSerialPort 的打开、关闭、收发和错误信号。
// 内部维护接收缓冲区，按 AA...BB 帧组装后发射，主窗口收到的始终是完整帧。
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
    // 向 UI 层发射完整帧，由 UI 决定如何显示或解析。
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);
    void openStateChanged(bool open);

private slots:
    void handleReadyRead();
    void handleErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serialPort;
    QByteArray m_buffer;
};

#endif // SERIAL_MANAGER_H
