#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GetEmailThread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onEmailAvailable(QString email, int num);
    void onUrlChanged(QString url);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_stop_clicked();

private:
    Ui::MainWindow *ui;
    QList<GetEmailThread*> m_thread;
    QString m_fileName;
    int m_urlCount;
    int m_finishCnt;
};

#endif // MAINWINDOW_H
