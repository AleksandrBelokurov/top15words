#include "scoreworker.h"

ScoreWorker::ScoreWorker(QThread *readThread, ReadWorker* readWorker, int topCounter)
    :readThread_(readThread), readWorker_(readWorker), topCounter_(topCounter)
{}

ScoreWorker::~ScoreWorker()
{}

void ScoreWorker::setTopCounter(int topCounter)
{
    topCounter_ = topCounter;
}

void ScoreWorker::process()
{
    if (readThread_ == nullptr)
        return;
    while(readThread_->isRunning())
        calcScore();
    calcScore();
    emit finished();
}

void ScoreWorker::calcScore()
{
    if (readWorker_ == nullptr)
        return;
    QMap<QString, int> result(readWorker_->getTopWords(topCounter_));
    if (result_ != result)
    {
        result_ = result;
        emit sendToChart(result_);
    }
}
