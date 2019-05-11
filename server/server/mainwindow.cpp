#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    movie = new QMovie(":/res/1.gif");
    ui->label->setMovie(movie);
    movie->start();
    ui->label_2->setFrameStyle(QFrame::NoFrame|QFrame::Sunken);
    ui->label_2->setText(tr("<a href=\"mailto:yukun.qin@tcl.com?subject=AP 人员管理 BUG&body=请详细说明\">BUG反馈请点此处</a>"));
    ui->label_2->setOpenExternalLinks(true);

    //打开数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    QFileInfo sql_file(QApplication::applicationDirPath() + "/config.db");
    if(sql_file.isFile()==false)
    {
        ui->textBrowser->append("数据库打开失败,请导入config.db");
        return;
    }
    else
    {
        db.setDatabaseName(QApplication::applicationDirPath() + "/config.db");
    }
    if(!db.open())
    {
        ui->textBrowser->append("数据库打开失败");
    }

    query=QSqlQuery(db);
    //   query.prepare("SELECT * FROM main");
    query.exec("SELECT * FROM main");
    model = new QStandardItemModel(this);

    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal, "姓名");
    model->setHeaderData(1,Qt::Horizontal, "工号");
    model->setHeaderData(2,Qt::Horizontal, "密码");

    int i=0;
    while(query.next())
    {
        model->setItem(i, 0, new QStandardItem(query.value(0).toString()));
        model->setItem(i, 1, new QStandardItem(query.value(2).toString()));
        model->setItem(i, 2, new QStandardItem(query.value(1).toString()));
        i++;
    }
    ui->tableView->setModel(model);
    ui->tableView->scrollToBottom();//到最后一行

    // ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑

    // qDebug()<<ui->tableView->model()->rowCount();
}

MainWindow::~MainWindow()
{
    delete ui;
}
//插入数据
void MainWindow::on_pushButton_clicked()
{
    QString id=ui->lineEdit->text();
    QString name=ui->lineEdit_2->text();
    QString passwd=ui->lineEdit_3->text();

    if(id.isEmpty()||name.isEmpty()||passwd.isEmpty())
    {
        ui->textBrowser->append("请填写正确的姓名、工号、密码");
        return;
    }
    //判断数据库中存在与否
    query.exec("SELECT * FROM main");
    int i=0;
    QString temp_name=ui->lineEdit_2->text();
    QString temp_id=ui->lineEdit->text();
    while(query.next())
    {
//        if(temp_name==query.value(0).toString())
//        {
//            ui->textBrowser->append("存在相同的姓名，请勿重复添加");
//            return;
//        }
        if(temp_id==query.value(2).toString())
        {
            ui->textBrowser->append("存在相同的工号，请勿重复添加");
            return;
        }
        i++;
    }

    //根据工号判断
    QString str = QString("insert into main(user_name, user_passwd, user_id) values('%1', '%2', '%3')").arg(name).arg(passwd).arg(id);
    query.exec(str);
    ui->textBrowser->append("插入成功");
    //显示
    int row_max=ui->tableView->model()->rowCount();
    model->setItem(row_max, 0, new QStandardItem(name));
    model->setItem(row_max, 1, new QStandardItem(id));
    model->setItem(row_max, 2, new QStandardItem(passwd));
    ui->tableView->setModel(model);
    ui->tableView->scrollToBottom();
}
//查询个例
void MainWindow::on_pushButton_4_clicked()
{
    if(!ui->lineEdit->text().isEmpty()||!ui->lineEdit_2->text().isEmpty())
    {
        QSqlQuery query;
        QString temp;
        if(ui->lineEdit->text().isEmpty())
        {
            temp = QString("select * from main where user_name = '%1'").arg(ui->lineEdit_2->text());
        }
        else
        {
            temp = QString("select * from main where user_id = '%1'").arg(ui->lineEdit->text());
        }
        query.exec(temp);
        query.next();
        QString show_name="姓名："+query.value(0).toString();
        QString show_id="工号："+query.value(2).toString();
        QString show_passwd="密码："+query.value(1).toString();
        ui->textBrowser->append(QString("%1%2%3").arg(show_name,20,' ').arg(show_id,6).arg(show_passwd,6));

        //   ui->textBrowser->append();
        //  ui->textBrowser->append("密码："+query.value(1).toString());
    }
    else
    {
        ui->textBrowser->append("查询用户失败");
    }
}

//查询所有的信息
void MainWindow::on_pushButton_5_clicked()
{
    model->removeRows(0,model->rowCount());
    QSqlQuery query;
    query.exec("SELECT * FROM main");
    int i=0;
    while(query.next())
    {
        model->setItem(i, 0, new QStandardItem(query.value(0).toString()));
        model->setItem(i, 1, new QStandardItem(query.value(2).toString()));
        model->setItem(i, 2, new QStandardItem(query.value(1).toString()));
        i++;
    }
    ui->textBrowser->append("查询所有的记录成功");
}
//删除记录
void MainWindow::on_pushButton_2_clicked()
{
    if(!ui->lineEdit->text().isEmpty()||!ui->lineEdit_2->text().isEmpty())
    {
        //从数据库中查询是否有这个人
        QSqlQuery query;
        QString temp;
        if(ui->lineEdit->text().isEmpty())
        {
            temp = QString("select * from main where user_name = '%1'").arg(ui->lineEdit_2->text());
        }
        else
        {
            temp = QString("select * from main where user_id = '%1'").arg(ui->lineEdit->text());
        }
        query.exec(temp);
        QString deletename,deleteid;
        while (query.next())
        {
            deletename = query.value(0).toString();
            deleteid = query.value(2).toString();
        }
        if(deletename == NULL||deleteid==NULL)
        {
            if(deletename==NULL)
            {
                ui->textBrowser->append("没有这个用户,删除失败");
            }
            else
            {
                ui->textBrowser->append("没有这个工号,删除失败");
            }
            return;
        }
        else
        {
            QString str ;
            if(deletename!=NULL)
            {
                str  =  QString("delete from main where user_name = '%1'").arg(deletename);
            }
            else
            {
                str  =  QString("delete from main where user_id = '%1'").arg(deleteid);
            }
            query.exec(str);//删除信息
            ui->textBrowser->append(QString("用户:%1       删除成功").arg(deletename));
            emit ui->pushButton_5->clicked();
        }
    }
    else
    {
        ui->textBrowser->append("删除失败，数据库中无记录");
    }
}

//修改用户信息
void MainWindow::on_pushButton_3_clicked()
{
    if(!ui->lineEdit->text().isEmpty()||!ui->lineEdit_2->text().isEmpty())
    {
        //从数据库中查询是否有这个人
        QSqlQuery query;
        QString temp;
        if(ui->lineEdit->text().isEmpty())
        {
            temp = QString("select * from main where user_name = '%1'").arg(ui->lineEdit_2->text());
        }
        else
        {
            temp = QString("select * from main where user_id = '%1'").arg(ui->lineEdit->text());
        }
        query.exec(temp);
        QString deletename,deleteid;
        while (query.next())
        {
            deletename = query.value(0).toString();
            deleteid = query.value(2).toString();
        }
        if(deletename == NULL||deleteid==NULL)
        {
            if(deletename==NULL)
            {
                ui->textBrowser->append("没有这个用户,修改失败");
            }
            else
            {
                ui->textBrowser->append("没有这个工号,修改失败");
            }
            return;
        }
        else
        {
            //update student set score = 66 where name = 'xiaoming';user_id

            if(deleteid==NULL)
            {
                temp = QString("update main set user_passwd = %1 where user_name = %2").arg(ui->lineEdit_3->text()).arg(deletename);
                ui->textBrowser->append(QString("修改成功，新密码为:%1").arg(ui->lineEdit_3->text()));
            }
            else
            {
                temp = QString("update main set user_passwd = %1 where user_id = %2").arg(ui->lineEdit_3->text()).arg(deleteid);
                ui->textBrowser->append(QString("修改成功，新密码为:%1").arg(ui->lineEdit_3->text()));
            }
            query.exec(temp);
            emit ui->pushButton_5->clicked();
        }

    }
    else
    {
        ui->textBrowser->append("请输入工号或姓名改密");
    }
}


