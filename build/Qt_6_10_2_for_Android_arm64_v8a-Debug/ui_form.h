/********************************************************************************
** Form generated from reading UI file 'form.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_H
#define UI_FORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QLabel *label;
    QLabel *current_watch_time;
    QLabel *label_3;
    QLabel *beijingtime;
    QPushButton *btn_checktime;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QPushButton *btn_makesure;
    QLabel *current_watch_temp;
    QLabel *current_watch_pressure;
    QLabel *current_watch_high;
    QLineEdit *modify_temp_H;
    QLineEdit *modify_temp_L;
    QLineEdit *modify_preesure_L;
    QLabel *label_2;
    QLabel *current_watch_extingguishtime;
    QLabel *label_11;
    QLineEdit *modify_extingguishtime;
    QLabel *L_pre;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *H_temp;
    QLabel *L_temp;
    QLabel *label_10;
    QLabel *current_watch_temp2;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName("Form");
        Form->resize(610, 789);
        Form->setStyleSheet(QString::fromUtf8("background-color: rgb(190, 255, 253);"));
        label = new QLabel(Form);
        label->setObjectName("label");
        label->setGeometry(QRect(34, 62, 61, 16));
        current_watch_time = new QLabel(Form);
        current_watch_time->setObjectName("current_watch_time");
        current_watch_time->setGeometry(QRect(100, 60, 131, 20));
        label_3 = new QLabel(Form);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(34, 44, 61, 16));
        beijingtime = new QLabel(Form);
        beijingtime->setObjectName("beijingtime");
        beijingtime->setGeometry(QRect(100, 40, 131, 20));
        btn_checktime = new QPushButton(Form);
        btn_checktime->setObjectName("btn_checktime");
        btn_checktime->setGeometry(QRect(240, 40, 61, 41));
        btn_checktime->setMinimumSize(QSize(0, 0));
        btn_checktime->setMaximumSize(QSize(70, 16777215));
        label_4 = new QLabel(Form);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(30, 100, 61, 16));
        label_5 = new QLabel(Form);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(43, 230, 41, 16));
        label_6 = new QLabel(Form);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(90, 290, 41, 16));
        label_6->setMaximumSize(QSize(50, 16777215));
        label_7 = new QLabel(Form);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(170, 130, 101, 16));
        label_8 = new QLabel(Form);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(44, 260, 111, 16));
        label_9 = new QLabel(Form);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(170, 160, 101, 16));
        btn_makesure = new QPushButton(Form);
        btn_makesure->setObjectName("btn_makesure");
        btn_makesure->setGeometry(QRect(120, 440, 126, 32));
        QFont font;
        font.setPointSize(20);
        btn_makesure->setFont(font);
        current_watch_temp = new QLabel(Form);
        current_watch_temp->setObjectName("current_watch_temp");
        current_watch_temp->setGeometry(QRect(100, 100, 51, 16));
        current_watch_pressure = new QLabel(Form);
        current_watch_pressure->setObjectName("current_watch_pressure");
        current_watch_pressure->setGeometry(QRect(90, 230, 81, 21));
        current_watch_high = new QLabel(Form);
        current_watch_high->setObjectName("current_watch_high");
        current_watch_high->setGeometry(QRect(160, 290, 51, 16));
        modify_temp_H = new QLineEdit(Form);
        modify_temp_H->setObjectName("modify_temp_H");
        modify_temp_H->setGeometry(QRect(270, 130, 81, 19));
        modify_temp_L = new QLineEdit(Form);
        modify_temp_L->setObjectName("modify_temp_L");
        modify_temp_L->setGeometry(QRect(270, 160, 81, 19));
        modify_preesure_L = new QLineEdit(Form);
        modify_preesure_L->setObjectName("modify_preesure_L");
        modify_preesure_L->setGeometry(QRect(190, 260, 114, 19));
        modify_preesure_L->setMinimumSize(QSize(50, 0));
        label_2 = new QLabel(Form);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(71, 370, 121, 20));
        current_watch_extingguishtime = new QLabel(Form);
        current_watch_extingguishtime->setObjectName("current_watch_extingguishtime");
        current_watch_extingguishtime->setGeometry(QRect(200, 370, 81, 21));
        label_11 = new QLabel(Form);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(55, 400, 121, 16));
        modify_extingguishtime = new QLineEdit(Form);
        modify_extingguishtime->setObjectName("modify_extingguishtime");
        modify_extingguishtime->setGeometry(QRect(190, 400, 114, 19));
        L_pre = new QLabel(Form);
        L_pre->setObjectName("L_pre");
        L_pre->setGeometry(QRect(290, 230, 51, 16));
        label_12 = new QLabel(Form);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(190, 230, 91, 16));
        label_13 = new QLabel(Form);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(30, 130, 61, 16));
        label_14 = new QLabel(Form);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(30, 160, 61, 16));
        H_temp = new QLabel(Form);
        H_temp->setObjectName("H_temp");
        H_temp->setGeometry(QRect(100, 130, 61, 21));
        L_temp = new QLabel(Form);
        L_temp->setObjectName("L_temp");
        L_temp->setGeometry(QRect(100, 160, 61, 16));
        label_10 = new QLabel(Form);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(170, 100, 81, 16));
        label_10->setMaximumSize(QSize(99999, 16777215));
        current_watch_temp2 = new QLabel(Form);
        current_watch_temp2->setObjectName("current_watch_temp2");
        current_watch_temp2->setGeometry(QRect(280, 100, 71, 21));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "Form", nullptr));
        label->setText(QCoreApplication::translate("Form", "\346\211\213\350\241\250\346\227\266\351\227\264\357\274\232", nullptr));
        current_watch_time->setText(QCoreApplication::translate("Form", "0000-00-00  00:00:00", nullptr));
        label_3->setText(QCoreApplication::translate("Form", "\345\214\227\344\272\254\346\227\266\351\227\264\357\274\232", nullptr));
        beijingtime->setText(QCoreApplication::translate("Form", "0000-00-00  00:00:00", nullptr));
        btn_checktime->setText(QCoreApplication::translate("Form", "\346\227\266\351\227\264\346\240\241\345\207\206", nullptr));
        label_4->setText(QCoreApplication::translate("Form", "\347\216\257\345\242\203\346\270\251\345\272\246\357\274\232", nullptr));
        label_5->setText(QCoreApplication::translate("Form", "\346\260\224\345\216\213\357\274\232", nullptr));
        label_6->setText(QCoreApplication::translate("Form", "\351\253\230\345\272\246\357\274\232", nullptr));
        label_7->setText(QCoreApplication::translate("Form", "\350\256\276\347\275\256\351\253\230\346\270\251\346\217\220\351\206\222\357\274\232", nullptr));
        label_8->setText(QCoreApplication::translate("Form", "\350\256\276\347\275\256\344\275\216\346\260\224\345\216\213\346\217\220\351\206\222\357\274\232", nullptr));
        label_9->setText(QCoreApplication::translate("Form", "\350\256\276\347\275\256\344\275\216\346\270\251\346\217\220\351\206\222\357\274\232", nullptr));
        btn_makesure->setText(QCoreApplication::translate("Form", "\347\241\256\345\256\232\346\233\264\346\224\271\351\205\215\347\275\256", nullptr));
        current_watch_temp->setText(QCoreApplication::translate("Form", "36.5", nullptr));
        current_watch_pressure->setText(QCoreApplication::translate("Form", "10000pa", nullptr));
        current_watch_high->setText(QCoreApplication::translate("Form", "100m", nullptr));
        modify_temp_H->setText(QString());
        modify_temp_L->setText(QString());
        label_2->setText(QCoreApplication::translate("Form", "\345\275\223\345\211\215\350\207\252\345\212\250\346\201\257\345\261\217\346\227\266\351\227\264\357\274\232", nullptr));
        current_watch_extingguishtime->setText(QCoreApplication::translate("Form", "30s", nullptr));
        label_11->setText(QCoreApplication::translate("Form", "\350\256\276\347\275\256\350\207\252\345\212\250\346\201\257\345\261\217\346\227\266\351\227\264\357\274\232", nullptr));
        L_pre->setText(QCoreApplication::translate("Form", "90000", nullptr));
        label_12->setText(QCoreApplication::translate("Form", "\344\275\216\346\260\224\345\216\213\346\217\220\351\206\222\357\274\232", nullptr));
        label_13->setText(QCoreApplication::translate("Form", "\351\253\230\346\270\251\346\217\220\351\206\222\357\274\232", nullptr));
        label_14->setText(QCoreApplication::translate("Form", "\344\275\216\346\270\251\346\217\220\351\206\222\357\274\232", nullptr));
        H_temp->setText(QCoreApplication::translate("Form", "50", nullptr));
        L_temp->setText(QCoreApplication::translate("Form", "50", nullptr));
        label_10->setText(QCoreApplication::translate("Form", "\344\275\223\350\241\250\346\270\251\345\272\246\357\274\232", nullptr));
        current_watch_temp2->setText(QCoreApplication::translate("Form", "36.5", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_H
