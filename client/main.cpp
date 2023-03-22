#include <QApplication>
#include "Multimetr.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Multimetr widget("/tmp/socket_multimetr", 10);
    widget.show();
    return app.exec();
}
