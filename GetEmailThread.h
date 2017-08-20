#ifndef GETEMAILTHREAD_H
#define GETEMAILTHREAD_H

#include <QThread>

class GetEmailThread : public QThread
{
    Q_OBJECT

public:
    GetEmailThread(QObject *parent = 0);

    void setStartNum(int startNum);

    void setEndNum(int endNum);

    void setUrl(const QString &url);

signals:
    void emailAvailable(QString email, int num, int percent);

protected:
    void run();

private:
    QString m_url;
    int m_startNum;
    int m_endNum;
};

#endif // GETEMAILTHREAD_H
