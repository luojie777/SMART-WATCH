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
            ui->current_watch_extingguishtime->setText(single_item[1]);
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


void Form::on_btn_makesure_clicked()
{
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
        modify_message += "$M_OffTime:" + ui->modify_extingguishtime->text() + "$";
    }

    if(modify_message != "modify_env+")
    {
        modify_message += "\r\n";
        emit senddata(modify_message);
    }

    ui->modify_temp_H->clear();
    ui->modify_temp_L->clear();
    ui->modify_preesure_L->clear();
    ui->modify_extingguishtime->clear();
}

