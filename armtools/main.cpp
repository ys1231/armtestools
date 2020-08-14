#include "armtools.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    armtools w;
    w.show();
    return a.exec();
}
