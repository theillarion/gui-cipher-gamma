#include "CipherWindow.hpp"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CipherWindow w;

	w.resize(1280, 860);
	qDebug() << QString{ "[info] size window %1 x %2" }.arg(w.size().width(), w.size().height());
    w.show();

    return a.exec();
}