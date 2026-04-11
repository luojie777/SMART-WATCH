QT       += core gui
QT += core gui bluetooth
QT += bluetooth
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    form.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    form.h \
    mainwindow.h

FORMS += \
    form.ui \
    mainwindow.ui


# Android specific configuration
android {
    # Android permissions - REQUIRED for BLE on Android
    ANDROID_PERMISSIONS = \
        android.permission.BLUETOOTH \
        android.permission.BLUETOOTH_ADMIN \
        android.permission.ACCESS_FINE_LOCATION \
        android.permission.ACCESS_COARSE_LOCATION \
        android.permission.BLUETOOTH_SCAN \
        android.permission.BLUETOOTH_CONNECT

    # Target SDK version - important for location permissions
    ANDROID_TARGET_SDK_VERSION = 31

    # For Android 12+ you need to explicitly request these permissions
    ANDROID_PERMISSIONS += \
        android.permission.BLUETOOTH_SCAN \
        android.permission.BLUETOOTH_CONNECT \
        android.permission.BLUETOOTH_ADVERTISE

    # 64-bit only
    ANDROID_ABIS = arm64-v8a

    # Enable QML debugging
    CONFIG += qtquickcompiler

    # Application name and package
    TARGET = BLE_Controller
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
