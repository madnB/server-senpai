#include "stdafx.h"
#include "serversenpai.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ServerSenpai w;
	w.show();
	return a.exec();
}
