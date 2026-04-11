/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_startScan;
    QPushButton *btn_stopScan;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_connect;
    QPushButton *btn_disconnect;
    QListWidget *list_devices;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLabel *status_labal;
    QPushButton *btn_watch;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *input_message;
    QPushButton *btn_send;
    QTextBrowser *text_log;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(383, 629);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 10, 341, 611));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");
        QFont font;
        font.setPointSize(21);
        font.setBold(true);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        btn_startScan = new QPushButton(widget);
        btn_startScan->setObjectName("btn_startScan");

        horizontalLayout->addWidget(btn_startScan);

        btn_stopScan = new QPushButton(widget);
        btn_stopScan->setObjectName("btn_stopScan");

        horizontalLayout->addWidget(btn_stopScan);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        btn_connect = new QPushButton(widget);
        btn_connect->setObjectName("btn_connect");

        horizontalLayout_2->addWidget(btn_connect);

        btn_disconnect = new QPushButton(widget);
        btn_disconnect->setObjectName("btn_disconnect");

        horizontalLayout_2->addWidget(btn_disconnect);


        verticalLayout->addLayout(horizontalLayout_2);

        list_devices = new QListWidget(widget);
        list_devices->setObjectName("list_devices");

        verticalLayout->addWidget(list_devices);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label = new QLabel(widget);
        label->setObjectName("label");
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        label->setFont(font1);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(label);

        status_labal = new QLabel(widget);
        status_labal->setObjectName("status_labal");

        horizontalLayout_3->addWidget(status_labal);

        btn_watch = new QPushButton(widget);
        btn_watch->setObjectName("btn_watch");

        horizontalLayout_3->addWidget(btn_watch);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        input_message = new QLineEdit(widget);
        input_message->setObjectName("input_message");

        horizontalLayout_4->addWidget(input_message);

        btn_send = new QPushButton(widget);
        btn_send->setObjectName("btn_send");

        horizontalLayout_4->addWidget(btn_send);


        verticalLayout->addLayout(horizontalLayout_4);

        text_log = new QTextBrowser(widget);
        text_log->setObjectName("text_log");

        verticalLayout->addWidget(text_log);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 383, 18));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "LUOJIE SMART_WATCH BLE", nullptr));
        btn_startScan->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\211\253\346\217\217", nullptr));
        btn_stopScan->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\346\211\253\346\217\217", nullptr));
        btn_connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        btn_disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\347\212\266\346\200\201:", nullptr));
        status_labal->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        btn_watch->setText(QCoreApplication::translate("MainWindow", "\350\277\233\345\205\245\346\211\213\350\241\250\346\216\247\345\210\266", nullptr));
        btn_send->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\266\210\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
