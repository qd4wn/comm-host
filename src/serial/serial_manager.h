#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <QObject>
#include <QSerialPort>

// 串口通信封装层：集中管理 QSerialPort 的打开、关闭、收发和错误信号。
// 主窗口只通过该类的接口和信号交互，避免 UI 代码直接依赖底层串口状态细节。
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
    // 向 UI 层暴露原始字节流，由 UI 决定如何显示或解析。
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
