#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"

#define SHOW_EDGE_WIDTH 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setAcceptDrops(false);
    this->setWindowIcon(QIcon(":/res/Tonly.ico"));
    setAcceptDrops(true);
    this->setFixedSize(345,500);
    //QListWidgetItem *test=new QListWidgetItem();
    ui->listWidget->setViewMode(QListView::IconMode);

    ini=new QFileInfo("config.ini");
    if(ini->isFile()==false)
    {
        QFile file("config.ini");
        qDebug()<<"no ini file";
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    //ini file
    configIniWrite = new QSettings("config.ini", QSettings::IniFormat);
    configIniWrite->setIniCodec("UTF-8");

    //read old config ini
    QStringList old_ini=configIniWrite->allKeys();
    QFileIconProvider icon;
    foreach (QString key, old_ini) {
        QFileInfo fileInfo(configIniWrite->value(key).toString());
        QListWidgetItem *item = new QListWidgetItem(icon.icon(fileInfo),fileInfo.fileName());
        item->setSizeHint(QSize(80,60));
        ui->listWidget->addItem(item);

    }

    //menu set
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(connectFri(QListWidgetItem*)));
    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon Tray_icon = QIcon(":/res/Tonly.ico");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(Tray_icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QObject::trUtf8("AP 管理"));
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    //在系统托盘显示此对象
    mSysTrayIcon->show();
    //QDate::currentDate().toString("yyyy-MM-dd");
    //QXlsx::Document xlsx(qApp->applicationDirPath ()+"/1.xlsx");


    // 检查目录是否存在，若不存在则新建
    QString dir_str = qApp->applicationDirPath ()+"/"+"data";
    QDir dir;
    if (!dir.exists(dir_str))
    {
        dir.mkpath(dir_str);
    }


    Q_xlsx=new QXlsx::Document(qApp->applicationDirPath ()+"/"+"data/"+QDate::currentDate().toString("yyyy-MM-dd")+".xlsx");
    //QXlsx::Document xlsx;
    Q_xlsx->setColumnWidth(1,6,30);//设置单元格宽度
    QXlsx::Format format1;/*设置该单元的样式*/
    format1.setFontColor(QColor(Qt::red));/*文字为红色*/
    format1.setPatternBackgroundColor(QColor(152,251,152));/*背景颜色*/
    format1.setFontSize(15);/*设置字体大小*/
    format1.setHorizontalAlignment(QXlsx::Format::AlignHCenter);/*横向居中*/
    format1.setBorderStyle(QXlsx::Format::BorderDashDotDot);/*边框样式*/
    //xlsx.write("A1", "Hello Qt111111111111111111111111!", format1);/*写入文字，应该刚才设置的样式*/


    // qDebug()<<"lastRow:"<<Q_xlsx->dimension().lastRow();
    // qDebug()<<"lastColumn:"<<Q_xlsx->dimension().lastColumn();
    if(Q_xlsx->dimension().lastRow()<0)
    {
        Q_xlsx->write(1, 1, "工号", format1);/*写入文字，应该刚才设置的样式*/
        Q_xlsx->write(1, 2, "姓名", format1);
        Q_xlsx->write(1, 3, "开始时间", format1);
        Q_xlsx->write(1, 4, "结束时间", format1);
        Q_xlsx->write(1, 5, "使用时长", format1);
        Q_xlsx->write(1, 6, "使用记录", format1);
    }
    // QXlsx::Format format2;/*重新设置另一个单元的样式*/
    format2.setFontBold(true);/*设置加粗*/
    format2.setFontColor(QColor(Qt::blue));
    format2.setHorizontalAlignment(QXlsx::Format::AlignHCenter);/*横向居中*/
    //    format2.setFontUnderline(QXlsx::Format::FontUnderlineDouble);/*下双划线*/
    //    format2.setFillPattern(QXlsx::Format::PatternLightUp);/*填充方式*/
    lastRow=QString::number(Q_xlsx->dimension().lastRow()+1);

    //  Q_xlsx->write("A"+lastRow, this->user_name, format2);/*写入文字，应该刚才设置的样式*/
    start_time_date =QDateTime::currentDateTime();
    start_time =start_time_date.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    Q_xlsx->write("C"+lastRow, start_time, format2);


    //    if(!xlsx.selectSheet("ziv")){/*在当前打开的xlsx文件中，找一个名字为ziv的sheet*/
    //            xlsx.addSheet("ziv");//找不到的话就添加一个名为ziv的sheet
    //        }
    #if 0
    for (int i=10; i<20; ++i) {/*写入一串数字*/
        Q_xlsx->write(i, 1, i*i*i);   //A10:A19
        Q_xlsx->write(i, 2, i*i); //B10:B19
        Q_xlsx->write(i, 3, i*i-1); //C10:C19
    }

    QXlsx::Chart *pieChart = Q_xlsx->insertChart(3, 5, QSize(300, 300));/*在3行5列的位置插入一个图标*/
    pieChart->setChartType(QXlsx::Chart::CT_Pie);/*插入一个饼形图*/
    pieChart->addSeries(QXlsx::CellRange("A10:A19"));/*饼形图添加数据*/
    pieChart->addSeries(QXlsx::CellRange("B10:B19"));
    pieChart->addSeries(QXlsx::CellRange("C10:C19"));

    QXlsx::Chart *barChart = Q_xlsx->insertChart(3, 13, QSize(300, 300));/*在3行13列的位置插入一个图标*/
    barChart->setChartType(QXlsx::Chart::CT_Bar);/*条形图*/
    barChart->addSeries(QXlsx::CellRange("A10:C19"));/*给条形图加入数据*/
#endif

    //文件存在就直接打开否者就创建文件
    //    QFile creat_xlsx(qApp->applicationDirPath ()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".xlsx");
    //    if(creat_xlsx.exists()==false)
    //    {
    //        //creat_xlsx.open(QIODevice::ReadWrite);
    //        //creat_xlsx.open(QIODevice::NewOnly);
    //       // creat_xlsx.close();
    //    }
    //   Q_xlsx->saveAs(qApp->applicationDirPath ()+"/"+QDate::currentDate().toString("yyyy-MM-dd")+".xlsx");/*保存*/
    //QXlsx::Document xlsx2("1.xlsx");/*复制book1到book2*/
    //xlsx2.saveAs("Book2.xlsx");
    m_eEdgeStatus = EG_NORMAL;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(300);

}
MainWindow::~MainWindow()
{
    delete ui;
    delete configIniWrite;
    free(ini);
}

void MainWindow::ma_init()
{

    //this->user_name="admin";
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::magenta);
    ui->label->setPalette(pa);
    QFont ft;
    ft.setPointSize(15);
    ui->label->setFont(ft);
    ui->label->setText("用户："+this->user_name);
    Q_xlsx->write("A"+lastRow, this->user_id, format2);
    Q_xlsx->write("B"+lastRow, this->user_name, format2);

}

//注销槽函数
void MainWindow::on_pushButton_clicked()
{
   // Q_xlsx->write("A"+lastRow, ui->label->text(), format2);
    // QThread::sleep(1);
    end_time_date=QDateTime::currentDateTime();
    end_time=end_time_date.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    Q_xlsx->write("D"+lastRow, end_time, format2);
    //  QThread::sleep(1);
    //计算时间差
    QTime m_time;
    QString used_time;
    m_time.setHMS(0, 0, 0, 0);                                       //初始化数据，时 分 秒 毫秒
    used_time= m_time.addSecs(start_time_date.secsTo(end_time_date)).toString("hh:mm:ss");//计算时间差(秒)，将时间差加入m_time，格式化输出


    Q_xlsx->write("E"+lastRow, used_time, format2);

    //保存使用记录
    QSet<QString>::iterator iter;
    QString his_app_list;
    for(iter=his_app.begin();iter!=his_app.end();iter++ )
    {
        //qDebug()<<*iter;
        his_app_list=his_app_list+QString(*iter).replace(".exe","、");
    }
    Q_xlsx->write("F"+lastRow, his_app_list, format2);

    Q_xlsx->saveAs(qApp->applicationDirPath ()+"/"+"data/"+QDate::currentDate().toString("yyyy-MM-dd")+".xlsx");/*保存*/

    LoginWindow *login=new LoginWindow;
    login->show();
    delete this;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    QString path=event->mimeData()->text();
    filename=path;

    //if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }

}

void MainWindow::dropEvent(QDropEvent *event)
{
    if(filename.isEmpty())
    {
        return;
    }


    QStringList path1=event->mimeData()->text().split("\n");
    qDebug()<<path1;

    for(int i=0;i<path1.size();i++)
    {
        filename=path1.at(i);
        if(filename.isEmpty())
        {
            continue;
        }
        filename.replace("file:///","");

        QFileInfo fileInfo(filename);
        if(fileInfo.suffix()!="exe")
        {
            qDebug()<<fileInfo.suffix();
            continue;
        }
        QFileIconProvider icon;
        qDebug()<<"filename:"<<filename;

        //qDebug()<<"key:"<<configIniWrite->value("ini/"+QString(fileInfo.fileName()));

        //check repeat add
        QStringList old_ini=configIniWrite->allKeys();
        foreach (QString key, old_ini) {
            if(configIniWrite->value(key).toString()==filename)
            {
                QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("请务重复添加"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                return;
            }
        }
        QListWidgetItem *item = new QListWidgetItem(icon.icon(fileInfo),fileInfo.fileName());
        configIniWrite->setValue(("ini/"+QString(fileInfo.fileName())),filename);
        item->setSizeHint(QSize(80,60));
        ui->listWidget->addItem(item);
    }

}

void MainWindow::connectFri(QListWidgetItem * Item)
{
    QString path_exe;
    QString path_Ex;//split
    qDebug()<<Item->text();
    his_app.insert(Item->text());
    qDebug()<<"key:"<<configIniWrite->value("ini/"+QString(Item->text()));
    path_exe=configIniWrite->value("ini/"+QString(Item->text())).toString();
    QProcess pro;
    QFileInfo tt(path_exe);
    QDir::setCurrent(tt.path());
    path_Ex="\""+path_exe+"\"";
    qDebug()<<path_Ex;
    pro.startDetached(path_Ex);
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{

    QListWidgetItem* curItem = ui->listWidget->itemAt( pos );
    if( curItem == NULL )
    {
        QMenu *Menu_blank = new QMenu( this );
        QAction *deleteSeed_all = new QAction(tr("Delete ALL"), this);
        Menu_blank->addAction(deleteSeed_all);
        connect( deleteSeed_all, SIGNAL(triggered(bool)), this, SLOT(clearSeedsSlot()));
        Menu_blank->exec( QCursor::pos());
        return;
    }

    QMenu *popMenu = new QMenu( this );
    QAction *deleteSeed = new QAction(tr("Delete"), this);
    QAction *clearSeeds = new QAction(tr("待定"), this);
    popMenu->addAction( deleteSeed );
    popMenu->addAction( clearSeeds );
    connect( deleteSeed, SIGNAL(triggered(bool)), this, SLOT(deleteSeedSlot()));
    //    connect( clearSeeds, SIGNAL(triggered(bool)), this, SLOT(clearSeedsSlot()));
    popMenu->exec( QCursor::pos());
    delete popMenu;
    delete deleteSeed;
    delete clearSeeds;
}

void MainWindow::deleteSeedSlot()
{
    qDebug()<<"deleteSeedSlot";
    if(this->user_name!="admin")
    {
        QMessageBox::warning(NULL, "error","请联系管理员删除",
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::No);
        return;
    }
    int ch = QMessageBox::warning(NULL, "Warning","Are you sure to delete?",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

    if ( ch != QMessageBox::Yes )
        return;
    QListWidgetItem * item = ui->listWidget->currentItem();
    if(item==NULL)return;
    //delete record
    QFileInfo *record;
    QStringList old_ini=configIniWrite->allKeys();
    foreach (QString key, old_ini) {
        record=new QFileInfo(configIniWrite->value(key).toString());
        if(record->fileName()==item->text())
        {
            configIniWrite->remove(key);
        }
        delete(record);
    }

    int curIndex = ui->listWidget->row(item);
    ui->listWidget->takeItem(curIndex);
    delete item;
}

void MainWindow::clearSeedsSlot()
{
    qDebug()<<"clearSeedsSlot";
    int ch = QMessageBox::warning(NULL, "Warning",
                                  "Are you sure to clear ALL?",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

    if ( ch != QMessageBox::Yes )
        return;

    QListWidgetItem * item = ui->listWidget->currentItem();
    if( item == NULL )
        return;
    QStringList old_ini=configIniWrite->allKeys();
    foreach (QString key, old_ini) {
        configIniWrite->remove(key);
    }
    ui->listWidget->clear();
}


//void MainWindow::changeEvent(QEvent *event)
//{
//    if(event->type()!=QEvent::WindowStateChange) return;
//    if(this->windowState()==Qt::WindowMinimized)
//    {
//        this->hide();
//        //新建QSystemTrayIcon对象
//        mSysTrayIcon = new QSystemTrayIcon(this);
//        //新建托盘要显示的icon
//        QIcon icon = QIcon(":/res/Tonly.ico");
//        //将icon设到QSystemTrayIcon对象中
//        mSysTrayIcon->setIcon(icon);
//        //当鼠标移动到托盘上的图标时，会显示此处设置的内容
//        mSysTrayIcon->setToolTip(QObject::trUtf8("AP 管理"));
//        connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

//        //在系统托盘显示此对象
//        mSysTrayIcon->show();
//    }
//}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
        ui->listWidget->setFocus();
        // delete mSysTrayIcon;
        break;
    default:
        break;
    }
}

void MainWindow::hideEvent(QHideEvent *event)
{
    if(mSysTrayIcon->isVisible())
    {
        this->hide();
        event->ignore(); //忽略事件
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(mSysTrayIcon->isVisible())
    {
        //hide(); //隐藏窗口
       // qDebug()<<"请先注销";
        QMessageBox::critical(
                    this,QString("错误"),QString("请先注销!!!"));
        event->ignore(); //忽略事件
    }
}
///////////////
void MainWindow::enterEvent(QEvent *event)
{
    //qDebug()<<"enterEvent";
    switch (m_eEdgeStatus)
    {
    case EG_TOP:
        setGeometry(pos().x()+8, -SHOW_EDGE_WIDTH, width(), height());
        break;
    case EG_LEFT:
        setGeometry(-SHOW_EDGE_WIDTH, y()+30, width(), height());
        break;
    case EG_RIGHT:
        setGeometry(QApplication::desktop()->width() - width() + SHOW_EDGE_WIDTH, y()+30, width(), height());
        break;
    }
    return QWidget::enterEvent(event);
}
//窗口的离开事件（leaveEvent）
void MainWindow::leaveEvent(QEvent *event)
{
    //qDebug()<<"leaveEvent";
    switch (m_eEdgeStatus)
    {
    case EG_TOP:
        setGeometry(pos().x()+8, -height() + SHOW_EDGE_WIDTH, width(), height());
        break;
    case EG_LEFT:
        setGeometry(-width() + SHOW_EDGE_WIDTH, pos().y()+30, width(), height());
        break;
    case EG_RIGHT:
        setGeometry(QApplication::desktop()->width() - SHOW_EDGE_WIDTH, pos().y()+30, width(), height());
        break;
    }
    return QWidget::leaveEvent(event);
}

//鼠标释放弹起事件(mouseReleaseEvent)
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    //qDebug()<<"mouseReleaseEvent";
    //记录新移动到的坐标相对于上个坐标的位置
    int dx = event->globalX() - m_lastPoint.x();
    int dy = event->globalY() - m_lastPoint.y();

    move(x() + dx, y() + dy);

    //    if (y() <= 0)
    //    {
    //        m_eEdgeStatus = EG_TOP;
    //    }
    //    else if (QApplication::desktop()->width() <= (this->x() + width()))
    //    {
    //        m_eEdgeStatus = EG_RIGHT;
    //    }
    //    else if (this->x() <= 0)
    //    {
    //        m_eEdgeStatus = EG_LEFT;
    //    }
    //    else
    //    {
    //        m_eEdgeStatus = EG_NORMAL;
    //    }
    //  QWidget::mouseReleaseEvent(event);
}
void MainWindow::mouseMoveEvent(QMouseEvent*event)
{
    //qDebug()<<"mouseMoveEvent";
    //把移动的位置记录下来，这是相对于上个位置的距离
    int dx = event->globalX() - m_lastPoint.x();
    int dy = event->globalY() - m_lastPoint.y();

    m_lastPoint = event->globalPos(); //更新记录点

    move(x() + dx, y() + dy); //窗口移动到此处
#if 0
    if (event->buttons() & Qt::LeftButton)
    {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() -  this->pos();

        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() -  this->pos().y());
        }
        move(curPoint);
    }
#endif
    // QWidget::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent*event)
{
    //qDebug()<<"mousePressEvent";
    m_lastPoint = event->globalPos();
#if 0
    if (event->button() == Qt::LeftButton)
    {
        this->pos() = QCursor::pos() - frameGeometry().topLeft();
    }
#endif
    //QWidget::mousePressEvent(event);
}

void MainWindow::onTimeout()
{
    //   qDebug()<<"onTimeout";
    if (y() <= 0)
    {
        m_eEdgeStatus = EG_TOP;
    }
    else if (QApplication::desktop()->width()-this->x() <= width()-8)
    {
        //        qDebug()<<"test";
        //        qDebug()<<QApplication::desktop()->width();
        //        qDebug()<<this->x() + width();
        m_eEdgeStatus = EG_RIGHT;
    }
    else if (this->x() <= 0)
    {
        m_eEdgeStatus = EG_LEFT;
    }
    else
    {
        m_eEdgeStatus = EG_NORMAL;
        // qDebug()<<"EG_NORMAL";
    }

}





/*数据库操作，备用*/
//https://blog.csdn.net/y____xiang/article/details/80412247
