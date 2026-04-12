#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothUuid>
#include <QtCore>
#include <form.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



signals:
    void analyzeData(QString receivedText);
    void bluetoothDisconnected();

public slots:
    void handlerbledata(QString receivedText);

private slots:
    void on_btn_startScan_clicked();
    void on_btn_stopScan_clicked();
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void on_btn_send_clicked();

    // 扫描相关槽函数
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void scanFinished();
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanCanceled();

    // BLE连接相关槽函数
    void deviceConnected();
    void deviceDisconnected();
    void serviceDiscovered(const QBluetoothUuid &serviceUuid);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error error);
    void serviceError(QLowEnergyService::ServiceError error);

    // 服务和特征相关
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void characteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &c, const QByteArray &value);

    void on_btn_watch_clicked();


private:
    Ui::MainWindow *ui;

    Form *watch = nullptr;
    int count = 0;
    // BLE相关对象
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QLowEnergyController *controller;
    QLowEnergyService *service;

    // 用于存储发现的设备
    QList<QBluetoothDeviceInfo> discoveredDevices;
    QBluetoothDeviceInfo currentDevice;

    // BLE服务和特征UUID - HC-06 BLE通常使用的UUID
    static const QBluetoothUuid SERVICE_UUID;
    static const QBluetoothUuid WRITE_CHARACTERISTIC_UUID;
    static const QBluetoothUuid NOTIFY_CHARACTERISTIC_UUID;

    QLowEnergyCharacteristic writeCharacteristic;
    QLowEnergyCharacteristic notifyCharacteristic;

    // 重试机制相关
    QTimer *responseTimer;
    QString currentCommand;
    int retryCount;
    const int MAX_RETRIES = 3;
    const int RESPONSE_TIMEOUT = 2000; // 2秒超时
    
    // 消息拼接
    QString partialMessage;

    // 日志记录
    void logMessage(const QString &message);
    void clearLog();

    // 连接管理
    void connectToDevice(const QBluetoothDeviceInfo &device);
    void disconnectFromDevice();

    // 状态更新
    void updateConnectionState(bool connected);
    void updateScanState(bool scanning);

    // 初始化
    void initializeUI();
    void initializeBLE();
    
    // 命令发送与重试
    void sendCommandWithRetry(const QString &command);
    void onResponseTimeout();
    void handleCommandResponse(const QString &response);
    
    // 重新连接
    void reconnectToLastDevice();


};

#endif // MAINWINDOW_H
