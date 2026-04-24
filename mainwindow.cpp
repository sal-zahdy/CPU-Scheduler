#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QMessageBox>

#include <iostream>
using namespace std;

LinkedList mohsen;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    ui->quantumLabel->setVisible(false);
    ui->quantumSpinBox->setVisible(false);

    ui->processTable->setRowCount(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_algorithmBox_currentIndexChanged(int index)
{
    algorithmType = index;

    bool isPriority = (index == 3 || index == 4);

    if (isPriority) {
        ui->processTable->setColumnCount(4);
        ui->processTable->setHorizontalHeaderLabels(
            {"ID", "Arrival", "Burst", "Priority"});
    } else {
        ui->processTable->setColumnCount(3);
        ui->processTable->setHorizontalHeaderLabels(
            {"ID", "Arrival", "Burst"});
    }

    ui->quantumLabel->setVisible(index == 5);
    ui->quantumSpinBox->setVisible(index == 5);
}

void MainWindow::on_processNumberSpinBox_valueChanged(int value)
{
    if (!isDynamic)
        ui->processTable->setRowCount(value);
    else
        ui->processTable->setRowCount(value + 1);
}

void MainWindow::on_dynamicCheckBox_toggled(bool checked)
{
    isDynamic = checked;
    int n = ui->processNumberSpinBox->value();
    if (!isDynamic)
        ui->processTable->setRowCount(n);
    else
        ui->processTable->setRowCount(n + 1);
}

void MainWindow::on_addProcessButton_clicked()
{
    if (!isDynamic) {
        QMessageBox::warning(this, "Error", "Enable dynamic mode first!");
        return;
    }

    int lastRow = ui->processTable->rowCount() - 1;

    QTableWidgetItem *arrivalItem = ui->processTable->item(lastRow, 1);
    QTableWidgetItem *burstItem   = ui->processTable->item(lastRow, 2);

    if (!arrivalItem || !burstItem) {
        QMessageBox::warning(this, "Error", "Fill the new process data!");
        return;
    }

    double burst    = burstItem->text().toDouble();
    int    priority = 0;

    if (algorithmType == 3 || algorithmType == 4) {
        QTableWidgetItem *priorityItem = ui->processTable->item(lastRow, 3);
        if (priorityItem)
            priority = priorityItem->text().toInt();
    }

    waitingPool.insertAtTail(burst, priority, universalTime);

    ui->processTable->insertRow(ui->processTable->rowCount());

    cout << "[DYNAMIC ADD] burst=" << burst
         << " arrival=" << universalTime
         << " priority=" << priority << endl;

    waitingPool.print();
}

void MainWindow::on_startButton_clicked()
{
    quantumTime      = (int)(ui->quantumSpinBox->value() / 0.05);
    universalTime    = 0;
    totalTurnaround  = 0;
    totalWaiting     = 0;
    finishedCount    = 0;
    ganttLog.clear();
    waitingPool.clear();
    currentGanttNode = nullptr;
    mohsen.clear();

    isPaused = false;
    ui->pauseButton->setText("Pause");

    seconds = 0;
    ui->lcdNumber->display("00:00:00");
    ui->turnaroundLabel->setText("Turnaround Time: -");
    ui->waitingLabel->setText("Waiting Time: -");

    if (timer->isActive())
        timer->stop();

    timer->start(50);

    int rows = ui->processTable->rowCount();
    int pid  = 1;

    for (int i = 0; i < rows; i++) {

        QTableWidgetItem *arrivalItem = ui->processTable->item(i, 1);
        QTableWidgetItem *burstItem   = ui->processTable->item(i, 2);

        if (!arrivalItem || !burstItem)
            continue;

        double arrival  = arrivalItem->text().toDouble();
        double burst    = burstItem->text().toDouble();
        int    priority = 0;

        if (algorithmType == 3 || algorithmType == 4) {
            QTableWidgetItem *priorityItem = ui->processTable->item(i, 3);
            if (priorityItem)
                priority = priorityItem->text().toInt();
        }

        waitingPool.insertAtTail(burst, priority, arrival);

        cout << "Added to pool: P" << pid++
             << " arrival=" << arrival
             << " burst="   << burst
             << " pri="     << priority << endl;
    }

    waitingPool.print();
    drawGantt();
}

void MainWindow::drawGantt()
{
    QGraphicsScene *scene = ui->ganttView->scene();
    if (!scene) {
        scene = new QGraphicsScene(this);
        ui->ganttView->setScene(scene);
    }
    scene->clear();

    int x     = 0;
    int scale = 40;

    Node* cur = ganttLog.head;
    while (cur) {
        int width = (int)(cur->burst * scale);
        if (width < 1) width = 1;

        scene->addRect(x, 0, width, 50);
        scene->addText("P" + QString::number(cur->id))
            ->setPos(x + width / 3, 10);
        scene->addText(QString::number(cur->arrival, 'f', 1))
            ->setPos(x, 60);

        x += width;
        cur = cur->next;
    }

    if (finishedCount > 0) {
        double avgTurnaround = totalTurnaround / finishedCount;
        double avgWaiting    = totalWaiting    / finishedCount;

        ui->turnaroundLabel->setText(
            "Turnaround Time: " + QString::number(avgTurnaround, 'f', 2));
        ui->waitingLabel->setText(
            "Waiting Time: " + QString::number(avgWaiting, 'f', 2));
    }
}

void MainWindow::updateTimer()
{
    seconds += 0.05;

    int hrs  =  seconds / 3600;
    int mins = ((int)seconds % 3600) / 60;
    int secs =  (int)seconds % 60;

    QString timeStr = QString("%1:%2:%3")
                          .arg(hrs,  2, 10, QChar('0'))
                          .arg(mins, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0'));

    ui->lcdNumber->display(timeStr);

    if (algorithmType == 5)
        step(&mohsen, algorithmType, ui->quantumSpinBox->value());
    else
        step(&mohsen, algorithmType);

    drawGantt();
}

void MainWindow::on_pauseButton_clicked()
{
    if (isPaused) {
        timer->start(50);
        isPaused = false;
        ui->pauseButton->setText("Pause");
    } else {
        timer->stop();
        isPaused = true;
        ui->pauseButton->setText("Resume");
    }
}