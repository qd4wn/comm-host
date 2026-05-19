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

// 主窗口只负责 UI 状态和用户交互；串口底层操作交给 SerialManager。
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // UI 事件槽：负责把按钮点击和串口信号转换为界面状态变化。
    void refreshSerialPorts();
    void toggleSerialPort();
    void openSerialPort();
    void closeSerialPort();
    void sendSerialData();
    void handleSerialDataReceived(const QByteArray &data);
    void handleSerialError(const QString &message);
    void updateSerialUiState(bool open);

private:
    // 初始化、参数读取和类型转换统一放在主窗口内部，保持 SerialManager 只关心串口本身。
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
