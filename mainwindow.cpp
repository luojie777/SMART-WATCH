#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothUuid>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
// HC-06 BLE模块的典型UUID
const QBluetoothUuid MainWindow::SERVICE_UUID = QBluetoothUuid(QString("0000ffe0-0000-1000-8000-00805f9b34fb"));
const QBluetoothUuid MainWindow::WRITE_CHARACTERISTIC_UUID = QBluetoothUuid(QString("0000ffe2-0000-1000-8000-00805f9b34fb"));
const QBluetoothUuid MainWindow::NOTIFY_CHARACTERISTIC_UUID = QBluetoothUuid(QString("0000ffe1-0000-1000-8000-00805f9b34fb"));

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , discoveryAgent(nullptr)
    , controller(nullptr)
    , service(nullptr)
    , responseTimer(nullptr)
    , retryCount(0)
{
    ui->setupUi(this);

    // 初始化
    initializeUI();
    initializeBLE();
    
    // 初始化响应定时器
    responseTimer = new QTimer(this);
    connect(responseTimer, &QTimer::timeout, this, &MainWindow::onResponseTimeout);

    logMessage("BLE控制器启动完成");
    logMessage("注意：请确保HC-06 BLE模块已开启");
}

MainWindow::~MainWindow()
{
    if (controller) {
        controller->disconnectFromDevice();
        delete controller;
    }

    if (discoveryAgent) {
        discoveryAgent->stop();
        delete discoveryAgent;
    }

    if (responseTimer) {
        responseTimer->stop();
        delete responseTimer;
    }

    delete ui;
}

void MainWindow::handlerbledata(QString receivedText)
{
    if (receivedText.isEmpty()) {
        logMessage("提示：请输入要发送的消息");
        return;
    }

    // 检查控制器
    if (!controller) {
        logMessage("错误：控制器未初始化");
        return;
    }

    // 检查服务
    if (!service) {
        logMessage("错误：服务未初始化");
        return;
    }
    if (service->state() != QLowEnergyService::RemoteServiceDiscovered) {
        logMessage("错误：服务未发现完成");
        return;
    }

    // 检查写特征
    if (!writeCharacteristic.isValid()) {
        logMessage("错误：写特征无效（未找到可用的写入特征）");
        qDebug() << "当前写特征UUID：" << writeCharacteristic.uuid().toString();
        const QList<QLowEnergyCharacteristic> characteristics = service->characteristics();
        for (const QLowEnergyCharacteristic &c : characteristics) {
            qDebug() << "所有可用特征：" << c.uuid().toString() << "属性：" << c.properties();
        }
        return;
    }

    // 使用带重试机制的命令发送
    sendCommandWithRetry(receivedText);

}

void MainWindow::initializeUI()
{
    // 设置窗口标题
    setWindowTitle("BLE控制器 - HC-06连接工具");

    // 初始化按钮状态
    ui->btn_stopScan->setEnabled(false);
    ui->btn_disconnect->setEnabled(false);
    ui->btn_send->setEnabled(false);
    ui->btn_connect->setEnabled(false);

    // 设置列表控件
    ui->list_devices->clear();

    // 设置状态栏
    ui->statusbar->showMessage("准备就绪");

    // 连接设备列表双击信号
    connect(ui->list_devices, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
                if (item && discoveryAgent && !discoveryAgent->isActive()) {
                    on_btn_connect_clicked();
                }
            });
}

void MainWindow::initializeBLE()
{
    // 创建设备发现代理
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    // 设置低功耗设备发现
    discoveryAgent->setLowEnergyDiscoveryTimeout(10000); // 10秒超时

    // 连接扫描信号
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &MainWindow::deviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &MainWindow::scanFinished);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &MainWindow::scanCanceled);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &MainWindow::scanError);
}



void MainWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->text_log->append(QString("[%1] %2").arg(timestamp).arg(message));

    // 自动滚动到底部
    QTextCursor cursor = ui->text_log->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->text_log->setTextCursor(cursor);
}

void MainWindow::clearLog()
{
    ui->text_log->clear();
}

void MainWindow::updateConnectionState(bool connected)
{
    if (connected) {
        ui->statusbar->showMessage("已连接到设备", 5000);
        ui->btn_connect->setEnabled(false);
        ui->btn_disconnect->setEnabled(true);
        ui->btn_send->setEnabled(true);
        ui->status_labal->setText("已连接");
        ui->input_message->setFocus();
    } else {
        ui->statusbar->showMessage("未连接", 5000);
        ui->btn_connect->setEnabled(ui->list_devices->currentItem() != nullptr);
        ui->btn_disconnect->setEnabled(false);
        ui->btn_send->setEnabled(false);
        ui->status_labal->setText("连接断开");
    }
}

void MainWindow::updateScanState(bool scanning)
{
    ui->btn_startScan->setEnabled(!scanning);
    ui->btn_stopScan->setEnabled(scanning);
}

// 扫描按钮点击
void MainWindow::on_btn_startScan_clicked()
{
    count = 0;
    clearLog();
    ui->list_devices->clear();
    discoveredDevices.clear();

    logMessage("开始扫描BLE设备...");
    logMessage("请确保蓝牙已开启且设备在范围内");

    updateScanState(true);

    // 开始扫描低功耗设备
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

// 停止扫描按钮点击
void MainWindow::on_btn_stopScan_clicked()
{
    logMessage("停止扫描");
    discoveryAgent->stop();
    if(ui->list_devices!=nullptr)
        ui->btn_connect->setEnabled(true);
    updateScanState(false);
}

// 连接按钮点击
void MainWindow::on_btn_connect_clicked()
{
    QListWidgetItem *item = ui->list_devices->currentItem();
    if (!item) {
        logMessage("错误：请先在列表中选择一个设备");
        return;
    }

    int index = ui->list_devices->row(item);
    if (index >= 0 && index < discoveredDevices.size()) {
        connectToDevice(discoveredDevices[index]);
    }
}

// 断开连接按钮点击
void MainWindow::on_btn_disconnect_clicked()
{
    disconnectFromDevice();
}

// 发送按钮点击
void MainWindow::on_btn_send_clicked()
{
    QString message = ui->input_message->text().trimmed();
    if (message.isEmpty()) {
        logMessage("提示：请输入要发送的消息");
        return;
    }

    // 检查控制器
    if (!controller) {
        logMessage("错误：控制器未初始化");
        return;
    }

    // 检查服务
    if (!service) {
        logMessage("错误：服务未初始化");
        return;
    }
    if (service->state() != QLowEnergyService::RemoteServiceDiscovered) {
        logMessage("错误：服务未发现完成");
        return;
    }

    // 检查写特征
    if (!writeCharacteristic.isValid()) {
        logMessage("错误：写特征无效（未找到可用的写入特征）");
        qDebug() << "当前写特征UUID：" << writeCharacteristic.uuid().toString();
        const QList<QLowEnergyCharacteristic> characteristics = service->characteristics();
        for (const QLowEnergyCharacteristic &c : characteristics) {
            qDebug() << "所有可用特征：" << c.uuid().toString() << "属性：" << c.properties();
        }
        return;
    }

    // 使用带重试机制的命令发送
    sendCommandWithRetry(message);

    // 清空输入框
    //ui->input_message->clear();
}

// 发现设备
void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // 只显示低功耗设备
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        QString deviceName = device.name();
        if (deviceName.isEmpty()) {
            deviceName = "未知设备";
        }

        QString deviceInfo = QString("设备%1:\n设备名:%2\n设备MAC地址:%3\n").arg(++count).arg(deviceName).arg(device.address().toString());

        // 检查是否已存在
        bool found = false;
        for (int i = 0; i < discoveredDevices.size(); ++i) {
            if (discoveredDevices[i].address() == device.address()) {
                found = true;
                // 更新设备信息
                discoveredDevices[i] = device;
                ui->list_devices->item(i)->setText(deviceInfo);
                break;
            }
        }

        if (!found) {
            discoveredDevices.append(device);
            QListWidgetItem *item = new QListWidgetItem(deviceInfo);
            ui->list_devices->addItem(item);

            // 如果是HC-06相关设备，高亮显示ming
            if (deviceName.contains("HC06", Qt::CaseInsensitive) ||
                deviceName.contains("BLE", Qt::CaseInsensitive)) {
                item->setForeground(Qt::blue);
            }
        }
    }
}

// 扫描完成
void MainWindow::scanFinished()
{
    logMessage(QString("扫描完成，发现 %1 个BLE设备").arg(discoveredDevices.size()));
    updateScanState(false);

    if (discoveredDevices.isEmpty()) {
        logMessage("提示：未发现任何BLE设备，请确保设备已开启且在范围内");
    }
}

// 扫描取消
void MainWindow::scanCanceled()
{
    logMessage("扫描已取消");
    updateScanState(false);
}

// 扫描错误
void MainWindow::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QString errorMsg;
    switch (error) {
    case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
        errorMsg = "蓝牙未开启";
        break;
    case QBluetoothDeviceDiscoveryAgent::InputOutputError:
        errorMsg = "输入输出错误";
        break;
    case QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError:
        errorMsg = "无效的蓝牙适配器";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError:
        errorMsg = "平台不支持";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnsupportedDiscoveryMethod:
        errorMsg = "不支持的发现方法";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnknownError:
    default:
        errorMsg = "未知错误";
        break;
    }

    logMessage(QString("扫描错误: %1").arg(errorMsg));
    updateScanState(false);
}

// 连接到设备
void MainWindow::connectToDevice(const QBluetoothDeviceInfo &device)
{
    logMessage(QString("正在连接到: %1 [%2]").arg(device.name()).arg(device.address().toString()));

    currentDevice = device;

    // 如果已经有控制器，先清理
    if (controller) {
        controller->disconnectFromDevice();
        controller->deleteLater();
        controller = nullptr;
    }

    // 创建新的控制器
    controller = QLowEnergyController::createCentral(device, this);

    if (!controller) {
        logMessage("错误：无法创建BLE控制器");
        return;
    }

    // 连接控制器信号
    connect(controller, &QLowEnergyController::connected,
            this, &MainWindow::deviceConnected);
    connect(controller, &QLowEnergyController::disconnected,
            this, &MainWindow::deviceDisconnected);
    connect(controller, &QLowEnergyController::serviceDiscovered,
            this, &MainWindow::serviceDiscovered);
    connect(controller, &QLowEnergyController::discoveryFinished,
            this, &MainWindow::serviceScanDone);
    connect(controller, &QLowEnergyController::errorOccurred,
            this, &MainWindow::controllerError);

    // 开始连接
    controller->connectToDevice();
}

// 断开连接
void MainWindow::disconnectFromDevice()
{
    if (controller) {
        logMessage("正在断开连接...");
        ui->status_labal->setText("连接断开");
        controller->disconnectFromDevice();
    }
}

// 设备已连接
void MainWindow::deviceConnected()
{
    logMessage("设备连接成功");
    logMessage("正在发现服务...");

    updateConnectionState(true);

    // 开始发现服务
    controller->discoverServices();
}

// 设备已断开
void MainWindow::deviceDisconnected()
{
    logMessage("设备已断开连接");

    updateConnectionState(false);

    // 发出蓝牙断开信号
    emit bluetoothDisconnected();

    // 清理服务
    if (service) {
        service->deleteLater();
        service = nullptr;
    }

    // 清理特征
    writeCharacteristic = QLowEnergyCharacteristic();
    notifyCharacteristic = QLowEnergyCharacteristic();
}

// 发现服务
void MainWindow::serviceDiscovered(const QBluetoothUuid &serviceUuid)
{
    qDebug() << "发现服务:" << serviceUuid.toString();

    // 检查是否是我们需要的服务
    if (serviceUuid == SERVICE_UUID) {
        logMessage("找到目标服务");

        // 清理旧的服务
        if (service) {
            service->deleteLater();
            service = nullptr;
        }

        // 创建服务对象
        service = controller->createServiceObject(serviceUuid, this);

        if (!service) {
            logMessage("错误：创建服务对象失败");
            return;
        }

        // 连接服务信号
        connect(service, &QLowEnergyService::stateChanged,
                this, &MainWindow::serviceStateChanged);
        connect(service, &QLowEnergyService::characteristicChanged,
                this, &MainWindow::characteristicChanged);
        connect(service, &QLowEnergyService::characteristicWritten,
                this, &MainWindow::characteristicWritten);
        connect(service, &QLowEnergyService::errorOccurred,
                this, &MainWindow::serviceError);

        // 发现特征
        service->discoverDetails();
    }
}

// 服务扫描完成
void MainWindow::serviceScanDone()
{
    logMessage("服务扫描完成");

    if (!service) {
        logMessage("警告：未找到目标服务，请检查设备UUID配置");
        logMessage("目标服务UUID: " + SERVICE_UUID.toString());
    }
}

// 控制器错误
void MainWindow::controllerError(QLowEnergyController::Error error)
{
    QString errorMsg;
    switch (error) {
    case QLowEnergyController::UnknownError:
        errorMsg = "未知错误";
        break;
    case QLowEnergyController::RemoteHostClosedError:
        errorMsg = "远程主机关闭";
        break;
    case QLowEnergyController::ConnectionError:
        errorMsg = "连接错误";
        break;
    default:
        errorMsg = "控制器错误";
        break;
    }

    logMessage(QString("连接错误: %1").arg(errorMsg));
    updateConnectionState(false);
}

// 服务错误
void MainWindow::serviceError(QLowEnergyService::ServiceError error)
{
    QString errorMsg;
    switch (error) {
    case QLowEnergyService::OperationError:
        errorMsg = "操作错误";
        break;
    case QLowEnergyService::CharacteristicWriteError:
        errorMsg = "特征写入错误";
        break;
    case QLowEnergyService::DescriptorWriteError:
        errorMsg = "描述符写入错误";
        break;
    default:
        errorMsg = "服务错误";
        break;
    }

    logMessage(QString("服务错误: %1").arg(errorMsg));
}


void MainWindow::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::InvalidService: {
        logMessage("无效的服务");
        break;
    }
    case QLowEnergyService::RemoteService: {
        logMessage("远程服务");
        break;
    }
    case QLowEnergyService::RemoteServiceDiscovering: {
        logMessage("正在发现远程服务...");
        break;
    }
    case QLowEnergyService::RemoteServiceDiscovered: {
        logMessage("服务发现完成，查找特征中...");
        {
            // 查找特征
            const QList<QLowEnergyCharacteristic> characteristics = service->characteristics();

            bool writeFound = false;
            bool notifyFound = false;

            for (const QLowEnergyCharacteristic &characteristic : characteristics) {
                qDebug() << "找到特征:" << characteristic.uuid().toString()
                    << "属性:" << characteristic.properties();

                QString charUuid = characteristic.uuid().toString().toLower();
                // 优先选择 e1 作为写特征（避免被 e2 覆盖）
                bool isE1 = charUuid.contains("ffe1");
                bool isE2 = charUuid.contains("ffe2");

                // 动态识别写特征，优先 e1
                if ((characteristic.properties() & QLowEnergyCharacteristic::Write) && !writeFound) {
                    // 优先选择 e1，如果没有 e1 再选其他写特征
                    if (isE1 || (!isE1 && !isE2) || (isE2 && !isE1)) {
                        writeCharacteristic = characteristic;
                        writeFound = true;
                        logMessage("找到写特征（优先选择e1）: " + characteristic.uuid().toString());
                    }
                }

                // 动态识别通知特征（仅 e1）
                if (characteristic.properties() & QLowEnergyCharacteristic::Notify) {
                    notifyCharacteristic = characteristic;
                    notifyFound = true;
                    logMessage("找到通知特征: " + characteristic.uuid().toString());

                    // 启用通知
                    QLowEnergyDescriptor notification = characteristic.descriptor(
                        QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                    if (notification.isValid()) {
                        service->writeDescriptor(notification, QByteArray::fromHex("0100"));
                        logMessage("已启用通知");
                    }
                }
            }
            if (!writeFound) {
                logMessage("警告：未找到写特征");
            }

            if (!notifyFound) {
                logMessage("警告：未找到通知特征");
            }

            if (writeFound || notifyFound) {
                logMessage("设备就绪，可以发送数据");
            }
        } // 变量作用域结束

        break;
    }
    default: {
        logMessage(QString("未知服务状态: %1").arg(static_cast<int>(s)));
        break;
    }
    }
}

// 特征值改变（接收到数据）
void MainWindow::characteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    Q_UNUSED(c);

    if (!value.isEmpty()) {
        QString receivedText = QString::fromUtf8(value);
        
        // 消息拼接逻辑
        partialMessage += receivedText;
        
        // 检查是否收到完整消息
        if (partialMessage.contains("\n") || partialMessage.contains("(-)")) {
            // 处理完整消息
            QStringList messages = partialMessage.split("\n");
            for (const QString &message : messages) {
                QString trimmedMessage = message.trimmed();
                if (!trimmedMessage.isEmpty()) {
                    if(watch && trimmedMessage.contains("(:)")) {
                        // 发送给watch处理的消息（包含"(:)"格式）
                        emit analyzeData(trimmedMessage);
                    }
                    logMessage(QString("接收: %1").arg(trimmedMessage));
                    
                    // 处理命令响应
                    handleCommandResponse(trimmedMessage);
                }
            }
            
            // 清空部分消息
            partialMessage.clear();
        } else {
            // 消息不完整，等待下一部分
            logMessage(QString("接收部分数据: %1").arg(receivedText.trimmed()));
        }
    }
}

// 特征写入完成
void MainWindow::characteristicWritten(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    Q_UNUSED(c);
    Q_UNUSED(value);
    logMessage("数据发送成功");
}

void MainWindow::on_btn_watch_clicked()
{
    if(!watch)
    {
        watch = new Form(nullptr);
        connect(this,&MainWindow::analyzeData,watch,&Form::handlerdata);
        connect(watch,&Form::senddata,this,&MainWindow::handlerbledata);
        // 连接蓝牙断开信号
        connect(this, &MainWindow::bluetoothDisconnected, watch, &Form::handleBluetoothDisconnected);
        // 2. 核心：绑定 Form 的返回信号到 MainWindow 的槽函数（这里用 lambda 表达式，简洁高效）
        connect(watch, &Form::backToMainWindow, this, [this]() {
            // 3. 显示 MainWindow 主界面
            this->showFullScreen(); // 保持全屏，和之前的适配逻辑一致

            // 4. 隐藏 Form（不删除，下次点击按钮可直接显示，避免重复创建）
            if (watch) {
                watch->hide();
            }
        });
        // 连接重新连接信号
        connect(watch, &Form::reconnectBluetooth, this, &MainWindow::reconnectToLastDevice);
        this->hide();
    }
    watch->showFullScreen();
}

// 发送命令并启用重试机制
void MainWindow::sendCommandWithRetry(const QString &command)
{
    // 保存当前命令
    currentCommand = command;
    retryCount = 0;
    
    // 发送命令
    QByteArray data = command.toUtf8();
    logMessage(QString("发送: %1").arg(command));
    service->writeCharacteristic(writeCharacteristic, data, QLowEnergyService::WriteWithResponse);
    
    // 启动响应定时器
    responseTimer->start(RESPONSE_TIMEOUT);
}

// 响应超时处理
void MainWindow::onResponseTimeout()
{
    responseTimer->stop();
    
    // 检查重试次数
    if (retryCount < MAX_RETRIES) {
        retryCount++;
        logMessage(QString("响应超时，正在重试 (%1/%2)...").arg(retryCount).arg(MAX_RETRIES));
        
        // 重新发送命令
        QByteArray data = currentCommand.toUtf8();
        logMessage(QString("重发: %1").arg(currentCommand));
        service->writeCharacteristic(writeCharacteristic, data, QLowEnergyService::WriteWithResponse);
        
        // 重新启动定时器
        responseTimer->start(RESPONSE_TIMEOUT);
    } else {
        logMessage(QString("命令执行失败：%1").arg(currentCommand));
        logMessage("已达到最大重试次数，停止重试");
    }
}

// 处理命令响应
void MainWindow::handleCommandResponse(const QString &response)
{
    // 检查是否是当前命令的响应
    if (!currentCommand.isEmpty()) {
        // 根据当前命令类型检查响应
        if (currentCommand.startsWith("check_time+") && response.startsWith("CALIB_OK")) {
            // 时间校准命令的响应
            responseTimer->stop();
            retryCount = 0;
            currentCommand.clear();
            logMessage("时间校准命令执行成功");
        } else if (currentCommand.startsWith("modify_env+") && (response.startsWith("ENV_OK") || response.startsWith("ENV_ERR"))) {
            // 环境参数修改命令的响应
            responseTimer->stop();
            retryCount = 0;
            currentCommand.clear();
            logMessage("环境参数修改命令执行成功");
        }
        // 其他命令的响应处理可以在这里添加
    }
}

// 重新连接到最后设备
void MainWindow::reconnectToLastDevice()
{
    if (currentDevice.isValid()) {
        logMessage("正在重新连接到设备...");
        connectToDevice(currentDevice);
    } else {
        logMessage("没有找到上次连接的设备");
    }
}

