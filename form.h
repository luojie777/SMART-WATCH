#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QKeyEvent> // 包含按键事件头文件
#include <QMessageBox> // 包含消息框头文件
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

protected:
    // 重写按键按下事件（核心：拦截返回键）
    void keyPressEvent(QKeyEvent *event) override;

signals:
    // 自定义信号：通知主界面返回（无参数，只需触发即可）
    void backToMainWindow();
    void senddata(QString sendText);
    void reconnectBluetooth();

public slots:
    void handlerdata(QString receivedText);
    void handleBluetoothDisconnected();

private slots:
    void on_btn_checktime_clicked();

    void on_btn_makesure_clicked();

private:
    Ui::Form *ui;
    void setBeijingTimeToUI();
};

#endif // FORM_H
