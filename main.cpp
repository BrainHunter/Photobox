#include "photobox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Photobox w;
    w.show();

    return a.exec();
}
