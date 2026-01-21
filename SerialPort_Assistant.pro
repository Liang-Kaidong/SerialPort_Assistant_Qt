#-------------------------------------------------
#
# Project created by QtCreator 2026-01-20T14:33:40
#
#-------------------------------------------------

# 引入QT核心模块、图形界面模块、串口模块
# core：核心功能（字符串、容器等）； gui：图形界面基础； serialport：串口通信
QT       += core gui serialport

# 如果Qt版本大于4（Qt5/6），额外引入widgets模块（窗口组件）
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 生成的可执行文件名称
TARGET = SerialPort_Assistant

# 项目模板类型：app表示生成应用程序（区别于库文件lib）
TEMPLATE = app

# 定义编译宏：让编译器对Qt废弃的API发出警告
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 源文件列表：main.cpp（程序入口）、widget.cpp（窗口逻辑）
SOURCES += \
        main.cpp \
        widget.cpp

# 头文件列表：widget.h（窗口类声明）
HEADERS += \
        widget.h

# UI文件列表：widget.ui（Qt设计师绘制的界面文件）
FORMS += \
        widget.ui

# 程序图标文件路径（Windows系统）
RC_ICONS = .\Icons\SerialPort_Assistant.ico

RESOURCES += \
    Pics/pics.qrc
