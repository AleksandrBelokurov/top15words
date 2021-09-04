#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      centralWidget_(new QWidget()),
      pbOpenFile_(new QPushButton(tr("Open file"))),
      pbBreak_(new QPushButton(tr("Break"))),
      lFileName_(new QLabel),
      hlControls_(new QHBoxLayout),
      ltopCounterSetting_(new QLabel("Top Counter 1 .. 15: ")),
      topCounterSetting_(new QSpinBox),
      hlCounterSettings_(new QHBoxLayout),
      lcharSetCodecSetting_(new QLabel("CharSet: ")),
      charSetCodecSetting_(new QComboBox),
      hlcharSetCodecSettings_(new QHBoxLayout),
      hlSettings_(new QHBoxLayout),
      vlChart_(new QVBoxLayout),
      pgbFileRead_(new QProgressBar),
      set0_(new QBarSet("Words")),
      series_(new QHorizontalBarSeries),
      axisX_(new QValueAxis),
      axisY_(new QBarCategoryAxis),
      chart_(new QChart),
      chartView_(new QChartView(chart_)),
      readWorker_(new ReadWorker),
      readThread_(new QThread),
      scoreWorker_(new ScoreWorker(readThread_, readWorker_)),
      scoreThread_(new QThread)
{
    topCounterSetting_->setMinimum(1);
    topCounterSetting_->setMaximum(15);
    topCounterSetting_->setValue(15);
    charSetCodecSetting_->addItems({
                                       {"UTF-8"},
                                       {"ISO 8859-1"},
                                       {"UTF-16"},
                                       {"KOI8-R"},
                                       {"Windows-1251"},
                                       {"CP866"},
                                   });
    hlControls_->addWidget(pbOpenFile_);
    hlControls_->addWidget(pbBreak_);
    hlControls_->addWidget(lFileName_);
    hlControls_->setSpacing(5);
    hlControls_->addStretch();
    vlChart_->addLayout(hlControls_);
    ltopCounterSetting_->setBuddy(topCounterSetting_);
    hlCounterSettings_->addWidget(ltopCounterSetting_);
    hlCounterSettings_->addWidget(topCounterSetting_);
    hlCounterSettings_->setSpacing(5);
    hlcharSetCodecSettings_->addWidget(lcharSetCodecSetting_);
    hlcharSetCodecSettings_->addWidget(charSetCodecSetting_);
    hlcharSetCodecSettings_->setSpacing(5);
    lcharSetCodecSetting_->setBuddy(charSetCodecSetting_);
    hlSettings_->addLayout(hlCounterSettings_);
    hlSettings_->addLayout(hlcharSetCodecSettings_);
    hlSettings_->setSpacing(50);
    hlSettings_->addStretch();
    vlChart_->addLayout(hlSettings_);

    QFont font = QApplication::font();
    font.setPixelSize(16);
    set0_->setColor(Qt::darkGreen);
    set0_->setLabelFont(font);
    set0_->setLabelColor(Qt::black);
    series_->setLabelsPosition(QHorizontalBarSeries::LabelsInsideBase);
    series_->setLabelsVisible(true);
    series_->append(set0_);
    chart_->addSeries(series_);
    chart_->setTitle("Top words in the selected utf8 text file");
    chart_->setAnimationOptions(QChart::NoAnimation);
    QStringList categories;
    categories << "";
    axisY_->setLabelsFont(font);
    axisY_->append(categories);
    chart_->addAxis(axisY_, Qt::AlignLeft);
    series_->attachAxis(axisY_);
    chart_->addAxis(axisX_, Qt::AlignBottom);
    series_->attachAxis(axisX_);
    axisX_->setLabelsFont(font);
    axisX_->applyNiceNumbers();
    chart_->legend()->setVisible(false);
    chartView_->setRenderHint(QPainter::Antialiasing);


    vlChart_->addWidget(chartView_);
    centralWidget_->setLayout(vlChart_);
    setCentralWidget(centralWidget_);
    statusBar()->addPermanentWidget(pgbFileRead_, 1);

    readWorker_->moveToThread(readThread_);
    scoreWorker_->moveToThread(scoreThread_);

    connect(pbOpenFile_, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(pbBreak_, &QPushButton::clicked, this, &MainWindow::abortReadFile);

    qRegisterMetaType<QMap<QString, int> >("QMap<QString, int>");

    connect(readThread_, &QThread::started, readWorker_, &ReadWorker::process);
    connect(readWorker_, &ReadWorker::startScore, this, [this] () { this->scoreThread_->start(); });
    connect(readWorker_, &ReadWorker::finished, readThread_, &QThread::quit);
    connect(readWorker_, &ReadWorker::progress, pgbFileRead_, &QProgressBar::setValue);

    connect(scoreThread_, &QThread::started, scoreWorker_, &ScoreWorker::process);
    connect(scoreWorker_, &ScoreWorker::finished, scoreThread_, &QThread::quit);
    connect(scoreWorker_, &ScoreWorker::sendToChart, this, &MainWindow::updateChart, Qt::BlockingQueuedConnection);

    connect(this, &MainWindow::abortRead, readWorker_, &ReadWorker::setStop, Qt::DirectConnection);
}

MainWindow::~MainWindow()
{

}

QSize MainWindow::sizeHint() const
{
    return QSize(1024, 768);
}

void MainWindow::openFile()
{
    abortReadFile();
    clearChart();
    QString strFile = QFileDialog::getOpenFileName(this, "Open File");
    if (!strFile.isEmpty())
    {
        scoreWorker_->setTopCounter(topCounterSetting_->value());
        lFileName_->setText(strFile);
        readWorker_->setFileName(strFile);
        readWorker_->setCharSetCodec(charSetCodecSetting_->currentText());
        readWorker_->setStop(false);
        readThread_->start();
    }
}

void MainWindow::abortReadFile()
{
    lFileName_->setText("");
    emit abortRead(true);
}

void MainWindow::clearChart()
{
    axisY_->clear();
    axisY_->append(QStringList{""});
    axisX_->setRange(0, 100);
    set0_->remove(0, set0_->count());
}

void MainWindow::updateChart(QMap<QString, int> result)
{
    set0_->remove(0, set0_->count());
    QStringList categories;
    int xmax = 0;
    QMapIterator<QString, int> it(result);
    it.toBack();
    while (it.hasPrevious())
    {
        it.previous();
        *set0_ << it.value();
        if (it.value() > xmax)
            xmax = it.value();
        categories << it.key();
    }
    axisY_->clear();
    axisY_->append(categories);
    axisX_->setRange(0, xmax);
}
