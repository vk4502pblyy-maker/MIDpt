#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QStyleFactory>
#include "function/functionmovement.h"
#include "function/functionmovesl.h"
#include "function/funcpztctl.h"
#include "function/funcmovewm.h"


//#1D2129;
const QString industrialStyle = R"(
    /* 全局基础样式 */
    * {
        font-family: "Microsoft YaHei", "Consolas", "SimHei", sans-serif;
        font-size: 24px;
        color: #000000; /* 主要文本色 */
        background-color: #F5F5F5; /* 主背景 */
    }

    /* 主窗口和对话框 */
    QMainWindow, QDialog, QWidget#centralWidget {
        background-color: #F5F5F5;
        border: none;
    }

    /* 标题样式 - 主色调 */
    QLabel#mainTitle {
        font-size: 22px;
        font-weight: bold;
        color: #000000; /* 主色 */
        padding: 10px 0;
    }

    QLabel#sectionTitle {
        font-size: 24px;
        font-weight: bold;
        color: #000000;
        border-bottom: 1px solid #38404D; /* 次要分割线 */
        padding: 8px 0;
        margin-bottom: 10px;
    }

    /* 功能区域背景 */
    QFrame#functionArea, QGroupBox {
        background-color: #DCDCDC; /* 区域背景 */
        border: 2px solid #4E5969; /* 主要边框 */
        border-radius: 4px;
    }

    /* 数据卡片/面板 */
    QFrame#dataCard {
        background-color: #DCDCDC; /* 卡片背景 */
        border: 1px solid #4E5969; /* 主要边框 */
        border-radius: 4px;
        padding: 10px;
    }

    /* 次要文本 */
    QLabel#secondaryText {
        color: #000000; /* 次要文本色 */
        font-weight: bold;
        font-size: 24px;
    }

    /* 禁用文本 */
    QLabel:disabled, QPushButton:disabled, QLineEdit:disabled {
        color: #86909C; /* 禁用文本色 */
    }

    /* 状态指示 - 正常/运行中 */
    QLabel#statusNormal {
        color: #00B42A; /* 绿色 - 正常 */
        font-weight: bold;
    }

    /* 状态指示 - 错误/警报 */
    QLabel#statusError {
        color: #F53F3F; /* 红色 - 错误 */
        font-weight: bold;
    }

    /* 状态指示 - 警告/待处理 */
    QLabel#statusWarning {
        color: #FF7D00; /* 黄色 - 警告 */
        font-weight: bold;
    }

    /* 状态指示 - 手动模式 */
    QLabel#statusManual {
        color: #722ED1; /* 紫色 - 手动模式 */
        font-weight: bold;
    }

    /* 按钮样式 */
    QPushButton {
        background-color: #C0C0C0; /* 主色 - 按钮正常 */
        color: #000000;
        border: none;
        border-radius: 3px;
        padding: 6px 14px;
        font-size: 18px;
    }

    QPushButton:hover {
        background-color: #ADD8E6; /* 按钮悬停 */
        border: 2px solid #4E5969;
    }

    QPushButton:pressed {
        background-color: #0A2491; /* 按钮点击 */
    }

    QPushButton:disabled {
        background-color: #DCDCDC; /* 按钮禁用 */
    }

    /* 特殊功能按钮 */
    QPushButton#emergencyBtn {
        background-color: #F53F3F; /* 红色按钮 */
    }

    QPushButton#emergencyBtn:hover {
        background-color: #E02020;
    }

    QPushButton#confirmBtn {
        background-color: #00B42A; /* 绿色按钮 */
    }

    QPushButton#confirmBtn:hover {
        background-color: #009A22;
    }

    /* 输入控件 */
    QLineEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateTimeEdit {
        background-color: #DCDCDC; /* 输入框背景 */
        color: #000000; /* 输入框文本 */
        border: 1px solid #4E5969; /* 正常边框 */
        border-radius: 3px;
        padding: 6px;
        font-size: 18px;
    }

    QLineEdit:disabled, QTextEdit:disabled, QComboBox:disabled, QSpinBox:disabled, QDoubleSpinBox:disabled, QDateTimeEdit:disabled {
        background-color: #F5F5F5; /* 输入框背景 */
        color: #474747; /* 输入框文本 */
        border: 1px solid #4E5969; /* 正常边框 */
        border-radius: 3px;
        padding: 6px;
        font-size: 18px;
    }

    QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
        border: 1px solid #165DFF; /* 聚焦边框 - 主色 */
    }

    QLineEdit:error, QTextEdit:error {
        border: 1px solid #F53F3F; /* 错误边框 - 红色 */
    }

    /* QTabWidget 样式 */
    QTabWidget::pane {
        border: 0px solid #4E5969; /* 主要边框 */
        background-color: #DCDCDC; /* 区域背景 */
        buttom: 1px;
        margin: 0; /* 消除上下左右边距 */
        padding: 0; /* 消除内边距 */
        border-radius: 0;
    }

    QTabBar::tab {
        background-color: #DCDCDC; /* 区域背景 */
        color: #000000; /* 次要文本 */
        border: 1px solid #4E5969; /* 主要边框 */
        padding: 8px 16px;
        font-size: 18px;
        margin-right: 2px;
        border-top-left-radius: 4px;
        border-top-right-radius: 4px;
    }

    QTabBar::tab:selected {
        background-color: #C0C0C0; /* 主色 */
        color: #000000; /* 主要文本 */
        border-bottom-color: #C0C0C0;
        font-weight: bold;
    }

    QTabBar::tab:hover:!selected {
        background-color: #ADD8E6; /* 卡片背景 */
        color: #000000;
    }

    /* 表格和列表控件 */
    QTableView, QListView, QTreeView {
        background-color: #DCDCDC; /* 区域背景 */
        color: #000000; /* 主要文本 */
        border: 1px solid #4E5969; /* 主要边框 */
        alternate-background-color: #DCDCDC; /* 卡片背景 */
        font-size: 18px;
    }

    QHeaderView::section {
        background-color: #DCDCDC; /* 卡片背景 */
        color: #F2F3F5; /* 主要文本 */
        border: 1px solid #4E5969; /* 主要边框 */
        padding: 6px;
        font-weight: bold;
        text-align: center;
    }

    /* 滚动条 */
    QScrollBar:vertical {
        background-color: #272E3B; /* 区域背景 */
        width: 12px;
        margin: 0;
    }

    QScrollBar::handle:vertical {
        background-color: #4E5969; /* 主要边框色 */
        border-radius: 3px;
    }

    QScrollBar::handle:vertical:hover {
        background-color: #5E6A7A;
    }

    QScrollBar:horizontal {
        background-color: #272E3B; /* 区域背景 */
        height: 10px;
        margin: 0;
    }

    QScrollBar::handle:horizontal {
        background-color: #4E5969; /* 主要边框色 */
        border-radius: 3px;
    }

    QScrollBar::handle:horizontal:hover {
        background-color: #5E6A7A;
    }

    /* 分组框 */
    QGroupBox {
        margin-top: 12px;
        padding: 10px;
    }

    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        left: 10px;
        top: -8px;
        background-color: #272E3B; /* 区域背景 */
        padding: 0 5px;
        font-weight: bold;
    }

    /* 进度条 */
    QProgressBar {
        background-color: #DCDCDC; /* 卡片背景 */
        border: 1px solid #4E5969; /* 主要边框 */
        border-radius: 3px;
        text-align: center;
        color: #000000;
        height: 8px;
    }

    QProgressBar::chunk {
        background-color: #00D26A; /* 主色 */
        border-radius: 2px;
    }

    /* 仪表盘/进度条状态颜色 */
    QProgressBar#normalProgress::chunk {
        background-color: #00B42A; /* 正常范围 */
    }

    QProgressBar#warningProgress::chunk {
        background-color: #FF7D00; /* 警告范围 */
    }

    QProgressBar#dangerProgress::chunk {
        background-color: #F53F3F; /* 危险范围 */
    }

    /* 分割线 */
    QFrame#line {
        background-color: #38404D; /* 次要分割 */
        height: 1px;
    }
    QToolTip {
        /* 字体设置 */
        font-family: "Microsoft YaHei", "SimHei", sans-serif; /* 字体家族 */
        font-size: 18px;                                     /* 字号 */
        font-weight: normal;                                 /* 字重（normal/bold） */

        /* 颜色设置 */
        color: #000000;                                      /* 文本颜色（白色） */
        background-color: #ffffff;                           /* 背景色（深灰色） */
        border: 1px solid #555555;                           /* 边框颜色和宽度 */

        /* 其他样式（可选） */
        padding: 4px 8px;                                    /* 内边距（上下 左右） */
        border-radius: 3px;                                  /* 圆角半径 */
    }
    QListWidget::item {
        background-color: #DCDCDC; /* 与你界面中圈出的浅灰色接近 */
        color: #000000; /* 黑色文本，确保清晰 */
        padding: 4px 8px;
    }

    QMenuBar {
        background-color: #DCDCDC;
        width: 100%;
        font-size: 18px;           /* 字体大小 */
    }
    QMenu {
        font-size: 18px;           /* 字体大小 */
    }
}

)";

int main(int argc, char *argv[])
{
    //注册元对象
    qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<QImage>("QImage");

    //注册非独立线程单例
    FunctionMovement::getInstance();
    FunctionMoveSL::getInstance();
    FuncPZTCtl::getInstance();
    FuncMoveWM::getInstance();

    //注册独立线程单例


    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // 加载样式表
    QFile file("./styles/style-gbl-qyqx.qss"); // 或者使用绝对/相对路径
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&file);
        QString style = ts.readAll();
        a.setStyleSheet(style);
        file.close();
    } else {
        qDebug() << "无法打开样式表文件";
    }
//    a.setStyleSheet(industrialStyle);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    MainWindow w;
    qDebug()<<"MainWindow Thread: "<< w.thread();
    w.showMaximized();

    return a.exec();
}
