#include "GetEmailThread.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore>
#include <iostream>
#include <QDebug>
using namespace std ;
class TT : public QObject
{
    Q_OBJECT
public:
    static QString getHtml(QString url)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
        QByteArray responseData;
        QEventLoop eventLoop;
        connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        eventLoop.exec();       //block until finish
        responseData = reply->readAll();
        manager->deleteLater();
        return QString(responseData);
    }
};

GetEmailThread::GetEmailThread(QObject *parent)
    : QThread(parent)
{
    m_stop = false;
}

void GetEmailThread::run()
{
    for(int i=m_startNum; i<m_endNum; i++) {
        QString url = m_url + QString::number(i);
        QString data = TT::getHtml(url);

        qDebug() << url;

        int from = 0;
        int nCurrentFrameStart = 1;
        while(nCurrentFrameStart > 0) {
            nCurrentFrameStart = data.indexOf(QRegExp("class=\"mail\""), from);

            from = nCurrentFrameStart+13;

            QString email = data.mid(from, 100);

            if(!email.contains("[at]")) {
                continue;
            }
            int end = email.indexOf(QRegExp("\\</a\\>"));

            if(end < 0) {
                qDebug() << email;
                continue;
            }

            email = email.left(end);

            email.replace("&#46;", ".");
            email.replace("[at]", "@");
            email.remove("<br />");
            email.remove(" ");
            email.remove("&nbsp;");

            qDebug() << i
                     << email;

            if(email.contains("esb-online.com")) {
                continue;
            }

            emit emailAvailable(email, i);
        }

        emit urlChanged(url);

        if(m_stop) {
            m_stop = false;
            quit();
            return;
        }
    }
}

void GetEmailThread::setUrl(const QString &url)
{
    m_url = url;
}

void GetEmailThread::stop()
{
    m_stop = true;
}

void GetEmailThread::setEndNum(int endNum)
{
    m_endNum = endNum;
}

void GetEmailThread::setStartNum(int startNum)
{
    m_startNum = startNum;
}
