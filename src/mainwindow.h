#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QValueAxis>

#include "readworker.h"
#include "scoreworker.h"

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void abortRead(bool stop);

protected:
    QSize sizeHint() const;

private:
    QWidget *centralWidget_;
    QPushButton *pbOpenFile_;
    QPushButton *pbBreak_;
    QLabel *lFileName_;
    QHBoxLayout *hlControls_;
    QLabel *ltopCounterSetting_;
    QSpinBox *topCounterSetting_;
    QHBoxLayout *hlCounterSettings_;
    QLabel *lcharSetCodecSetting_;
    QComboBox *charSetCodecSetting_;
    QHBoxLayout *hlcharSetCodecSettings_;
    QHBoxLayout *hlSettings_;
    QVBoxLayout *vlChart_;
    QProgressBar *pgbFileRead_;

    QBarSet *set0_;
    QHorizontalBarSeries *series_;
    QValueAxis *axisX_;
    QBarCategoryAxis *axisY_;
    QChart *chart_;
    QChartView *chartView_;

    ReadWorker *readWorker_;
    QThread *readThread_;
    ScoreWorker *scoreWorker_;
    QThread *scoreThread_;

private slots:
    void openFile();
    void abortReadFile();
    void clearChart();
    void updateChart(QMap<QString, int> result);

};
#endif // MAINWINDOW_H
