#include <QMainWindow>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QDebug>
#include <QNetworkDatagram>
#include <QTimeZone>
#include <QColor>
#include <time.h>
#include "mainwindow.h"
#include "qcustomplot.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    srand(time({}));
    resize(800, 600);

    //Widnow management
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    Layout = new QVBoxLayout(central);
    plot = new QCustomPlot(this);
    Layout->addWidget(plot);

    //Plot setting
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Usage (%)");
    plot->yAxis->setRange(0, 100);

    //legend settings
    QFont legendFont = font();
    legendFont.setPointSize(9);
    plot->legend->setFont(legendFont);
    plot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    plot->legend->setVisible(true);

    //time management
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setTimeZone(QTimeZone::systemTimeZone());
    dateTicker->setDateTimeFormat("hh:mm:ss");
    plot->xAxis->setTicker(dateTicker);

    //socket
    udpSocket = new QUdpSocket(this);
    bool ok = udpSocket->bind(QHostAddress::LocalHost, 1234);
    if(!ok)
    {
        qDebug() << "Binding failed: " << udpSocket->errorString();
    }

    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

void MainWindow::SetCoresVisibility(bool Check)
{
    if(plot->graphCount() < 1) return;
    if(Check)
    {
        for(int i = 1; i < plot->graphCount(); i++)
        {
            bool Visibility = CoresVisibilityCheckBoxes[i - 1]->isChecked();
            plot->graph(i)->setVisible(Visibility);
            ConfigurateLegend();
        }
    }
    else
    {
        for(int i = 1; i < plot->graphCount(); i++)
        {
            plot->graph(i)->setVisible(false);
            plot->graph(i)->removeFromLegend();
        }
    }
}

void MainWindow::ConfigurateLegend()
{
    for (int i = 0; i < plot->graphCount(); i++) {plot->graph(i)->removeFromLegend();}
    for(int i = 0; i < plot->graphCount(); i++)
    {
        auto * Graph = plot->graph(i);
        if(Graph->visible())
        {
            plot->legend->addItem(new QCPPlottableLegendItem(plot->legend, plot->graph(i)));
        }
    }
}

static QColor GenerateRGB()
{
    return QColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, CORES_COLOR_ALPHA);
}

void MainWindow::AddGraph()
{
    plot->addGraph();
    if(plot->graphCount() == 1)
    {
        //CPU Checkbox settings
        plot->graph(0)->setName("CPU");
        CPU_VisibilityCheckBox = new QCheckBox("CPU", this);
        Layout->addWidget(CPU_VisibilityCheckBox);
        CPU_VisibilityCheckBox->setChecked(true);
        QPen Pen(QColor(0, 0, 0, 255));
        Pen.setWidth(GRAPH_LINE_WIDTH);
        plot->graph(0)->setPen(Pen);
        connect(CPU_VisibilityCheckBox, &QCheckBox::toggled, this, [=](bool checked)
        {
            plot->graph(0)->setVisible(checked);
            if(checked) {ConfigurateLegend();}
            else        {plot->graph(0)->removeFromLegend();}
            plot->replot();
        });

        //Show cores CheckBox settings
        ShowCoresCheckBox = new QCheckBox("Show cores", this);
        Layout->addWidget(ShowCoresCheckBox);
        ShowCoresCheckBox->setChecked(true);
        connect(ShowCoresCheckBox, &QCheckBox::toggled, this, &MainWindow::SetCoresVisibility);
        return;
    }

    //Core CheckBox settings
    QCPGraph * Graph = plot->graph(plot->graphCount() - 1);
    QString Name = QString("Core ") + QString::number(plot->graphCount() - 1);
    Graph->setName(Name);

    QPen Pen(GenerateRGB());
    Pen.setWidth(GRAPH_LINE_WIDTH);
    Graph->setPen(Pen);

    QCheckBox * CB = new QCheckBox(Name, this);
    CB->setChecked(true);
    CoresVisibilityCheckBoxes.push_back(CB);
    Layout->addWidget(CB);
    connect(CB, &QCheckBox::toggled, this, [=](bool checked)
    {
        if(ShowCoresCheckBox->isChecked())
        {
            Graph->setVisible(checked);
            if(checked) {ConfigurateLegend();}
            else        {Graph->removeFromLegend();}
            plot->replot();
        }
    });
}

void MainWindow::ManageGraphsNum(int Num)
{
    if(Num > plot->graphCount())
    {
        int Temp = plot->graphCount();
        for(int i = 0; i < Num - Temp; i++) {AddGraph();}
    }
}

void MainWindow::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram Datagram = udpSocket->receiveDatagram();
        QByteArray Data = Datagram.data();
        int Count = Data.size() / sizeof(float);
        const float* Values = reinterpret_cast<const float *>(Data.constData());

        double now = QDateTime::currentDateTime().toTime_t();
        ManageGraphsNum(Count);
        for(int i = 0; i < plot->graphCount(); i++) {plot->graph(i)->addData(now, Values[i]);}
        plot->xAxis->setRange(now - DELTA_TIME, now);
        plot->replot();
    }
 }
