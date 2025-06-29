#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QVBoxLayout>
#include "qcustomplot.h"

#define CORES_COLOR_ALPHA 200
#define GRAPH_LINE_WIDTH 3
#define DELTA_TIME 50

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
    void ManageGraphsNum(int Num);
    void AddGraph();
    void ConfigurateLegend();

private slots:
    void SetCoresVisibility(bool Check);
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    QVBoxLayout * Layout;
    QCustomPlot *plot;
    QCheckBox * CPU_VisibilityCheckBox;
    QCheckBox * LegendVisibilityCheckBox;
    QCheckBox * ShowCoresCheckBox;
    QVector<QCheckBox*> CoresVisibilityCheckBoxes;
};

#endif
