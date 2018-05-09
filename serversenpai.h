#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_serversenpai.h"

class ServerSenpai : public QMainWindow
{
	Q_OBJECT

public:
	ServerSenpai(QWidget *parent = Q_NULLPTR);

private:
	Ui::ServerSenpaiClass ui;
};
