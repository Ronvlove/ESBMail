#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>

#define MAX_THREADS     100

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_thread.clear();

    for(int i=0; i<MAX_THREADS; i++) {
        GetEmailThread* thread = new GetEmailThread;
        connect(thread, SIGNAL(emailAvailable(QString, int)),
                this, SLOT(onEmailAvailable(QString, int)));

        connect(thread, SIGNAL(urlChanged(QString)),
                this, SLOT(onUrlChanged(QString)));

        m_thread.append(thread);
    }

    setWindowTitle(tr("ESB Mail Collector"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEmailAvailable(QString email, int num)
{
    if(email.isEmpty()) return;

    ui->listWidget->addItem(QString::number(num) + "\t" + email);

    if(m_fileName.isEmpty()) return;

    QFile data(m_fileName);
    if (data.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream out(&data);
        out << email
            << "\r\n";
    }
}

void MainWindow::onUrlChanged(QString url)
{
    ui->statusBar->showMessage(url);

    m_finishCnt++;

    ui->progressBar->setValue(m_finishCnt*100/m_urlCount);
}

void MainWindow::on_pushButton_clicked()
{
    if(m_fileName.isEmpty()) {
        on_pushButton_2_clicked();
    }

    if(m_fileName.isEmpty() || ui->lineEditUrl->text().isEmpty()) {
        ui->statusBar->showMessage(tr("Error!"));
        return;
    }

    for(int i=0; i<MAX_THREADS; i++) {
        if(m_thread.at(i)->isRunning()) {
            ui->statusBar->showMessage(tr("Stop first!"));
            return;
        }
    }

    int threads = ui->spinBoxThreads->value();
    int start = ui->spinBoxStart->value();
    int end = ui->spinBoxEnd->value();
    int step;
    if(start > end) {
        step = start;
        start = end;
        end = start;
    }

    m_urlCount = end - start;
    m_finishCnt = 0;
    step = m_urlCount / threads;

    ui->progressBar->setValue(0);

    for(int i=0; i<threads; i++) {
        m_thread.at(i)->setUrl(ui->lineEditUrl->text());
        m_thread.at(i)->setStartNum(start + i*step);

        if(i == threads-1) {
            m_thread.at(i)->setEndNum(end);
        } else {
            m_thread.at(i)->setEndNum(start + (i+1)*step);
        }

        m_thread.at(i)->start();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    m_fileName = QFileDialog::getSaveFileName(this,
                                 tr("Save File"),
                                 QApplication::applicationDirPath(),
                                 tr("*.txt"));

    ui->lineEditFile->setText(m_fileName);
}

void MainWindow::on_stop_clicked()
{
    for(int i=0; i<MAX_THREADS; i++) {
        m_thread.at(i)->stop();
    }
}
