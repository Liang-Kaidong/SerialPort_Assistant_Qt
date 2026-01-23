# SerialPort_Assistant_Qt
## 一、项目概述与设计核心目的
本项目基于Qt框架开发跨平台串口助手工具，支持串口枚举、参数配置、数据收发、交互提示等核心功能，适配Windows、Linux、macOS多系统。设计核心围绕四大目标展开，确保程序稳定性、兼容性与易用性：
1.  **内存安全管控**：堆区创建`QSerialPort`、UI控件等核心对象，绑定主窗口`Widget`为父对象，依托Qt父子对象机制自动释放内存，从根源规避内存泄漏问题。
2.  **跨平台统一适配**：通过Qt封装的`QSerialPort`类抽象串口操作，屏蔽不同系统原生串口编程差异，实现“一套代码多端运行”。
3.  **数据传输可靠**：采用“全局缓冲区缓存+按行解码+UTF-8强制编码+正则过滤”方案，彻底解决串口数据截断、中文乱码及无效控制字符干扰问题。
4.  **用户交互友好**：自动枚举可用串口填充下拉框、操作结果弹窗提示、接收框配置等宽字体并自动滚动，最大化降低用户操作门槛。

## 二、核心类与API详解（按功能分层）
本章按“基础支撑层-功能实现层”分层梳理核心资源，每类下细分具体子功能，每个类/API均以“中文功能标题”引领，配套原型、实战例子、必引头文件，兼顾可读性与实用性。

### 2.1 基础支撑层（核心业务类/全局变量）
该层级为程序运行基础，提供核心对象容器、数据缓存等支撑能力。

#### 2.1.1 全局接收缓冲区
- **功能标题**：串口原始数据缓存容器
- **原型**：`static QByteArray g_receiveBuffer = "";`
- **功能解释**：全局静态字节数组，用于缓存串口单次未读取完整的原始字节数据，避免UTF-8解码时因数据截断导致乱码，保障数据完整性。
- **实战例子**：`g_receiveBuffer.append(SerialPort->readAll());`
- **必须引入的头文件**：`<QByteArray>`（Qt核心模块，可随其他头文件间接引入）
- **额外配置**：无

#### 2.1.2 主窗口类（Widget）
- **功能标题**：程序主容器与逻辑封装载体
- **原型**：`class Widget : public QWidget`
- **功能解释**：串口助手主窗口类，封装所有UI交互逻辑和串口业务逻辑，作为所有子对象的父容器，依托Qt父子机制自动管理子对象生命周期。
- **实战例子**：`Widget::Widget(QWidget *parent) : QWidget(parent) {}`
- **必须引入的头文件**：`<QWidget>`
- **额外配置**：项目.pro文件需添加`QT += widgets`

#### 2.1.3 串口操作核心对象（QSerialPort）
- **功能标题**：串口全流程操作核心载体
- **原型**：`QSerialPort *SerialPort = new QSerialPort(this);`
- **功能解释**：统一管理串口的打开/关闭、参数配置、数据收发等所有串口相关操作，是串口通信功能的核心入口。
- **实战例子**：`SerialPort = new QSerialPort(this);`
- **必须引入的头文件**：`<QSerialPort>`
- **额外配置**：项目.pro文件需添加`QT += serialport`

### 2.2 功能实现层（高频Qt API）
该层级为程序核心功能实现载体，按“串口操作、交互提示、UI控件、数据处理、信号槽”五大类细分，覆盖所有业务场景。

#### 2.2.1 串口操作类（核心业务API）
负责串口枚举、启停、参数配置及数据收发，是串口助手的核心功能集合。

##### 2.2.1.1 串口枚举
###### API1：枚举系统可用串口
- **功能标题**：获取系统所有可用串口列表
- **原型**：`QList<QSerialPortInfo> QSerialPortInfo::availablePorts();`
- **功能解释**：扫描当前系统中已连接的串口设备，返回包含所有串口信息的列表，为串口选择提供数据支撑。
- **实战例子**：`QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();`
- **必须引入的头文件**：`<QSerialPortInfo>`

###### API2：提取串口名称
- **功能标题**：从串口信息中获取设备名称
- **原型**：`QString QSerialPortInfo::portName() const;`
- **功能解释**：从串口信息对象中提取串口设备名称（如Windows下的COM3、Linux下的/dev/ttyUSB0），用于填充串口选择下拉框。
- **实战例子**：`QString portName = ports.at(i).portName();`
- **必须引入的头文件**：`<QSerialPortInfo>`

##### 2.2.1.2 串口启停与参数配置
###### API1：打开串口
- **功能标题**：以指定模式启动串口
- **原型**：`bool QSerialPort::open(QIODevice::OpenMode mode);`
- **功能解释**：按指定模式（读写/只读/只写）打开串口，返回布尔值标识打开结果，是串口通信的前置操作。
- **实战例子**：`SerialPort->open(QIODevice::ReadWrite);`
- **必须引入的头文件**：`<QSerialPort>`

###### API2：关闭串口
- **功能标题**：停止串口通信并释放资源
- **原型**：`void QSerialPort::close();`
- **功能解释**：关闭已打开的串口，释放串口占用的系统资源，避免资源泄露。
- **实战例子**：`SerialPort->close();`
- **必须引入的头文件**：`<QSerialPort>`

###### API3：设置串口名称
- **功能标题**：指定待操作的串口设备
- **原型**：`void QSerialPort::setPortName(const QString &name);`
- **功能解释**：绑定要操作的串口设备名称，需在打开串口前配置，确保操作目标正确。
- **实战例子**：`SerialPort->setPortName("COM3");`
- **必须引入的头文件**：`<QSerialPort>`

###### API4：设置波特率
- **功能标题**：配置串口通信速率
- **原型**：`void QSerialPort::setBaudRate(QSerialPort::BaudRate baudRate);`
- **功能解释**：设置串口数据传输速率（如9600、115200），需与外接设备波特率保持一致，否则无法正常通信。
- **实战例子**：`SerialPort->setBaudRate(QSerialPort::Baud115200);`
- **必须引入的头文件**：`<QSerialPort>`

###### API5：设置数据位
- **功能标题**：配置单帧数据的有效位数
- **原型**：`void QSerialPort::setDataBits(QSerialPort::DataBits dataBits);`
- **功能解释**：设置串口通信中单帧数据的有效位数（5/6/7/8位），是串口参数的核心配置项之一。
- **实战例子**：`SerialPort->setDataBits(QSerialPort::Data8);`
- **必须引入的头文件**：`<QSerialPort>`

###### API6：设置停止位
- **功能标题**：配置单帧数据的结束标识位
- **原型**：`void QSerialPort::setStopBits(QSerialPort::StopBits stopBits);`
- **功能解释**：设置用于标识单帧数据结束的停止位（1/1.5/2位），确保接收方正确识别数据边界。
- **实战例子**：`SerialPort->setStopBits(QSerialPort::OneStop);`
- **必须引入的头文件**：`<QSerialPort>`

###### API7：设置校验位
- **功能标题**：配置数据传输错误检测机制
- **原型**：`void QSerialPort::setParity(QSerialPort::Parity parity);`
- **功能解释**：设置串口通信的校验方式（无校验/奇校验/偶校验），用于检测数据传输过程中的错误。
- **实战例子**：`SerialPort->setParity(QSerialPort::NoParity);`
- **必须引入的头文件**：`<QSerialPort>`

##### 2.2.1.3 串口数据收发
###### API1：读取串口数据
- **功能标题**：获取串口接收缓冲区数据
- **原型**：`QByteArray QSerialPort::readAll();`
- **功能解释**：读取串口接收缓冲区中所有可用的原始字节数据，是串口接收逻辑的核心接口。
- **实战例子**：`QByteArray data = SerialPort->readAll();`
- **必须引入的头文件**：`<QSerialPort>`

###### API2：发送串口数据
- **功能标题**：向串口发送数据
- **原型**：`qint64 QSerialPort::write(const QByteArray &data);`
- **功能解释**：向串口发送指定的字节数组，返回实际发送的字节数，支持文本、二进制等任意格式数据传输。
- **实战例子**：`SerialPort->write("hello\r\n".toUtf8());`
- **必须引入的头文件**：`<QSerialPort>`

###### API3：判断串口状态
- **功能标题**：校验串口是否处于打开状态
- **原型**：`bool QSerialPort::isOpen() const;`
- **功能解释**：检查串口当前是否处于打开状态，用于发送数据前的合法性校验，避免无效操作。
- **实战例子**：`if(!SerialPort->isOpen()) { QMessageBox::warning(this, "警告", "请先打开串口！"); }`
- **必须引入的头文件**：`<QSerialPort>`

###### API4：获取串口错误信息
- **功能标题**：获取串口操作错误描述
- **原型**：`QString QSerialPort::errorString() const;`
- **功能解释**：获取串口操作过程中的错误描述字符串，用于弹窗提示失败原因，便于问题排查。
- **实战例子**：`QString err = SerialPort->errorString();`
- **必须引入的头文件**：`<QSerialPort>`

#### 2.2.2 交互提示类（弹窗API）
负责程序操作结果的可视化提示，提升用户交互体验，明确操作反馈。

###### API1：信息提示框
- **功能标题**：展示正常操作结果提示
- **原型**：`static void QMessageBox::information(QWidget *parent, const QString &title, const QString &text);`
- **功能解释**：弹出蓝色信息图标提示框，用于展示正常操作结果（如串口打开成功、数据发送完成）。
- **实战例子**：`QMessageBox::information(this, "成功", "串口打开成功！");`
- **必须引入的头文件**：`<QMessageBox>`

###### API2：错误提示框
- **功能标题**：展示严重错误信息
- **原型**：`static void QMessageBox::critical(QWidget *parent, const QString &title, const QString &text);`
- **功能解释**：弹出红色错误图标提示框，用于展示严重错误信息（如串口打开失败、设备连接异常）。
- **实战例子**：`QMessageBox::critical(this, "失败", SerialPort->errorString());`
- **必须引入的头文件**：`<QMessageBox>`

###### API3：警告提示框
- **功能标题**：提示用户注意潜在问题
- **原型**：`static void QMessageBox::warning(QWidget *parent, const QString &title, const QString &text);`
- **功能解释**：弹出黄色警告图标提示框，用于提示用户需注意的潜在问题（如未打开串口就发送数据、参数配置不匹配）。
- **实战例子**：`QMessageBox::warning(this, "警告", "请先打开串口！");`
- **必须引入的头文件**：`<QMessageBox>`

#### 2.2.3 UI控件操作类（界面交互API）
负责窗口、下拉框、文本框等UI控件的配置与交互，支撑程序可视化界面功能。

##### 2.2.3.1 窗口基础配置
###### API：设置窗口标题
- **功能标题**：配置程序窗口显示标题
- **原型**：`void QWidget::setWindowTitle(const QString &title);`
- **功能解释**：设置串口助手窗口的显示标题，提升界面辨识度。
- **实战例子**：`setWindowTitle("Qt串口助手");`
- **必须引入的头文件**：`<QWidget>`

##### 2.2.3.2 下拉框操作
###### API1：批量添加下拉框选项
- **功能标题**：批量填充下拉框选项
- **原型**：`void QComboBox::addItems(const QStringList &texts);`
- **功能解释**：将串口名称、波特率等选项批量填充到下拉框，简化UI初始化流程。
- **实战例子**：`ui->Serial_Selection->addItems(portNames);`
- **必须引入的头文件**：`<QComboBox>`

###### API2：获取下拉框选中内容
- **功能标题**：读取用户选中的下拉框选项
- **原型**：`QString QComboBox::currentText() const;`
- **功能解释**：读取下拉框当前选中项的文本内容，用于获取用户配置的串口参数（如波特率、串口名称）。
- **实战例子**：`QString baud = ui->Baud_Rate->currentText();`
- **必须引入的头文件**：`<QComboBox>`

##### 2.2.3.3 文本框操作
###### API1：设置文本框字体
- **功能标题**：配置文本框显示字体
- **原型**：`void QTextEdit::setFont(const QFont &font);`
- **功能解释**：配置文本框的显示字体（如等宽字体Consolas），保证串口日志对齐显示，提升可读性。
- **实战例子**：`ui->Recieve_Edit->setFont(QFont("Consolas", 9));`
- **必须引入的头文件**：`<QTextEdit>`、`<QFont>`

###### API2：清空文本框内容
- **功能标题**：清理文本框显示内容
- **原型**：`void QTextEdit::clear();`
- **功能解释**：清空接收/发送文本框的所有内容，支持用户手动清理界面，保持显示整洁。
- **实战例子**：`ui->Recieve_Edit->clear();`
- **必须引入的头文件**：`<QTextEdit>`

###### API3：读取文本框内容
- **功能标题**：获取文本框中的用户输入内容
- **原型**：`QString QTextEdit::toPlainText() const;`
- **功能解释**：获取文本框中的纯文本内容（无格式），用于读取用户要发送的串口数据。
- **实战例子**：`QString sendData = ui->Send_Edit->toPlainText();`
- **必须引入的头文件**：`<QTextEdit>`

###### API4：追加文本内容
- **功能标题**：向文本框末尾添加内容
- **原型**：`void QTextEdit::appendPlainText(const QString &text);`
- **功能解释**：向文本框末尾追加纯文本内容（自动换行），用于显示串口接收的解析后数据。
- **实战例子**：`ui->Recieve_Edit->appendPlainText(decodedData);`
- **必须引入的头文件**：`<QTextEdit>`

###### API5：移动文本光标
- **功能标题**：控制文本框光标位置
- **原型**：`void QTextCursor::movePosition(QTextCursor::MoveOperation op);`
- **功能解释**：移动文本框光标位置（如移至末尾），实现接收数据时自动滚动到最新内容，无需用户手动滑动。
- **实战例子**：`ui->Recieve_Edit->textCursor().movePosition(QTextCursor::End);`
- **必须引入的头文件**：`<QTextCursor>`

#### 2.2.4 数据处理类（编码/正则API）
负责串口数据的编码转换与无效字符过滤，保障数据显示正确性与整洁性。

##### 2.2.4.1 编码转换
###### API1：字符串转UTF-8字节数组
- **功能标题**：文本字符串编码适配串口传输
- **原型**：`QByteArray QString::toUtf8() const;`
- **功能解释**：将字符串转换为UTF-8编码的字节数组，适配串口通信的编码要求，解决中文乱码问题。
- **实战例子**：`QByteArray data = sendStr.toUtf8();`
- **必须引入的头文件**：`<QString>`

###### API2：UTF-8字节数组转字符串
- **功能标题**：串口字节数据解码为文本
- **原型**：`QString QString::fromUtf8(const char *data, int size);`
- **功能解释**：将指定长度的UTF-8字节数据转换为字符串，避免因数据不全导致的解码错误，保障文本显示完整。
- **实战例子**：`QString str = QString::fromUtf8(buffer.data(), buffer.size());`
- **必须引入的头文件**：`<QString>`

##### 2.2.4.2 正则过滤
###### API1：创建正则表达式
- **功能标题**：定义无效字符匹配规则
- **原型**：`QRegularExpression::QRegularExpression(const QString &pattern);`
- **功能解释**：构造正则表达式对象，定义要匹配的串口数据中的无效控制字符（如换行、退格、ASCII控制符）。
- **实战例子**：`QRegularExpression re("[\\x00-\\x1F\\x7F]");`
- **必须引入的头文件**：`<QRegularExpression>`

###### API2：正则替换字符串
- **功能标题**：过滤无效控制字符
- **原型**：`QString QString::replace(const QRegularExpression &re, const QString &after);`
- **功能解释**：按正则规则替换字符串中的匹配内容，将无效控制字符替换为空字符串，保证显示内容整洁。
- **实战例子**：`QString cleanStr = rawStr.replace(re, "");`
- **必须引入的头文件**：`<QString>`、`<QRegularExpression>`

#### 2.2.5 信号槽绑定类（事件联动API）
###### API：信号与槽函数绑定
- **功能标题**：实现事件与逻辑的联动
- **原型**：`bool QObject::connect(const QObject *sender, const char *signal, const QObject *receiver, const char *method, Qt::ConnectionType type = Qt::AutoConnection);`
- **功能解释**：手动关联信号发送者（如串口）和槽函数接收者（如数据处理函数），实现事件联动（如串口有数据可读时触发接收逻辑）。
- **实战例子**：`connect(SerialPort, SIGNAL(readyRead()), this, SLOT(SerialPort_ReadyRead_Slot()));`
- **必须引入的头文件**：`<QObject>`（Qt核心模块，所有Qt类默认继承）

## 三、项目依赖与配置说明
本章梳理项目运行所需的头文件、模块配置及核心设计要点，确保程序正常编译运行。

### 3.1 必选全局头文件（widget.cpp中引入）
以下头文件覆盖项目所有功能，需在实现文件中显式引入，避免编译报错，建议按功能分类组织：
```cpp
// 串口相关（核心业务）
#include <QSerialPort>
#include <QSerialPortInfo>

// UI控件相关（界面交互）
#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QFont>
#include <QTextCursor>

// 弹窗提示（交互反馈）
#include <QMessageBox>

// 编码/正则（数据处理）
#include <QString>
#include <QByteArray>
#include <QRegularExpression>

// 信号槽/核心对象（事件联动）
#include <QObject>
```

### 3.2 项目.pro文件配置（必须添加）
Qt项目需通过.pro文件配置依赖模块，以下配置为运行必要条件，缺一不可：
```bash
QT += core gui widgets serialport

# 源文件
SOURCES += \
    main.cpp \
    widget.cpp

# 头文件
HEADERS += \
    widget.h

# UI文件
FORMS += \
    widget.ui
```

**配置说明**：
- `QT += core gui`：Qt核心模块，默认包含，可省略；
- `QT += widgets`：支撑UI控件（QWidget、QComboBox等）功能，必须添加；
- `QT += serialport`：支撑串口操作（QSerialPort、QSerialPortInfo）功能，必须添加。

### 3.3 核心设计要点
1.  内存管理：堆区对象（如`SerialPort`）必须绑定父对象，依托Qt父子机制自动释放内存，无需手动调用`delete`，规避内存泄漏。
2.  数据完整性：串口接收逻辑依赖`QByteArray`缓冲区和`QString`的UTF-8转换，按行解码确保数据不截断、无乱码。
3.  信号槽联动：槽函数命名遵循“on_控件名_信号名”规则（如`on_Open_Button_clicked`），可自动关联UI信号，无需手动调用`connect`。
4.  跨平台适配：全程使用Qt封装API，不调用原生系统接口，确保程序在不同系统下表现一致。

## 四、总结
本项目以Qt框架为基础，通过合理的类与API设计，实现了功能完整、稳定可靠的跨平台串口助手。核心亮点在于“内存安全管控”“数据可靠传输”与“友好交互设计”，同时通过清晰的功能分层与详细的API说明，提升了代码的可读性与可维护性。

---

