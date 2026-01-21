#include "widget.h" /* 包含Widget类的头文件：使用Widget类必须先包含 */
#include <QApplication> /* 包含Qt应用类头文件：QApplication是Qt程序的入口核心 */

int main(int argc, char *argv[])
{
    /* 创建QApplication对象：初始化Qt应用环境（自动创建） */
    /* 必须创建，且是第一个Qt对象（处理界面、事件循环等） */
    QApplication a(argc, argv);
    Widget w;   /* 创建Widget对象：实例化我们的串口助手窗口 */
    w.show();   /* 显示窗口：调用继承自QWidget的show()函数 */

    /* 启动Qt事件循环：等待用户操作（点击按钮、串口数据等） */
    /* 类似C语言的while(1)循环，但Qt自动处理事件 */
    return a.exec();
}
