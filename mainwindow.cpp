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

/*void MainWindow::DB(){
    qDebug() << "Running DB";


    db.number_of_rilevations(CTime(2019, 10, 1, 19, 00, 0).GetTime(), CTime(2019, 10, 1, 19, 30, 0).GetTime());
    db.last_positions(CTime(2019, 10, 1, 19, 01, 0).GetTime());
    db.statistics_fun(CTime(2019, 10, 1, 19, 00, 0).GetTime(), 1);
}

*/
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



    // Start MQTT
    MqttStart();

    // Start DB
    Db_original db;
    db.triang=Triangulation();
    // Init triangulation
    // TODO - read configuration
    Point root1(0.0, 0.0), root2(0.8,0.0); //root3(0.0,5.0);
    pair<string,Point> a("30:AE:A4:1D:52:BC",root1),b("30:AE:A4:75:23:E8",root2);//,c("a",root3);
    map<string, Point> roots = { a,b};

    db.triang.initTriang(roots);

    int n_sec=1;
    this->timer->setInterval(n_sec*1000);
    connect(this->timer, &QTimer::timeout,this, []() {
        Db_original db;
        db.triang=Triangulation();
        db.loop(CTime(2019, 10, 4, 13, 30, 00).GetTime());});
    this->timer->start();

    //---------------------
    // Create Main Window
    //---------------------

    ui->setupUi(this);


    // MAP TAB



    // Create your time series
    QScatterSeries *boardScatter = new QScatterSeries();
    boardScatter->setName("Boards");
    boardScatter->setPointLabelsVisible(true);
    boardScatter->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    boardScatter->setMarkerSize(10.0);
    *boardScatter << QPointF(3, 3) << QPointF(7, 6) << QPointF(10, 2);

    QScatterSeries *phoneScatter = new QScatterSeries();
    phoneScatter->setName("Phones");
    phoneScatter->setPointLabelsVisible(true);
    phoneScatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    phoneScatter->setMarkerSize(10.0);
    *phoneScatter << QPointF(5, 3) << QPointF(7, 2) << QPointF(3, 1);


    // Configure your chart
    QChart *chartScatter = new QChart();
    chartScatter->addSeries(boardScatter);
    chartScatter->addSeries(phoneScatter);
    QValueAxis *axisYmap = new QValueAxis();
    axisYmap->setRange(0, 20);
    chartScatter->addAxis(axisYmap, Qt::AlignLeft);
    boardScatter->attachAxis(axisYmap);
    phoneScatter->attachAxis(axisYmap);
    QValueAxis *axisXmap = new QValueAxis();
    axisXmap->setRange(0, 20);
    chartScatter->addAxis(axisXmap, Qt::AlignBottom);
    boardScatter->attachAxis(axisXmap);
    phoneScatter->attachAxis(axisXmap);
    chartScatter->setTitle("Real time map of detected devices");
    chartScatter->setDropShadowEnabled(false);
    chartScatter->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Create your chart view
    QChartView *graphicsViewScatter = new QChartView(chartScatter);
    graphicsViewScatter->setRenderHint(QPainter::Antialiasing);




    //HISTORY TAB



    // Create time and date picker

    QDateTimeEdit *histDateEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    histDateEdit->setMaximumDate(QDate::currentDate());
    histDateEdit->setDisplayFormat("yyyy.MM.dd hh:mm");

    QLabel *histFormatLabel = new QLabel(tr("Pick finish time"));

    QString histText = QString("Date selected: %1").arg(histDateEdit->date().toString("d/M/yyyy"));

    QLabel* histLabel = new QLabel(histText);

    // Create time series using current time
    QBarSet *set0hist = new QBarSet("Private MAC");
    QBarSet *set1hist = new QBarSet("Public MAC");

    *set0hist << 1 << 2 << 3 << 4 << 5 << 6;
    *set1hist << 5 << 0 << 0 << 4 << 0 << 7;

    QStackedBarSeries *histSeriesBar = new QStackedBarSeries();
    histSeriesBar->append(set0hist);
    histSeriesBar->append(set1hist);

    // Configure the chart using current time
    QChart *histChartBar = new QChart();
    histChartBar->addSeries(histSeriesBar);
    histChartBar->setTitle("Number of devices tracked");
    histChartBar->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    histChartBar->addAxis(axisX, Qt::AlignBottom);
    histSeriesBar->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    histChartBar->addAxis(axisY, Qt::AlignLeft);
    histSeriesBar->attachAxis(axisY);

    histChartBar->legend()->setVisible(true);
    histChartBar->legend()->setAlignment(Qt::AlignBottom);

    // Create chart view
    QChartView *histChartViewBar = new QChartView(histChartBar);
    histChartViewBar->setRenderHint(QPainter::Antialiasing);


    // Update chart with selected time
    connect(histDateEdit, &QDateTimeEdit::dateTimeChanged, this, [histLabel, histChartViewBar] (QDateTime temp){
        QString dateText = QString("Date selected: %1").arg(temp.toString("d/M/yyyy"));
        histLabel->setText(dateText);
        QBarSet *set0 = new QBarSet("Private MAC");
        QBarSet *set1 = new QBarSet("Public MAC");

        *set0 << 1 << 2 << 3 << 4 << 5 << 6;
        *set1 << 5 << 0 << 0 << 4 << 0 << 7;

        QStackedBarSeries *seriesBar = new QStackedBarSeries();
        seriesBar->append(set0);
        seriesBar->append(set1);

        // Configure updated chart
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

        histChartViewBar->setChart(chartBar);
        histChartViewBar->setRenderHint(QPainter::Antialiasing);


    });

    QVBoxLayout *histLayout = new QVBoxLayout;
    histLayout->addWidget(histFormatLabel);
    histLayout->addWidget(histDateEdit);
    histLayout->addWidget(histChartViewBar);
    histLayout->addWidget(histLabel);
    QWidget *histWidget = new QWidget;
    histWidget->setLayout(histLayout);



    // STATS TAB



    // Create time and date picker

    QDateTimeEdit *statsDateEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    statsDateEdit->setMaximumDate(QDate::currentDate());
    statsDateEdit->setDisplayFormat("yyyy.MM.dd hh:mm");

    QLabel *statsEndLabel = new QLabel(tr("Pick finish time"));

    QLabel *statsFormatLabel = new QLabel(tr("Pick frequency period"));

    QString statsText = QString("Date selected: %1").arg(statsDateEdit->date().toString("d/M/yyyy"));

    QLabel* statsLabel = new QLabel(statsText);

    QComboBox *statsComboBox = new QComboBox;
        statsComboBox->addItem(tr("Last 2 hours"));
        statsComboBox->addItem(tr("Last day"));
        statsComboBox->addItem(tr("Last week"));

    // Create time series using current time
    QLineSeries *statsLineseries = new QLineSeries();
        statsLineseries->setName("Number of devices tracked");
        statsLineseries->append(QPoint(0, 4));
        statsLineseries->append(QPoint(1, 15));
        statsLineseries->append(QPoint(2, 20));
        statsLineseries->append(QPoint(3, 4));
        statsLineseries->append(QPoint(4, 12));
        statsLineseries->append(QPoint(5, 17));


    // Configure the chart using current time
    QChart *statsChartBar = new QChart();
    statsChartBar->addSeries(statsLineseries);
    statsChartBar->setTitle("Number of devices tracked");
    statsChartBar->setAnimationOptions(QChart::SeriesAnimations);

    QStringList statsCategories;
    statsCategories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axisXstats = new QBarCategoryAxis();
    axisXstats->append(statsCategories);
    statsChartBar->addAxis(axisXstats, Qt::AlignBottom);
    statsLineseries->attachAxis(axisXstats);
    axisXstats->setRange(QString("Jan"), QString("Jun"));
    QValueAxis *axisYstats = new QValueAxis();
    statsChartBar->addAxis(axisYstats, Qt::AlignLeft);
    statsLineseries->attachAxis(axisYstats);

    statsChartBar->legend()->setVisible(true);
    statsChartBar->legend()->setAlignment(Qt::AlignBottom);

    // Create chart view
    QChartView *statsChartViewBar = new QChartView(statsChartBar);
    statsChartViewBar->setRenderHint(QPainter::Antialiasing);


    // Update chart with selected time
    connect(statsDateEdit, &QDateTimeEdit::dateTimeChanged, this, [statsLabel, statsChartViewBar] (QDateTime temp){
        QString dateText = QString("Date selected: %1").arg(temp.toString("d/M/yyyy"));
        statsLabel->setText(dateText);
        QLineSeries *statsLineseries = new QLineSeries();
            statsLineseries->setName("Number of devices tracked");
            statsLineseries->append(QPoint(0, 4));
            statsLineseries->append(QPoint(1, 15));
            statsLineseries->append(QPoint(2, 20));
            statsLineseries->append(QPoint(3, 4));
            statsLineseries->append(QPoint(4, 12));
            statsLineseries->append(QPoint(5, 17));

            // Configure the chart using current time
            QChart *statsChartBar = new QChart();
            statsChartBar->addSeries(statsLineseries);
            statsChartBar->setTitle("Number of devices tracked");
            statsChartBar->setAnimationOptions(QChart::SeriesAnimations);

            QStringList statsCategories;
            statsCategories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
            QBarCategoryAxis *axisXstats = new QBarCategoryAxis();
            axisXstats->append(statsCategories);
            statsChartBar->addAxis(axisXstats, Qt::AlignBottom);
            statsLineseries->attachAxis(axisXstats);
            axisXstats->setRange(QString("Jan"), QString("Jun"));
            QValueAxis *axisYstats = new QValueAxis();
            statsChartBar->addAxis(axisYstats, Qt::AlignLeft);
            statsLineseries->attachAxis(axisYstats);

            statsChartBar->legend()->setVisible(true);
            statsChartBar->legend()->setAlignment(Qt::AlignBottom);

            statsChartViewBar->setChart(statsChartBar);
            statsChartViewBar->setRenderHint(QPainter::Antialiasing);



    });


    // Update chart with selected frequency
    connect(statsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [statsChartViewBar] (){
        QLineSeries *statsLineseries = new QLineSeries();
            statsLineseries->setName("Number of devices tracked");
            statsLineseries->append(QPoint(0, 4));
            statsLineseries->append(QPoint(1, 15));
            statsLineseries->append(QPoint(2, 20));
            statsLineseries->append(QPoint(3, 4));
            statsLineseries->append(QPoint(4, 12));
            statsLineseries->append(QPoint(5, 17));

            // Configure the chart using current time
            QChart *statsChartBar = new QChart();
            statsChartBar->addSeries(statsLineseries);
            statsChartBar->setTitle("Number of devices tracked");
            statsChartBar->setAnimationOptions(QChart::SeriesAnimations);

            QStringList statsCategories;
            statsCategories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
            QBarCategoryAxis *axisXstats = new QBarCategoryAxis();
            axisXstats->append(statsCategories);
            statsChartBar->addAxis(axisXstats, Qt::AlignBottom);
            statsLineseries->attachAxis(axisXstats);
            axisXstats->setRange(QString("Jan"), QString("Jun"));
            QValueAxis *axisYstats = new QValueAxis();
            statsChartBar->addAxis(axisYstats, Qt::AlignLeft);
            statsLineseries->attachAxis(axisYstats);

            statsChartBar->legend()->setVisible(true);
            statsChartBar->legend()->setAlignment(Qt::AlignBottom);

            statsChartViewBar->setChart(statsChartBar);
            statsChartViewBar->setRenderHint(QPainter::Antialiasing);



    });

    QVBoxLayout *statsLayout = new QVBoxLayout;
    statsLayout->addWidget(statsEndLabel);
    statsLayout->addWidget(statsDateEdit);
    statsLayout->addWidget(statsFormatLabel);
    statsLayout->addWidget(statsComboBox);
    statsLayout->addWidget(statsChartViewBar);
    statsLayout->addWidget(statsLabel);
    QWidget *statsWidget = new QWidget;
    statsWidget->setLayout(statsLayout);




    // LOAD CREATED TABS



    auto tw = new QTabWidget (this);
    tw->addTab(graphicsViewScatter, "Map");
    tw->addTab(histWidget, "History");
    tw->addTab(statsWidget, "Stats");

    setCentralWidget(tw);


}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;
}

