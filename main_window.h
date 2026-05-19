#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SerialManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void refreshSerialPorts();
    void toggleSerialPort();
    void openSerialPort();
    void closeSerialPort();
    void sendSerialData();
    void handleSerialDataReceived(const QByteArray &data);
    void handleSerialError(const QString &message);
    void updateSerialUiState(bool open);

private:
    void initSerialUi();
    void initSerialConnections();
    QByteArray currentSendData() const;
    QSerialPort::DataBits currentDataBits() const;
    QSerialPort::Parity currentParity() const;
    QSerialPort::StopBits currentStopBits() const;
    QSerialPort::FlowControl currentFlowControl() const;

    Ui::MainWindow *ui;
    SerialManager *m_serialManager;
};
#endif // MAIN_WINDOW_H
