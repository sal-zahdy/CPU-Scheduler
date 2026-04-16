#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QMessageBox>
#include <list>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    ui->quantumLabel->setVisible(false);
    ui->quantumSpinBox->setVisible(false);

    // initial rows
    ui->processTable->setRowCount(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 🔴 Algorithm selection
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

// 🔴 SpinBox controls rows
void MainWindow::on_processNumberSpinBox_valueChanged(int value)
{
    if (!isDynamic) {
        ui->processTable->setRowCount(value);
    } else {
        // dynamic → extra row for new process
        ui->processTable->setRowCount(value + 1);
    }
}

// 🔴 Dynamic checkbox
void MainWindow::on_dynamicCheckBox_toggled(bool checked)
{
    isDynamic = checked;

    int n = ui->processNumberSpinBox->value();

    if (!isDynamic) {
        ui->processTable->setRowCount(n);
    } else {
        ui->processTable->setRowCount(n + 1); // extra input row
    }
}

// 🔴 Add button → ADD to LINKED LIST + redraw
void MainWindow::on_addProcessButton_clicked()
{
    if (!isDynamic) {
        QMessageBox::warning(this, "Error",
                             "Enable dynamic mode first!");
        return;
    }

    int lastRow = ui->processTable->rowCount() - 1;

    QTableWidgetItem *arrivalItem = ui->processTable->item(lastRow, 1);
    QTableWidgetItem *burstItem   = ui->processTable->item(lastRow, 2);

    if (!arrivalItem || !burstItem) {
        QMessageBox::warning(this, "Error", "Fill the new process data!");
        return;
    }

    Process p;
    p.id = processList.size() + 1;
    p.arrival = arrivalItem->text().toInt();
    p.burst   = burstItem->text().toInt();
    p.priority = 0;

    // Priority
    if (algorithmType == 3 || algorithmType == 4) {
        QTableWidgetItem *priorityItem =
            ui->processTable->item(lastRow, 3);

        if (priorityItem)
            p.priority = priorityItem->text().toInt();
    }

    // ✅ CORRECT VALIDATION
    if (p.arrival > seconds) {
        QMessageBox::warning(this, "Error",
                             "Arrival time cannot be greater than current time!");
        return;
    }

    // 🔥 ADD to linked list
    processList.push_back(p);

    // 🔥 SORT again
    processList.sort([](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    // 🔥 ADD NEW EMPTY ROW for next process
    ui->processTable->insertRow(ui->processTable->rowCount());

    // 🔥 UPDATE GANTT immediately
    drawGantt();
}

// 🔴 Start button
void MainWindow::on_startButton_clicked()
{
    seconds = 0;
    ui->lcdNumber->display("00:00:00");

    if (!timer->isActive())
        timer->start(1000);

    processList.clear();

    int rows = ui->processTable->rowCount();

    for (int i = 0; i < rows; i++) {

        QTableWidgetItem *arrivalItem = ui->processTable->item(i, 1);
        QTableWidgetItem *burstItem   = ui->processTable->item(i, 2);

        if (!arrivalItem || !burstItem)
            continue;

        Process p;
        p.id = i + 1;
        p.arrival = arrivalItem->text().toInt();
        p.burst   = burstItem->text().toInt();
        p.priority = 0;

        if (algorithmType == 3 || algorithmType == 4) {
            QTableWidgetItem *priorityItem =
                ui->processTable->item(i, 3);

            if (priorityItem)
                p.priority = priorityItem->text().toInt();
        }

        processList.push_back(p);
    }

    processList.sort([](const Process &a, const Process &b) {
        return a.arrival < b.arrival;
    });

    drawGantt();
}

// 🔴 Gantt drawing (separated for reuse)
void MainWindow::drawGantt()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->ganttView->setScene(scene);

    int x = 0;
    int scale = 40;
    int currentTime = 0;

    scene->addText("0")->setPos(0, 60);

    for (auto p : processList) {

        int width = p.burst * scale;

        scene->addRect(x, 0, width, 50);

        scene->addText("P" + QString::number(p.id))
            ->setPos(x + width / 3, 10);

        currentTime += p.burst;

        scene->addText(QString::number(currentTime))
            ->setPos(x + width, 60);

        x += width;
    }
}

// 🔴 Timer
void MainWindow::updateTimer()
{
    seconds++;

    int hrs = seconds / 3600;
    int mins = (seconds % 3600) / 60;
    int secs = seconds % 60;

    QString timeStr = QString("%1:%2:%3")
                          .arg(hrs, 2, 10, QChar('0'))
                          .arg(mins, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0'));

    ui->lcdNumber->display(timeStr);
}