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

        qDebug() << url << data.size();

        int from = 0;
        int nCurrentFrameStart = 1;

        do {
            nCurrentFrameStart = data.indexOf(QRegExp("\\[at\\]"), from);
            if(nCurrentFrameStart > 0) {
                int s = data.lastIndexOf(">", nCurrentFrameStart);
                from = nCurrentFrameStart+13;

                if(s > 0) {
                    QString email = data.mid(s+1, 100);

                    if(email.contains("[at]")) {
                        getMailAddress(email, i);
                    }
                }
            }
        } while (nCurrentFrameStart > 0);

        do {
            nCurrentFrameStart = data.indexOf(QRegExp("@"), from);
            if(nCurrentFrameStart > 0) {
                int s = data.lastIndexOf(">", nCurrentFrameStart);
                from = nCurrentFrameStart+13;

                if(s > 0) {
                    QString email = data.mid(s+1, 100);

                    if(email.contains("@")) {
                        getMailAddress(email, i);
                    }
                }
            }
        } while(nCurrentFrameStart > 0);

        emit urlChanged(url);

        if(m_stop) {
            m_stop = false;
            quit();
            return;
        }
    }
}

bool GetEmailThread::getMailAddress(QString &d, int &idx)
{
    QString email;

    int end = d.indexOf(QRegExp("\\<"));

    if(end < 0) {
        qDebug() << d;
        return false;
    }

    email = d.left(end);

    email.replace("&#46;", ".");
    email.replace("[at]", "@");
    email.remove("&nbsp;");

    email = email.simplified();

    email.remove(" ");

    if(email.contains("esb-online.com")) {
        return false;
    }

    if(email.contains(" ")) {
        QStringList lst = email.split(" ");

        foreach (QString e, lst) {
            if(e.contains("@")) {
                email = e;
                break;
            }
        }
    }

    if(email.contains(":")) {
        QStringList lst = email.split(":");

        foreach (QString e, lst) {
            if(e.contains("@")) {
                email = e;
                break;
            }
        }
    }

    if(!email.contains("@")) {
        return false;
    }

    email = email.toLower();

    if(email.startsWith('@') || email.endsWith('@')) {
        return false;
    }

    qDebug() << idx
             << email;

    emit emailAvailable(email, idx);

    return true;
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
