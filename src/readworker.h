#ifndef READWORKER_H
#define READWORKER_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QThread>

class ReadWorker : public QObject
{
    Q_OBJECT

public:
    ReadWorker(QString CodecName = "UTF-8");
    virtual ~ReadWorker();
    void setFileName(QString fileName);
    void setCharSetCodec(QString codecName);
    QMap<QString, int> getTopWords(int topCounter);

public slots:
    void process();
    void setStop(bool stop);

signals:
    void progress(int);
    void finished();
    void startScore();

private:
    QString fileName_;
    QString codecName_;
    QQueue<bool> flag();
    bool m_stop{false};
    QMap<QString, int> words_;
    QMutex syncWords_;
    QMutex syncCancel_;

};

#endif // READWORKER_H
