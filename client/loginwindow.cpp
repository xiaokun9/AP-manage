#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"

#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
HHOOK  keyHook = NULL;
HHOOK  mouseHook =NULL;
//备份

/*
 * https://blog.csdn.net/lms1008611/article/details/81271712
 * qt sqlite生成db文件
*/

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/Tonly.ico"));
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
    this->setFixedSize(360,280);
    //this->ui->statusBar->showMessage("hello",3000);

    QLabel *permanent=new QLabel(this);
    permanent->setFrameStyle(QFrame::NoFrame|QFrame::Sunken);

    permanent->setText(tr("<a href=\"mailto:yukun.qin@tcl.com?subject=AP 管理软件 BUG&body=请详细说明\">BUG反馈请点击此处</a>"));
    permanent->setOpenExternalLinks(true);//设置可以打开网站链接

    QLabel *temp=new QLabel("<h2><font color=red>欢迎使用</font></h2>");
    temp->setFixedSize(240,25);
    ui->statusBar->addPermanentWidget(temp);
    ui->statusBar->addPermanentWidget(permanent);//显示永久信息

    ui->lineEdit_2->installEventFilter(this);//安装事件过滤器
    ui->pushButton->installEventFilter(this);
    ui->lineEdit->setFocus();
    //////////////////////////////////
//    //测试csv 文件
//    QString fileName = QFileDialog::getSaveFileName(this,tr("保存 文件"),
//                               qApp->applicationDirPath (),
//                                tr("Files (*.csv)"));
//    /*
//     * QFileDialog::getOpenFileName(this, tr("Excel file"), qApp->applicationDirPath (),

//                                                   tr("Files (*.csv)"));
//    */
//    if(fileName.isEmpty()) return;
//    //打开csv 文件
//    QFile file(fileName);
//    if(!file.open(QIODevice::Append| QIODevice::Text))

//    {
//        QMessageBox::question(this, tr("错误"), tr("文件错误"), tr("确定"));
//        return;
//    }

//    QTextStream out(&file);
//    out << tr("工号,") << tr("开始时间,") << tr("结束时间,")<<",\n";//表头
//    QDateTime current_date_time =QDateTime::currentDateTime();
//    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
//    out << tr(" 11011430,") << tr("%1,").arg(current_date)<< tr("%1,").arg(current_date) <<",\n";
//    file.close();
//    qDebug()<<fileName;
    /////////////////////////////////////

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_pushButton_clicked()
{


    //this->ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    //win->user_name=ui->lineEdit->text();
    //qDebug()<<"user_name:"<<ui->lineEdit->text();
    //win->pass_wd=ui->lineEdit_2->text();

    //数据库操作
    log_allow=false;

    db = QSqlDatabase::addDatabase("QSQLITE");//a
//    db.setHostName("127.0.0.1");             //b
//    db.setDatabaseName("test");              //c 数据库名称
//    db.setUserName("root");                  //d
//    db.setPassword("root");                //e

    qDebug()<<QApplication::applicationDirPath() + "/config.db";
    QFileInfo sql_file(QApplication::applicationDirPath() + "/config.db");
    if(sql_file.isFile()==false)
    {
        QMessageBox::warning(NULL, "error","没有数据库文件，请联系管理员",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No);
        return;
    }
    else
    {
        db.setDatabaseName(QApplication::applicationDirPath() + "/config.db");
    }
    if(!db.open())
    {
        qDebug()<<"数据库打开失败";
        QMessageBox::warning(NULL, "error","数据库打开失败，请联系作者",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No);
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM main");
    query.exec();
    QString user_name;
    while(query.next()){
        //校验用户名
        //qDebug()<<query.value("user_id").toString();
        user_name_log=query.value("user_id").toString();
        qDebug()<<user_name_log;
        if(user_name_log==ui->lineEdit->text())
        {
            //校验密码
            //qDebug()<<query.value("user_passwd").toString();
            user_password_log=query.value("user_passwd").toString();
            qDebug()<<user_password_log;
            if(ui->lineEdit_2->text()==user_password_log)
            {
                qDebug()<<"校验成功";
                log_allow=true;
                user_name=query.value("user_name").toString();
                break;
            }
        }
    }
    db.close();

    if(log_allow)
    {
        //可以登录
        MainWindow *win=new MainWindow;
        win->user_name=user_name;
        win->pass_wd=user_password_log;
        win->user_id=user_name_log;
        win->ma_init();//初始化
        win->show();
        this->close();
    }
    else
    {
        QMessageBox::critical(NULL,"错误","请输入正确的账号密码",QMessageBox::Ok);
        return;
    }

}
//启用美式输入法
void LoginWindow::disableIME()
{
    hCurKL = GetKeyboardLayout(0);
    LoadKeyboardLayout((LPCWSTR)QString("0x0409").utf16(), KLF_ACTIVATE);
}
//启用原输入法
void LoginWindow::enableIME()
{
    ActivateKeyboardLayout(hCurKL, 0);
}

bool LoginWindow::eventFilter(QObject *watched,QEvent * event)
{
    if(watched==ui->lineEdit_2)
    {
        if (event->type()==QEvent::FocusIn)     //然后再判断控件的具体事件 (这里指获得焦点事件)
        {
            disableIME();
        }
        else if (event->type()==QEvent::FocusOut)    // 这里指 lineEdit1 控件的失去焦点事件
        {
            enableIME();
        }
        else if(event->type()==QEvent::KeyPress)
        {
            QKeyEvent *thekey=static_cast<QKeyEvent*>(event);
            if((thekey->key()==Qt::Key_Enter)||(thekey->key()==Qt::Key_Return))
            {
                emit ui->pushButton->clicked();
            }
        }
    }
    if(watched==ui->pushButton)
    {
        if(event->type()==QEvent::KeyPress)
        {
            QKeyEvent *thekey=static_cast<QKeyEvent*>(event);
            if((thekey->key()==Qt::Key_Enter)||(thekey->key()==Qt::Key_Return))
            {
                emit ui->pushButton->clicked();
            }
        }
    }
    return QMainWindow::eventFilter(watched,event);
}

void LoginWindow::setAutoStart(bool is_auto_start)
{
    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    if(is_auto_start)
    {
        QString application_path = QApplication::applicationFilePath();
        settings->setValue(application_name, application_path.replace("/", "\\"));
    }
    else
    {
        settings->remove(application_name);
    }
    delete settings;
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    if(mSysTrayIcon->isVisible())
    {
        //hide(); //隐藏窗口
        event->ignore(); //忽略事件
    }
}

/////////////////////////////
//键盘钩子过程
LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    //在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    //如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
    if(nCode == HC_ACTION)
    {
        if((pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL)& 0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)|| //"Ctrl+Shift+Esc"
            (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000) || //"Ctrl+Esc"
            (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN )|| //"Alt+Tab"
            (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN )|| //"Alt+Esc"
            (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN )||	// "LWIN/RWIN"
            (pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN )) //"Alt+F4"
        {
            return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
        }

        // 		if(pkbhs->vkCode == VK_F12)
        // 		{
        // 			void unHook();
        // 			qApp->quit();
        // 		}
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}
//鼠标钩子过程
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{

    return 1;

}

//安装钩子,调用该函数即安装钩子
void LoginWindow::hook(bool flag)
{
    if (flag)
    {
        //这两个底层钩子,不要DLL就可以全局
        //底层键盘钩子
        keyHook =SetWindowsHookEx( WH_KEYBOARD_LL,keyProc,GetModuleHandle(NULL),0);
        //底层鼠标钩子
        //    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
    }
    else
    {
        UnhookWindowsHookEx(keyHook);
        //  UnhookWindowsHookEx(mouseHook);
    }
}

//添加注册屏蔽Ctrl+Alt+del
void LoginWindow::enableTaskManager(bool flag)
{
    // 屏蔽ctrl + alt +del 需要修改注册表的值， 取得管理员权限， 关闭360等杀毒软件
    int value = flag ? 0x00000001 : 0x00000000;
    QSettings *settings = new QSettings(TASKMANAGERSystem, QSettings::NativeFormat);
    settings->setValue("DisableTaskMgr", value); //任务管理器
    settings->setValue("DisableChangePassword", value); //更改密码
    settings->setValue("DisableLockWorkstation", value); //锁定计算机
    settings->setValue("DisableSwitchUserOption", value); //切换用户

    QSettings *settings2 = new QSettings(TASKMANAGERExplorer, QSettings::NativeFormat);
    settings2->setValue("NoLogOff", value); //注销

    delete settings;
    delete settings2;
}

//屏蔽任务栏
void LoginWindow::showTaskWindow(bool flag)
{
    if(flag)
        ShowWindow(task,SW_SHOW);
    else
    {
        task=FindWindow(L"Shell_TrayWnd",NULL);
        ShowWindow(task,SW_HIDE);
    }
}
