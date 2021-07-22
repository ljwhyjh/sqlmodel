#include "sqlmodel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sqlmodel w;
    w.show();


    //w.setWindowFlags(w.windowFlags()&~ Qt::WindowMinMaxButtonsHint);

    return a.exec();
}
