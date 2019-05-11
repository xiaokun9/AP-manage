#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QStandardItemModel>
#include <QtDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMovie *movie;
    QSqlDatabase db;
    QSqlQuery query;
    QStandardItemModel* model;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


public slots:


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
