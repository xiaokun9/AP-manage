#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QtDebug>

#include <QPixmap>
#include <QString>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QtWinExtras/QtWinExtras>

#include <QListWidgetItem>
#include <QProcess>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include <QDesktopWidget>
#include <QMouseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Status
    {
        EG_TOP,
        EG_LEFT,
        EG_RIGHT,
        EG_NORMAL
    };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void ma_init();
    QSet<QString> his_app;


    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void hideEvent(QHideEvent *event);
//    void changeEvent(QEvent * event);
    void closeEvent(QCloseEvent *event);
    QString user_name;
    QString pass_wd;
    QString user_id;
    enum Status m_eEdgeStatus;
protected:
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void mousePressEvent(QMouseEvent*event);
    QTimer *timer;
protected:


private slots:
    void on_pushButton_clicked();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    QString filename;
    QFileInfo *ini;//ini file
    QSettings *configIniWrite;//write and read ini file
    QSystemTrayIcon *mSysTrayIcon;

    QString lastRow;
    QString start_time;
    QDateTime start_time_date;
    QString end_time;
    QDateTime end_time_date;
    QXlsx::Document *Q_xlsx;
    QXlsx::Format format2;

    //记录鼠标位置
    QPoint m_lastPoint;

public slots:
    void deleteSeedSlot();
    void clearSeedsSlot();
    void connectFri(QListWidgetItem *Item);
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void onTimeout();
};

#endif // MAINWINDOW_H
