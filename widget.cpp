#include "widget.h"     /* 包含Widget头文件：实现类的成员函数必须包含 */
#include "ui_widget.h"  /* 包含自动生成的UI头文件（编译时由ui文件生成） */
#include <QSerialPortInfo>  /* 包含串口信息类头文件：用于枚举可用串口 */
#include <QMessageBox>  /* 包含消息框类头文件：用于弹出提示框 */
#include <QRegularExpression>   /* 包含正则表达式类头文件：用于过滤串口数据中的控制字符 */
#include <QTextCodec>   /* 包含编码转换类头文件：用于串口数据的编码转换 */
#include <QFont>    /* 包含字体类头文件：用于设置文本框字体 */

static QByteArray g_receiveBuffer = "";    /* 存放原始串口数据 */

/* 构造函数实现：Widget类的初始化逻辑 */
Widget::Widget(QWidget *parent) :
    /**
     * 调用父类构造函数
     *
     * 先确认地基和房子结构，QWidget(parent)就是告诉Qt
     * 我要盖一个房子，它的地基和规则要按QWidget类的标准来！
     */
    QWidget(parent),

    /**
     * 创建界面对象
     *
     * Ui::Widget是Qt自动生成的类，包含了所有界面元素
     * new Ui::Widget ：new执行后会返回Ui::Widget的地址
     * ui：指向Ui::Widget的所有元素
     */
    ui(new Ui::Widget)
{
    /**
     * 加载界面布局
     *
     * setupUi(this)的核心目的是：将当前Widget对象设为setupui的“父对象”。
     * 意义：如果一个对象有父对象，父对象销毁时会自动销毁所有子对象。
     *       即Widget窗口关闭时，自动释放内存。
     *
     * ui->setupUi(this)的目的：ui指向Ui::Widget，ui里的控件需调用setupUi，
     *                         放置在当前的Widget窗口（this）上。
     */
    ui->setupUi(this);

/*************************************************************************************/

    /**
     * 修改窗口标题：凡涉及界面内ui都需在ui(new Ui::Widget)处理
     *
     * void setWindowTitle(Widget* const this, QString title);
     */
    this->setWindowTitle("串口助手  由Kaydon制作");

/*************************************************************************************/

    /* 让串口下拉框显示串口 */
    /**
     * 1.搜索可用的串口（寻找地址）
     *
     * QStringList：Qt字符串列表类，替代C语言的char*数组
     * QStringList SerialPort_List; ：创建一个存储串口的列表。
     *
     * QSerialPort是Qt框架里封装好的一个类，new执行后会返回QSerialPort的地址。
     * new QSerialPort()是因为当函数执行完后会被自动销毁，但串口对象需要长期存在，
     * 所以必须用new在堆区创建。
     *
     * QSerialPort(this)核心目的是：将当前Widget对象设为这个串口对象的 “父对象”。
     * 意义：如果一个对象有父对象，父对象销毁时会自动销毁所有子对象。
     *      即Widget窗口关闭时，自动释放内存。
     *
     * 指针变量SerialPort在.h已声明，用于指向QSerialPort类实例对象的内存地址。
     *
     * SerialPort = new QSerialPort(this);的目的：
     * 让SerialPort重新指向堆区创建的QSerialPort类的实际实例对象的内存地址，
     * 同时将当前Widget（this）设为这个QSerialPort实例的父对象，由Qt自动管理其内存。
     */
    QStringList SerialPort_List;
    SerialPort = new QSerialPort(this);

    /**
     * 2.遍历可用串口
     *
     * foreach：Qt提供的遍历宏（本质是简化的for循环）
     * foreach (const QString &str, list)
     *
     * const QSerialPortInfo &temp_PortInfo
     * QSerialPortInfo：Qt的“串口信息类”，封装了串口的名称、厂商、描述等信息（相当于模板）
     * &temp_PortInfo：temp_PortInfo用于以QSerialPortInfo为模板，绑定每一个串口的信息（如COM3)
     * QSerialPortInfo::availablePorts()：获取所有可用串口信息
     */
    foreach (const QSerialPortInfo &temp_PortInfo, QSerialPortInfo::availablePorts()) {
        /* 将搜索到的每个串口名称添加到串口列表 .portName()只提取串口名 */
        SerialPort_List << temp_PortInfo.portName();
    }

    /**
     * 3.显示串口：将串口列表填充到下拉框中
     *
     * ui->Serial_Selection：访问ui对象中的Serial_Selection控件（下拉框）
     * addItems：下拉框的成员函数，批量添加选项
     *
     * ui->Serial_Selection->addItems(SerialPort_List);
     * 将串口列表添加到Serial_Selection的选项
     */
    ui->Serial_Selection->addItems(SerialPort_List);

/*************************************************************************************/

    /**
     * 设置接收文本框为等宽字体，保证日志对齐
     *
     * QFont：Qt字体类，Consolas是等宽字体，9是字号
     * void setFont(QFont("ttf_name", int size));
     */
    ui->Recieve_Edit->setFont(QFont("Consolas", 9));

/*************************************************************************************/

    /**
     * 当设备发出数据且可读时，显示到接受框中
     *
     * SerialPort的readyRead信号（有数据可读时触发）绑定到
     * 本类的serialPortReadyRead_Slot槽函数
     * 槽函数须在.h中的private slots进行声明。
     *
     * 手动链接信号与槽
     * bool QObject::connect(
     *     const QObject *sender,       // 信号发送者（如SerialPort）
     *     const char *signal,          // 要绑定的信号（如SIGNAL(readyRead())）
     *     const QObject *receiver,     // 信号接收者（如this，当前窗口）
     *     const char *method,          // 要触发的槽函数（如SLOT(SerialPortReadyRead_Slot())）
     *     Qt::ConnectionType type = Qt::AutoConnection  // 连接类型（默认自动）
     * );
     */
    connect(SerialPort, SIGNAL(readyRead()), this, SLOT(SerialPort_ReadyRead_Slot()));
}

Widget::~Widget()
{
    delete ui;  /* 释放内存 */
}

/* 点击打开串口按钮的槽函数 */
void Widget::on_Open_Button_clicked()
{
    /* Widget:: 说明on_Open_Button_clicked()是属于Widget的函数 */
    /* 需在.h文件声明槽函数 */

    /**
     * 1.初始化串口参数：防止打开空串口
     * QSerialPort::BaudRate：枚举类型（类似C的enum），表示波特率
     * QSerialPort::DataBits：表示数据位
     * QSerialPort::StopBits：表示停止位
     * QSerialPort::Parity：表示校验位
     * current_BaudRate：变量名，表示当前波特率（不建议与枚举名重复）
     *
     */
    QSerialPort::BaudRate current_BaudRate = QSerialPort::Baud115200;
    QSerialPort::DataBits current_DataBits = QSerialPort::Data8;
    QSerialPort::StopBits current_StopBits = QSerialPort::OneStop;
    QSerialPort::Parity current_Parity = QSerialPort::NoParity;

    /**
     * 2.收集选中的串口的不同属性（波特率、数据位等）
     *
     * 2.1 收集选择后波特率的信息
     *
     * QString QComboBox::currentText() const;
     * 返回值：QString类型，当前选中项的文本内容。若未选中任何项，返回空字符串
     * 参数：无。
     * 调用方式：通过QComboBox对象直接调用
     *
     * currentText()：获取下拉框当前选中的文本
     * ui->BaudRate_Selection：ui中有一个名为BaudRate_selection的下拉框（QComboBox）
     *
     * QString selected_BaudRate = ui->BaudRate_Selection->currentText();
     * 定义一个名为selected_BaudRate的字符串类的变量，指向ui中有一个名为BaudRate_Selection的下拉框，获取选中的文本
     *
     */
    QString selected_BaudRate = ui->BaudRate_Selection->currentText();
    if (selected_BaudRate == "4800") {  /* 选择4800波特率 */
        current_BaudRate = QSerialPort::Baud4800;
    } else if (selected_BaudRate == "9600") {
        current_BaudRate = QSerialPort::Baud9600;
    } else if (selected_BaudRate == "19200") {
        current_BaudRate = QSerialPort::Baud19200;
    } else if (selected_BaudRate == "38400") {
        current_BaudRate = QSerialPort::Baud38400;
    } else if (selected_BaudRate == "57600") {
        current_BaudRate = QSerialPort::Baud57600;
    } else if (selected_BaudRate == "115200") {
        current_BaudRate = QSerialPort::Baud115200;
    } /* 如有必要可以自行添加更多，记得设计界面也需新增 */

    /* 2.2 收集选择后的数据位信息 */
    QString selected_Databits = ui->DataBit_Selection->currentText();
    if (selected_Databits == "5") {
        current_DataBits = QSerialPort::Data5;
    } else if (selected_Databits == "6") {
        current_DataBits = QSerialPort::Data6;
    } else if (selected_Databits == "7") {
        current_DataBits = QSerialPort::Data7;
    } else if (selected_Databits == "8") {
        current_DataBits = QSerialPort::Data8;
    }

    /* 2.3 收集选择后的停止位信息 */
    QString selected_StopBits = ui->StopBit_Selection->currentText();
    if (selected_StopBits == "1") {
        current_StopBits = QSerialPort::OneStop;
    } else if (selected_StopBits == "1.5") {
        current_StopBits = QSerialPort::OneAndHalfStop;
    } else if (selected_StopBits == "2") {
        current_StopBits = QSerialPort::TwoStop;
    }

    /* 2.4 收集选择后校验位的信息 */
    QString selected_Parity = ui->Parity_Selection->currentText();
    if (selected_Parity == "NONE") {
        current_Parity = QSerialPort::NoParity;
    } else if (selected_Parity == "ODD") {
        current_Parity = QSerialPort::OddParity;
    } else if (selected_Parity == "EVEN") {
        current_Parity = QSerialPort::EvenParity;
    }

    /**
     * 3.点击打开按钮的瞬间，将收集的串口信息进行设置
     *
     * SerialPort->setPortName(ui->Serial_Selection->currentText());
     * void setPortName(const Qstring &name);   设置串口名称
     * void baudRateChanged(qint32 baudRate, QSerialPort::Directions directions)
     * ......
     */
    SerialPort->setPortName(ui->Serial_Selection->currentText());
    SerialPort->setBaudRate(current_BaudRate);
    SerialPort->setDataBits(current_DataBits);
    SerialPort->setStopBits(current_StopBits);
    SerialPort->setParity(current_Parity);

    /**
     * 4.打开串口
     *
     * QIODevice::ReadWrite表示读写模式
     *
     * open：返回bool值，表示是否打开成功
     */
    if (SerialPort->open(QIODevice::ReadWrite) == true) {
        /**
         * 清空接受框内容
         * void QTextEdit::clear();
         * 所属类：QTextEdit
         * 返回值：void
         * 参数：无参数
         * 删除控件内的所有文本（包括普通文本、换行符、富文本格式），恢复到控件初始化的空白状态；
         */

        /* 打开成功后清空缓冲区和显示框 */
        g_receiveBuffer.clear();  /* clear：清空字符数组 */
        ui->Recieve_Edit->clear();

        /**
         * 弹出信息提示框
         * [static] void QMessageBox::information(
         *      QWidget *parent,
         *      const QString &title,
         *      const QString &text
         * );
         */
        QMessageBox::information(this, "提示", "串口打开成功!");
    } else {
        /**
         * 打开失败的情况：
         * 当串口打开超时（比如串口被占用、设备未响应、波特率不匹配等），
         * SerialPort->open()会返回false，
         * 此时SerialPort->errorString()会读取到操作系统返回的错误描述
         * QMessageBox::critical弹出错误窗口
         */
        QMessageBox::critical(this, "提示", "串口打开失败：" + SerialPort->errorString());
    }
}

/*  点击关闭串口按钮的槽函数 */
void Widget::on_Close_Button_clicked()
{
    /* close：关闭串口 */
    SerialPort->close();

    /* 关闭串口时清空缓冲区 */
    g_receiveBuffer.clear();
    QMessageBox::information(this, "提示", "串口已关闭！");
}

/* 点击发送按钮的槽函数 */
void Widget::on_Send_Button_clicked()
{
    /* isOpen：判断串口是否打开 */
    if (SerialPort->isOpen()) {
        /**
         * 1.处理发送框的信息
         *
         * toPlainText()：获取发送文本框的内容
         *
         * 嵌入式设备通常以 “回车换行” 作为一条指令的结束标记
         * 设置字符串类的发送文本变量，获取文本内容并添加回车换行
         */
        QString SendText = ui->Send_Edit->toPlainText() + "\r\n";

        /**
         * 2.发送数据
         *
         * 根据UTF-8编码转换为串口通信所需的二进制字节数组，并通过串口发送出去
         * QByteArray QString::toUtf8() const;
         * qint64 QSerialPort::write(const QByteArray &data);
         */
        QByteArray sendData = SendText.toUtf8();
        SerialPort->write(sendData);

    } else {
        /* 会展示警告图标 */
        QMessageBox::warning(this, "提示", "请先打开串口！");
    }
}

/* 点击清空接收框按钮的槽函数 */
void Widget::on_Clear_Button_clicked()
{
    /* clear：清空接收文本框 */
    ui->Recieve_Edit->clear();

    /* 清空显示框时同时清空缓冲区 */
    g_receiveBuffer.clear();
}

/* 串口数据接收事件的槽函数 */
void Widget::SerialPort_ReadyRead_Slot()
{
    /* 1. 读取原始字节数据，拼入原有缓冲区 */
    g_receiveBuffer += SerialPort->readAll();
    if (g_receiveBuffer.isEmpty()) return;

    /* 2. 按换行符分割完整行（基于字节流，避免UTF-8截断） */
    int newlinePos;
    while ((newlinePos = g_receiveBuffer.indexOf('\n')) != -1) {
        /* 提取完整行的原始字节 */
        QByteArray lineBytes = g_receiveBuffer.left(newlinePos + 1);
        /* 移除已处理的字节，更新缓冲区 */
        g_receiveBuffer = g_receiveBuffer.mid(newlinePos + 1);

        /* 3. 对完整行解码（UTF-8），避免截断乱码 */
        QString line = QString::fromUtf8(lineBytes.constData(), lineBytes.size());

        /* 4. 过滤控制字符 */
        QRegularExpression ctrlRegex(
            R"(\x1B\[[0-9;]*[a-zA-Z]|\x1B[0-9A-Za-z]|)",
            QRegularExpression::CaseInsensitiveOption
        );
        QString cleanLine = line.replace(ctrlRegex, "").trimmed();

        /* 5. 显示非空行并自动滚动 */
        if (!cleanLine.isEmpty()) {
            ui->Recieve_Edit->appendPlainText(cleanLine);
            QTextCursor cursor = ui->Recieve_Edit->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->Recieve_Edit->setTextCursor(cursor);
        }
    }
}

/* 头像被点击的槽函数 */
void Widget::on_pushButton_clicked()
{
    QMessageBox::information(this, "求求你关注一下我~", "拜托啦~\nGithub:https://github.com/Liang-Kaidong/");
    QMessageBox::information(this, "更新日志", "1.修复了中文输入乱码的问题\n2.强制使用UTF-8编码格式\n3.新增关闭串口的消息提醒");
}
