#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QImage>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

#include <iostream>
#include <string>

using namespace std;
using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    QImage img01, img02, imgAfter, imgNew;
    int img01Array[64][64], img02Array[64][64];
    ~MainWindow();

private slots:
    void openImg01();
    void openImg02();
    void addPixel();
    void substractPixel();
    void multiplyPixel();
    void dividePixel();
    void averageImg();
    void specialImg();

private:
    int checkPixel(int pixel);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
