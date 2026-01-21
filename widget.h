#ifndef WIDGET_H    /* 防止头文件重复包含：C语言的#ifndef保护 */
#define WIDGET_H

#include <QWidget>  /* 包含Qt核心头文件：QWidget是所有窗口组件的基类 */
#include <QSerialPort>  /* 包含Qt串口类头文件：用于串口通信 */
#include <QString>  /* 包含Qt字符串类头文件：替代C语言的char* */

/* 命名空间：Qt设计师自动生成的UI代码放在这个命名空间下 */
namespace Ui {
class Widget;   /* 声明UI类，实际定义在ui_widget.h（编译时自动生成） */
}

class Widget : public QWidget   /* 定义Widget类：继承自QWidget（C++的继承特性） */
{
    Q_OBJECT    /* Qt的元对象宏：必须加！用于信号与槽机制 */
                /* 作用：让Qt能识别类的信号/槽函数，实现事件响应 */

public:
    /* 构造函数：创建Widget对象时调用 */
    /* explicit：防止隐式类型转换（C++语法） */
    /* parent = 0：默认参数（C++语法），表示父窗口为空（顶层窗口） */
    explicit Widget(QWidget *parent = 0);
    /* 析构函数：销毁Widget对象时调用，用于释放内存 */
    ~Widget();

private slots:  /* Qt的槽函数声明区：用于响应界面事件（按钮点击、串口数据接收），自动加上 */
    /* 打开串口按钮点击事件的槽函数 */
    void on_Open_Button_clicked();
    /* 关闭串口按钮点击事件的槽函数 */
    void on_Close_Button_clicked();
    /* 发送按钮点击事件的槽函数 */
    void on_Send_Button_clicked();
    /* 清空接收框按钮点击事件的槽函数 */
    void on_Clear_Button_clicked();
    /* 串口数据接收事件的槽函数 */
    void SerialPort_ReadyRead_Slot();
    /* 头像被点击的槽函数 */
    void on_pushButton_clicked();

private:
    /* ui相当于界面管家，可以指向Ui::Widget类的所有元素 */
    Ui::Widget *ui;

    /**
     * 声明串口指针变量
     *
     * 指针变量SerialPort用于指向QSerialPort类实例对象的内存地址，
     * 后续通过这个指针，能操作对应的串口通信对象，
     * 实现和物理串口（如COM3）的通信。
     */

    QSerialPort *SerialPort;
};

#endif /*  WIDGET_H 头文件保护结束 */
