#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow w;
   // w.setAutoStart(true);
    w.showTaskWindow(true);
    w.enableTaskManager(true);
    w.hook(true);

    w.show();

    return a.exec();
}
