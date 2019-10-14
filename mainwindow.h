#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "db/triangulation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QProcess mqtt;
    QTimer* timer;
    ~MainWindow();

public slots:
    void closing();    

private:
    Triangulation triang;
    void MqttStart();
    void DB();    
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
