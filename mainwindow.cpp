#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_thread = NULL;

    setWindowTitle(tr("ESB Mail Collector"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEmailAvailable(QString email, int num, int percent)
{    
    ui->progressBar->setValue(percent);

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

void MainWindow::on_pushButton_clicked()
{
    if(m_fileName.isEmpty()) {
        on_pushButton_2_clicked();
    }

    if(m_fileName.isEmpty() || ui->lineEditUrl->text().isEmpty()) {
        ui->statusBar->showMessage(tr("Error!"));
        return;
    }

    if(m_thread == NULL) {
        m_thread = new GetEmailThread;
        connect(m_thread, SIGNAL(emailAvailable(QString, int, int)),
                this, SLOT(onEmailAvailable(QString, int, int)));
    }

    if(!m_thread->isRunning()){
        ui->progressBar->setValue(0);
        m_thread->setUrl(ui->lineEditUrl->text());
        m_thread->setStartNum(ui->spinBoxStart->value());
        m_thread->setEndNum(ui->spinBoxEnd->value());
        m_thread->start();
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
