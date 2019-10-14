#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "schema/helperclass.h"
#include <QDebug>
#include <db/Db_original.h>
#include <atltime.h>
#include <ctime>

void MainWindow::MqttStart(){
    QString program = "python mqtt.py";
    qDebug() << "Start python";
    this->mqtt.startDetached(program);
    //this->mqtt.
}

void MainWindow::DB(){
    qDebug() << "Running DB";
    Db_original db;

    db.loop(CTime(2019, 10, 4, 13, 30, 00).GetTime());
    db.number_of_rilevations(CTime(2019, 10, 1, 19, 00, 0).GetTime(), CTime(2019, 10, 1, 19, 30, 0).GetTime());
    db.last_positions(CTime(2019, 10, 1, 19, 01, 0).GetTime());
    db.statistics_fun(CTime(2019, 10, 1, 19, 00, 0).GetTime(), 1);
}


void MainWindow::closing(){
    qDebug() << "Kill Process...";
    this->timer->stop();
    this->mqtt.kill();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->timer = new QTimer(this);

    // TODO - read configuration
    Point root1(0.0, 0.0), root2(5.0,0.0), root3(0.0,5.0);
    pair<string,Point> a("a",root1),b("a",root2),c("a",root3);
    map<string, Point> roots = { a,b,c };

    // Init triangulation
    Triangulation::initTriang(roots);

    // Start MQTT
    MqttStart();

    // Start DB
    this->timer->setInterval(30*1000);
    connect(this->timer, &QTimer::timeout,this, &MainWindow::DB);
    this->timer->start();

    //---------------------
    // Create Main Window
    //---------------------

    ui->setupUi(this);


    // MAP TAB



    // Create your time series
    QScatterSeries *seriesScatter = new QScatterSeries();
    seriesScatter->setName("scatter");
    seriesScatter->setPointLabelsVisible(true);
    seriesScatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    seriesScatter->setMarkerSize(5.0);
    *seriesScatter << QPointF(1, 1) << QPointF(3, 3) << QPointF(7, 6) << QPointF(8, 3) << QPointF(10, 2);

    // Configure your chart
    QChart *chartScatter = new QChart();
    chartScatter->addSeries(seriesScatter);
    chartScatter->createDefaultAxes();
    chartScatter->setTitle("Simple scatter chart example");
    chartScatter->setDropShadowEnabled(false);
    chartScatter->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Create your chart view
    QChartView *graphicsViewScatter = new QChartView(chartScatter);
    graphicsViewScatter->setRenderHint(QPainter::Antialiasing);




    //STATS TAB



    // Create time and date picker

    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
    dateEdit->setMaximumDate(QDate::currentDate());
    dateEdit->setDisplayFormat("yyyy.MM.dd hh:mm");

    QLabel *formatLabel = new QLabel(tr("Pick finish time"));

    QString dateText = QString("Date selected: %1").arg(dateEdit->date().toString("d/M/yyyy"));

    QLabel* dateLabel = new QLabel(dateText);

    // Create your time series
    QBarSet *set0 = new QBarSet("Private MAC");
    QBarSet *set1 = new QBarSet("Public MAC");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;

    QStackedBarSeries *seriesBar = new QStackedBarSeries();
    seriesBar->append(set0);
    seriesBar->append(set1);

    // Configure your chart
    QChart *chartBar = new QChart();
    chartBar->addSeries(seriesBar);
    chartBar->setTitle("Number of devices tracked");
    chartBar->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chartBar->addAxis(axisX, Qt::AlignBottom);
    seriesBar->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    chartBar->addAxis(axisY, Qt::AlignLeft);
    seriesBar->attachAxis(axisY);

    chartBar->legend()->setVisible(true);
    chartBar->legend()->setAlignment(Qt::AlignBottom);

    // Create your chart view
    QChartView *chartViewBar = new QChartView(chartBar);
    chartViewBar->setRenderHint(QPainter::Antialiasing);

    connect(dateEdit, &QDateTimeEdit::dateTimeChanged, this, [dateLabel, chartViewBar] (QDateTime temp){
        QString dateText = QString("Date selected: %1").arg(temp.toString("d/M/yyyy"));
        dateLabel->setText(dateText);
        QBarSet *set0 = new QBarSet("Private MAC");
        QBarSet *set1 = new QBarSet("Public MAC");

        *set0 << 1 << 2 << 3 << 4 << 5 << 6;
        *set1 << 5 << 0 << 0 << 4 << 0 << 7;

        QStackedBarSeries *seriesBar = new QStackedBarSeries();
        seriesBar->append(set0);
        seriesBar->append(set1);

        // Configure your chart
        QChart *chartBar = new QChart();
        chartBar->addSeries(seriesBar);
        chartBar->setTitle("Number of devices tracked");
        chartBar->setAnimationOptions(QChart::SeriesAnimations);

        QStringList categories;
        categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chartBar->addAxis(axisX, Qt::AlignBottom);
        seriesBar->attachAxis(axisX);
        QValueAxis *axisY = new QValueAxis();
        chartBar->addAxis(axisY, Qt::AlignLeft);
        seriesBar->attachAxis(axisY);

        chartBar->legend()->setVisible(true);
        chartBar->legend()->setAlignment(Qt::AlignBottom);

        chartViewBar->setChart(chartBar);
        chartViewBar->setRenderHint(QPainter::Antialiasing);


    });




    QVBoxLayout *statsLayout = new QVBoxLayout;
    statsLayout->addWidget(formatLabel);
    statsLayout->addWidget(dateEdit);
    statsLayout->addWidget(chartViewBar);
    statsLayout->addWidget(dateLabel);
    QWidget *statsWidget = new QWidget;
    statsWidget->setLayout(statsLayout);

    auto tw = new QTabWidget (this);
    tw->addTab(graphicsViewScatter, "Map");
    tw->addTab(statsWidget, "Stats");
    tw->addTab(new QWidget, "third");

    setCentralWidget(tw);


}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;
}

