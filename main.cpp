#include "widgets/QlixMainWindow.h"
#include <QMainWindow>
#include <QApplication>
#include <QPlastiqueStyle>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(Qlix);
    QlixMainWindow qmw;
    app.setStyle("Plastique");
    qmw.show();
    app.exec();
    return 0;
} 

