#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

# include <QMainWindow>
# include <QFileDialog>
# include <iostream>
# include <algorithm>
# include <sstream>
# include <cstdlib>
# include <string>
# include <math.h>
# define PI acos(-1)
using namespace std;

# include <opencv2/highgui/highgui.hpp>
# include <opencv2/core/core.hpp>
# include <opencv2/imgproc.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    int imgCols, imgRows, maskSize, orderMaskSize, pixelB, pixelG, pixelR, pixel;
    double BGRCoefficient, sigma;

    String coefficientsString, splitResult;
    Mat MatImgIn, MatForZero;
    QImage QImgIn, QImgOut;
    vector<int> pixelSeriesB, pixelSeriesG, pixelSeriesR;
    vector<double> coefficientsDouble;

    int pixelCheck(int index);

private slots:
    void openImg();
    void convImg();
    void medianImg();
    void minImg();
    void maxImg();
    void LoGImg();
    void MarrHildrethImg();

};

#endif // MAINWINDOW_H
