#include "readworker.h"
#include <QFile>
#include <QTextStream>

ReadWorker::ReadWorker(QString CodecName)
    :codecName_(CodecName)
{}

ReadWorker::~ReadWorker()
{}

void ReadWorker::setFileName(QString fileName)
{
    fileName_ = fileName;
}

void ReadWorker::setCharSetCodec(QString codecName)
{
    codecName_ = codecName;
}

QMap<QString, int> ReadWorker::getTopWords(int topCounter)
{
    QMultiMap<int, QString> inv;
    syncWords_.lock();
    QMap<QString, int>::const_iterator it1 = words_.cbegin();
    while (it1 != words_.cend())
    {
        inv.insert(it1.value(), it1.key());
        ++it1;
    }
    syncWords_.unlock();
    QMap<QString, int> result;
    QMapIterator<int, QString> it2(inv);
    it2.toBack();
    int i = 1;
    while (it2.hasPrevious())
    {
        it2.previous();
        result.insert(it2.value(), it2.key());
        if (++i > topCounter)
            break;
    }
    return result;
}

void ReadWorker::process()
{
    QFile file(fileName_);
    int fsize = file.size();
    int pc = 0;
    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        QString str;
        syncWords_.lock();
        words_.clear();
        syncWords_.unlock();
        QTextStream stream(&file);
        stream.setCodec(codecName_.toStdString().c_str());
        emit startScore();
        while(!stream.atEnd())
        {
            syncCancel_.lock();
            if (m_stop)
            {
                syncCancel_.unlock();
                break;
            }
            syncCancel_.unlock();
            stream >> str;
            if (!str.isEmpty() && str.at(0).isLetter())
            {
                syncWords_.lock();
                ++words_[str];
                syncWords_.unlock();
            }
            int tt =  ((pc += str.length() + 1) * 100) / fsize;
            emit progress(tt);
        }
        file.close();
        emit progress(100);
    }
    emit finished();
}

void ReadWorker::setStop(bool stop)
{
    syncCancel_.lock();
    m_stop = stop;
    syncCancel_.unlock();
}
