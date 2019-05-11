#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFileDialog>
#include <QLineEdit>


#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include <QSettings>
#include <QSystemTrayIcon>
#include <windows.h>
#pragma   comment(lib, "User32.lib")


//在注册表该目录下增加新内容
#define TASKMANAGERSystem "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"
#define TASKMANAGERExplorer "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    void setAutoStart(bool is_auto_start);
    QSystemTrayIcon *mSysTrayIcon;
    void closeEvent(QCloseEvent *event);


    void hook(bool flag); //功能键
    void showTaskWindow(bool flag); //显示任务栏
    void enableTaskManager(bool flag); //任务管理器


    HKL  hCurKL;
    QSqlDatabase db;
    QString user_name_log;
    QString user_password_log;
    void disableIME();
    void enableIME();
private slots:
    void on_pushButton_clicked();
public slots:
    bool eventFilter(QObject *watched,QEvent * event);
private:
    Ui::LoginWindow *ui;
    bool log_allow;

private:
    HWND task;
};

#endif // LOGINWINDOW_H
