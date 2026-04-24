#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "LinkedList.h"

#include <QMainWindow>
#include <QTimer>

extern LinkedList ganttLog;
extern Node* currentGanttNode;
extern double universalTime;
extern int quantumTime;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
private:
    int algorithmType = 0; // x variable
    bool isDynamic = false;
    int quantum = 0;
    bool isPaused = false;



    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;


private:
    QTimer *timer;
    double seconds = 0;

private slots:
    void updateTimer();
    void on_algorithmBox_currentIndexChanged(int index);
    void on_dynamicCheckBox_toggled(bool checked);
    void on_addProcessButton_clicked();
    void drawGantt();
    void on_processNumberSpinBox_valueChanged(int value);
    void on_pauseButton_clicked();



};


#endif // MAINWINDOW_H
