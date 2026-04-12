#include "form.h"
#include "ui_form.h"
#include <QDateTime>
#include <QTimeZone>
#include <QTimer>
Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    // 创建定时器，设置每秒触发一次
    QTimer *timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &Form::setBeijingTimeToUI);
    setBeijingTimeToUI();
    timeTimer->start(1000);

}

Form::~Form()
{
    delete ui;
}

void Form::keyPressEvent(QKeyEvent *event)
{
    // 1. 判断是否是手机返回键（Qt::Key_Back 对应 Android 物理返回键/虚拟返回键）
    if (event->key() == Qt::Key_Back) {
        // 2. 阻止返回键的默认行为（默认是直接退出整个程序）
        event->accept();

        // 3. 触发自定义信号，通知 MainWindow 执行返回逻辑
        emit backToMainWindow();

        // 可选：直接隐藏当前 Form（也可以在 MainWindow 中处理）
        this->hide();
    }
    // 4. 其他按键事件，交给父类处理（保留原有功能）
    else {
        QWidget::keyPressEvent(event);
    }
}

void Form::handlerdata(QString receivedText)
{
    QStringList messagelist = receivedText.split("(-)");
    int message_len = messagelist.size();
    for(int i = 0;i<message_len;i++)
    {
        QStringList single_item = messagelist[i].split("(:)");
        if(single_item[0] == "TIME")
        {
            ui->current_watch_time->setText(single_item[1]);
        }
        else if(single_item[0] == "TEMP1")
        {
            ui->current_watch_temp2->setText(single_item[1]);
        }
        else if(single_item[0] == "TEMP2")
        {
            ui->current_watch_temp->setText(single_item[1]);
        }
        else if(single_item[0] == "Pressure")
        {
            ui->current_watch_pressure->setText(single_item[1]);
        }
        else if(single_item[0] == "HIGH")
        {
            ui->current_watch_high->setText(single_item[1]);
        }
        else if(single_item[0] == "H_temp")
        {
            ui->H_temp->setText(single_item[1]);
        }
        else if(single_item[0] == "L_temp")
        {
            ui->L_temp->setText(single_item[1]);
        }
        else if(single_item[0] == "L_PRE")
        {
            ui->L_pre->setText(single_item[1]);
        }
        else if(single_item[0] == "OFF")
        {
            // 将毫秒转换为秒显示
            int ms = single_item[1].toInt();
            int sec = ms / 1000;
            ui->current_watch_extingguishtime->setText(QString::number(sec) + "s");
        }


    }
}

void Form::setBeijingTimeToUI()
{
    QDateTime currentLocalTime = QDateTime::currentDateTime();
    QTimeZone beijingTimeZone("Asia/Shanghai"); // 东八区，对应北京时间
    QDateTime currentBeijingTime = QDateTime::currentDateTimeUtc().toTimeZone(beijingTimeZone);

    QString timeStr = currentBeijingTime.toString("yyyy-M-d HH:mm:ss");
    ui->beijingtime->setText(timeStr);
}

void Form::on_btn_checktime_clicked()
{
    QString calibCmd = QString("check_time+%1\r\n").arg(ui->beijingtime->text().trimmed());
    emit senddata(calibCmd);
}


void Form::handleBluetoothDisconnected()
{
    // 显示蓝牙断开提示
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "蓝牙断开", "蓝牙连接已断开，是否重新连接？",
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 发出重新连接信号
        emit reconnectBluetooth();
    }
}

void Form::on_btn_makesure_clicked()
{
    // 验证输入
    bool valid = true;
    QString errorMessage = "";
    
    // 获取当前手表上的温度值
    float currentHtemp = ui->H_temp->text().toFloat();
    float currentLtemp = ui->L_temp->text().toFloat();
    
    // 验证高温设置
    if(!ui->modify_temp_H->text().isEmpty())
    {
        bool ok;
        float htemp = ui->modify_temp_H->text().toFloat(&ok);
        if(!ok)
        {
            valid = false;
            errorMessage = "高温设置必须是数字";
        }
        else if(htemp < -40 || htemp > 85)
        {
            valid = false;
            errorMessage = "高温设置范围应在-40到85之间";
        }
        else if(htemp < currentLtemp)
        {
            valid = false;
            errorMessage = "高温设置不能低于当前低温值";
        }
    }
    
    // 验证低温设置
    if(valid && !ui->modify_temp_L->text().isEmpty())
    {
        bool ok;
        float ltemp = ui->modify_temp_L->text().toFloat(&ok);
        if(!ok)
        {
            valid = false;
            errorMessage = "低温设置必须是数字";
        }
        else if(ltemp < -40 || ltemp > 85)
        {
            valid = false;
            errorMessage = "低温设置范围应在-40到85之间";
        }
        else if(ltemp > currentHtemp)
        {
            valid = false;
            errorMessage = "低温设置不能高于当前高温值";
        }
    }
    
    // 验证气压设置
    if(valid && !ui->modify_preesure_L->text().isEmpty())
    {
        bool ok;
        float lpre = ui->modify_preesure_L->text().toFloat(&ok);
        if(!ok)
        {
            valid = false;
            errorMessage = "气压设置必须是数字";
        }
        else if(lpre < 1000)
        {
            valid = false;
            errorMessage = "气压设置不能低于1000";
        }
    }
    
    // 验证息屏时间设置
    if(valid && !ui->modify_extingguishtime->text().isEmpty())
    {
        bool ok;
        int offtime = ui->modify_extingguishtime->text().toInt(&ok);
        if(!ok)
        {
            valid = false;
            errorMessage = "息屏时间必须是数字";
        }
        else if(offtime < 5 || offtime > 60)
        {
            valid = false;
            errorMessage = "息屏时间范围应在5到60秒之间";
        }
    }
    
    // 验证高温不能低于低温（当同时设置高温和低温时检查）
    if(valid && !ui->modify_temp_H->text().isEmpty() && !ui->modify_temp_L->text().isEmpty())
    {
        float htemp = ui->modify_temp_H->text().toFloat();
        float ltemp = ui->modify_temp_L->text().toFloat();
        if(htemp < ltemp)
        {
            valid = false;
            errorMessage = "高温设置不能低于低温设置";
        }
    }
    
    // 如果验证失败，显示错误信息
    if(!valid)
    {
        QMessageBox::warning(this, "输入错误", errorMessage);
        return;
    }
    
    // 构建修改命令
    QString modify_message = "modify_env+";
    if(!ui->modify_temp_H->text().isEmpty())
    {
        modify_message += "$M_Htemp:" + ui->modify_temp_H->text() + "$";
    }
    if(!ui->modify_temp_L->text().isEmpty())
    {
        modify_message += "$M_Ltemp:" + ui->modify_temp_L->text() + "$";
    }
    if(!ui->modify_preesure_L->text().isEmpty())
    {
        modify_message += "$M_Lpre:" + ui->modify_preesure_L->text() + "$";
    }
    if(!ui->modify_extingguishtime->text().isEmpty())
    {
        // 将秒数转换为毫秒（乘以1000）
        int offtime = ui->modify_extingguishtime->text().toInt();
        int offtime_ms = offtime * 1000;
        modify_message += "$M_OffTime:" + QString::number(offtime_ms) + "$";
    }

    if(modify_message != "modify_env+")
    {
        modify_message += "\r\n";
        emit senddata(modify_message);
    }

    // 清空输入框
    ui->modify_temp_H->clear();
    ui->modify_temp_L->clear();
    ui->modify_preesure_L->clear();
    ui->modify_extingguishtime->clear();
}

