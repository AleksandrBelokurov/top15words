#ifndef SCOREWORKER_H
#define SCOREWORKER_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QTimer>
#include "readworker.h"

class ScoreWorker : public QObject
{
    Q_OBJECT
public:
    explicit ScoreWorker(QThread *readThread = nullptr, ReadWorker *readWorker = nullptr, int topCounter = 15);
    virtual ~ScoreWorker();
    void setTopCounter(int topCounter);

public slots:
    void process();

signals:
    void finished();
    void sendToChart(QMap<QString, int>);

private:
    QThread *readThread_;
    ReadWorker *readWorker_;
    int topCounter_;
    void calcScore();
    QMap<QString, int> result_;

};

#endif // SCOREWORKER_H
