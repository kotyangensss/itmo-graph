#include <QApplication>
#include "QMain.h"
#include "Pixels.h"
#include "QImageWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    auto* pixels = new Pixels();

    auto interface = new QMain(pixels, new QImageWidget(pixels));
    interface->show();
    return QApplication::exec();
}
